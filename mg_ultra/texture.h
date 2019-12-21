/*Handles loading textures,
All textures are stored in closed-source(tm), properiety(tm) by Chalinda Rodrigo(tm)(1999-2019),
known as mg(???) textures: .mgt
*/

#ifndef __TEXTURE__
#define __TEXTURE__

#include <atomic>
#include <shared_mutex>
#include <string>
#include <tuple>

#include "constants.h"

#include "animations.h"
#include "reader.h"

#include "shaders.h"

//maximum unumber of textures
#define MAX_TEXTURE_COUNT 16

//Header Identifier
#define MGTID "mgt!KOH!"
#define MGTEND "mgt!END!"

#define ANI_TABLE_START "ANITABLE"
#define ANI_TABLE_END "ENDTABLE"
#define ANISET_LENGTH 24

#define ANI_SET_START "aniset"
#define ANI_START "anifra"
#define UV_START "UV"

#define DATA_HEADER_START "DATAHEAD"
#define DATA_HEADER_END "DATATAIL"

//A structure that has a minimum amount of information required to render a box
struct BoxData {
	//3 floats of center point (x,y,z)
	float xyz[3];
	//2 sizes (x,y)
	float wh[2];
	//4 texture coordinates (u,v) is center, (w,l) is width and length
	float uvwh[4];
	//rotation amount in radians
	float rotation = 0;
	//Used to overshoot wrapping
	float wrapFactor = 1.0f;
	//set to true if this box is valid
	bool draw;
};

//Encapsulates a texture loaded
//todo set status if size is too big
class Texture {
	//gl texture id
	GLuint textureID;

	int texWidth, texHeight;
public:
	Texture(Reader* file) {
		//size of whole texture
		unsigned int width, height, mipMapCount, linearSize;

		//set info
		width = file->interpretIntBytes(4);
		height = file->interpretIntBytes(4);
		mipMapCount = file->interpretIntBytes(4);
		linearSize = file->interpretIntBytes(4);

		//get max size of file
		GLint mSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mSize);

		//Check the size of width == height
		if not(width == height) {
			//todo set flag
		}

		if (width >= (unsigned int) mSize) {
			//todo set flag
		}

		texWidth = (int)width;
		texHeight = (int)height;

		unsigned int size = mipMapCount > 1 ? linearSize * 2 : linearSize;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		//go through each mipmap and load
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*16;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, width, height,
				0, size, file->directDataAccess());

			file->incrementPointer(size);
			width /= 2;
			height /= 2;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	GLint getTextureID() {
		return textureID;
	}

	int getWidth() {
		return texWidth;
	}

	int getHeight() {
		return texHeight;
	}

	//binds this texture to the correct texture unit
	void bindTexture(int TU) {
		//bind last steps
		glActiveTexture(GL_TEXTURE0 + TU);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
};

//Master that handles textures and animation
//Will load all textures and animation set to memory,
//Which can be accessed later through getAnimationSet
class AnimationsMaster {
private:
	//Map of currently loaded textures, index to texture*
	map<int, Texture*> loadedTextures;

	//read write lock, write lock when adding or removing mgt files, resulting in loadedTextures changing
	shared_mutex mtx;

	//map of animation set templates
	map<string, AnimationSetTemplate*> animationSets;

	//Adds an animationSet TODO: check error
	//only use in context of a unique aquisition of mtx
	void addSet(string name, AnimationSetTemplate* ptr) {
		animationSets[name] = ptr;
	}

	//Adds an animation to an aniset TODO: check error
	//only use in context of a unique aquisition of mtx
	void addAnimation(unsigned int animationType, AnimationSetTemplate* asptr, AnimationTemplate* atptr) {
		asptr->addAnimationTemplate(animationType, atptr);
	}

	//gets the lowest texture unit that isn't allocated
	//returns -1 if 16 texture units have been allocated
	int getFreeTextureIndex() {
		for (int i = 0; i < 16; i++) {
			if not(loadedTextures.count(i)) {
				return i;
			}
		}
		return -1;
	}

public:
	AnimationsMaster();

	//loads a mgt, which has a texture atlas and all associated animation
	//todo improve by returning enumerated error code
	int loadMGT(string location, TextureAccess textureAccess) {
		Reader file(location);
		unique_lock<shared_mutex> lock(mtx);

		//Check reader opened
		if (FAILURE(file.getStatus()))
			return EXIT_FAILURE;

		//check ID
		if not(file.checkSection(MGTID))
			return EXIT_FAILURE;

		//First section is the data section, which pulls texture atlas 
		if (file.checkSection(DATA_HEADER_START)) {
			//TODO file format error
		}

		//TODO -1/++ can be made a preqrement
		//data section is handled by sending to a newly created TU
		int fullWidth, fulllHeight;
		int index = getFreeTextureIndex();
		loadedTextures[index] = new Texture(&file);
		fullWidth = loadedTextures[index]->getWidth();
		fulllHeight = loadedTextures[index]->getHeight();

		if not(file.checkSection(DATA_HEADER_END)) {
			//TODO file format error
		}
		
		//start reading animation stuff
		if not(file.checkSection(ANI_TABLE_START)) {
			
		}
		
		AnimationSetTemplate* currentSet = NULL;
		AnimationTemplate* currentAnimation = NULL;

		while not(file.checkSection(ANI_TABLE_END)) {
			//Inside of animationset seek loop
			if (file.checkSection(ANI_SET_START)) {
				string name = file.getTerminatedString(ANISET_LENGTH);
				currentSet = new AnimationSetTemplate(textureAccess, index);
				addSet(name, currentSet);
			}
			else if (file.checkSection(ANI_START)) {
				unsigned int aniType = file.interpretIntBytes(1);
				int frameskip = file.interpretIntBytes(2);
				int width = file.interpretIntBytes(2);
				int height = file.interpretIntBytes(2);
				currentAnimation = new AnimationTemplate(width, height, fullWidth, fulllHeight, frameskip);
				addAnimation(aniType, currentSet, currentAnimation);
				//Go through uvs
				while (file.checkSection(UV_START)) {
					unsigned int u = file.interpretIntBytes(2);
					unsigned int v = file.interpretIntBytes(2);
					currentAnimation->addUVWH(u, v, width, height, fullWidth, fulllHeight);
				}
			}
		}

		//last id
		if not(file.checkSection(MGTEND)) {
		
		}

		return loadedTextures[index]->getTextureID();
	}

	//takes an animation state and returns boxdata
	//error checking here is superflous todo
	BoxData evaluateToBox(AnimationState state, float scale) {
		BoxData boxData;

		shared_lock<shared_mutex> lock(mtx);

		//check if this state exists
		//if it doesn't simply return an uninitalised empty box
		if (animationSets.count(state.animationSetName) == 0) {
			boxData.draw = 0;
			return boxData;
		}
		//current animation template
		auto ani = animationSets[state.animationSetName]->getAnimationTemplate(state.animationType);
		//if animation is invalid, set draw to false
		if (!ani) {
			boxData.draw = 0;
			return boxData;
			//todo report
		}
		boxData.draw = state.visible && state.valid;

		//populate box data
		memcpy(boxData.xyz, &state.centerPostion, 3*sizeof(float));
		boxData.wh[0] = scale * state.scale*(float)ani->getWidth();
		boxData.wh[1] = scale * state.scale*(float)ani->getHeight();
		boxData.rotation = DEG2RAD(state.rotation);
		memcpy(boxData.uvwh, &ani->getUVWH(state.currentFrame), 4 * sizeof(float));
		boxData.wrapFactor = state.wrapFactor;
		
		return boxData;
	}

	//gets an animation set tempalre, returns null if given animation doesn't exist
	AnimationSetTemplate* getAnimationSetTemplate(string name) {
		return animationSets[name];
	}

	//prints all the sets
	string print() {
		shared_lock<shared_mutex> lock(mtx);
		string str;
		for (auto i : animationSets) {
			vector<string> branches;
			
		}
		return str;
	}

	//lock shared, 
	void lockShared() {
		mtx.lock_shared();
	}

	//unlock shared, 
	void unlockShared() {
		mtx.unlock_shared();
	}

	//Takes a set and frame number, and populates a template with all but uvwhs
	//returns true if operation successful
	bool populateTemplate(string setName, unsigned int animationType, AnimationTemplate* animationTemplate) {
		if not(animationTemplate) {
			return false;
		}

		shared_lock<shared_mutex> lock(mtx);
		
		//check set exists
		auto set = animationSets[setName];
		if not(set) {
			return false;
		}

		//check template exists
		if not(set->checkTemplate(animationType)) {
			return false;
		}
		
		//set values
		*animationTemplate = *set->getAnimationTemplate(animationType);

		return true;
	}

	//attaches all current textures to the target shader
	//returns next texture unit to chain
	int attachTextures(ShaderMaster* shaderMaster, string programName, string textureVariableTargetName) {
		//vector of texture units to use
		vector<int> tuList;
		//bind to texture units
		int i = 0;
		for (; i < (int)loadedTextures.size(); i++) {
			loadedTextures[i]->bindTexture(i);
			tuList.push_back(i);
		}
		shaderMaster->setNUniformI(programName, textureVariableTargetName, (int)tuList.size(), tuList);

		return i;
	}

};

namespace g_aniquery {
	//returns (-1, -1) if animationSet/animationType doesn't exist
	tuple<int, int> getAnimationSize(string animationSet, int animationType);
}

#endif