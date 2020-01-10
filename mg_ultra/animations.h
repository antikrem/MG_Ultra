/*Defines methods for animations,
*/

#ifndef __ANIMATION_STATE__
#define __ANIMATION_STATE__

#include <vector>
#include <map>
#include <algorithm> 

#include "constants.h"
#include "cus_struct2.h"

//has enough information to get a boxdata 
//Needs an AnimationSetTemplate to actually have meaning
struct AnimationState {
	bool visible = true;
	bool valid = true; 
	Point3 centerPostion; // invalid until set by position component
	float scale = 1.0f;
	float wrapFactor = 1.0f;
	float rotation = 0; 
	float transparency = 1.0f;
	string animationSetName;
	unsigned int animationType;
	unsigned int currentFrame; //the index of the frame in animation
	unsigned int frameskipCounter; //recurring frameskip counter
	bool inSingleAnimation = false; //in a single animation

	float lightSensitivity = 1.0f;
	float minimumAmbient = 0.0f;

	//two floats that are used as additional offset
	//Used by system graphics when creating positioned state
	Point3 offset;

	AnimationState() : centerPostion(0), offset(0) {
	}

	//a prefered constructor that garantees a wellformed animation set
	AnimationState(string name, unsigned int type, unsigned int frame, unsigned int frameskip) 
			: centerPostion(0), offset(0) {
		animationSetName = name;
		animationType = type;
		currentFrame = frame;
		frameskipCounter = frameskip;
	}

	//copy constructor
	AnimationState(const AnimationState& state)
		    : centerPostion(state.centerPostion), offset(state.offset) {
		this->visible = state.visible;
		this->valid = state.valid;
		this->scale = state.scale;
		this->wrapFactor = state.wrapFactor;
		this->rotation = state.rotation;
		this->transparency = state.transparency;
		this->animationSetName = state.animationSetName;
		this->animationType = state.animationType;
		this->currentFrame = state.currentFrame;
		this->frameskipCounter = state.frameskipCounter;
		this->inSingleAnimation = state.inSingleAnimation;

		this->lightSensitivity = state.lightSensitivity;
		this->minimumAmbient = state.minimumAmbient;
	}

	//simple function to reset to the start of an animation
	void resetAnimation() {
		currentFrame = 0;
		frameskipCounter = 0;
	}

	//simple function to reset to the start of an animation
	//can also set the animation type
	void resetAnimation(unsigned int animationType) {
		this->animationType = animationType;
		currentFrame = 0;
		frameskipCounter = 0;
	}
};

//Different access values for textures
//Three textures are stored within the program
//And loaded at different times
enum TextureAccess {
	TA_global, //Loaded at start of the program and unloaded until the end
	TA_campaign, //Loaded when going from campaign to campaign
	TA_level, //Loaded for only the level
};
#define TA_LENGTH 3

//Stores UV position data, specifically, the centre index of the texture and the width/height of the frame
//specifically, this is done such that indexing is at the middle of the pixel
struct UVWH {
	float u;
	float v;
	float w;
	float h;

	UVWH(float u, float v, float w, float h) {
		this->u = u;
		this->v = v;
		this->w = w;
		this->h = h;
	}
};

//A template for animation that will be stores by animationSetTemplate
class AnimationTemplate {
	int numberOfFrames = 0;
	int width, height;
	float fwidth, fheight; //width and height as a fraction of texture size
	int frameskip; //number of cycles a frame of this animation is shown for
	vector<UVWH> uvwhs; //Location within the texture where the current frame is
public:
	void operator=(const AnimationTemplate &t) {
		numberOfFrames = t.numberOfFrames;
		width = t.width;
		height = t.height;
		fwidth = t.fwidth;
		fheight = t.fheight;
		frameskip = t.frameskip;
	}

	AnimationTemplate(int width, int height, int texWidth, int texHeight,int frameskip) {
		this->width = width;
		this->height = height;
		this->fwidth = (float)width / (float)texWidth;
		this->fheight = (float)height / (float)texHeight;
		this->frameskip = frameskip;
	}

	AnimationTemplate() {
	}

	int getNumberOfFrames() {
		return numberOfFrames;
	}

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	float getFWidth() {
		return fwidth;
	}

	float getFHeight() {
		return fheight;
	}

	int getFrameskip() {
		return frameskip;
	}

	void addUVWH(int u, int v, int width, int height, int tWidth, int tHeight) {
		numberOfFrames++;

		uvwhs.push_back(
			UVWH (
			    ((float)u+((float)width/2))/(float)tWidth, //center u
				((float)v+((float)height/2))/(float)tHeight, //center v
				(float)(width-1)/ tWidth, //u width
				(float)(height-1)/ tHeight //v height
			)
		);
	}

	//Will throw on incorrect n
	UVWH getUVWH(unsigned int n) {
		return uvwhs[n];
	}
};

//A template for animation that will be stored by animationSetTemplate
class AnimationSetTemplate {
	int texture; //Specifies which texture this set is stored in
	int maxHeight = 0; //max height of animation, zero means uncached
	TextureAccess access; //Specified the texture access of the base texture, which specifies when to clear this set
	map<unsigned int, AnimationTemplate*> animationTemplates;
public:
	//sets the access and the texture index
	AnimationSetTemplate(TextureAccess access, int texture) {
		this->access = access;
		this->texture = texture;
	}

	//TODO: check if there is already an element
	void addAnimationTemplate(unsigned int type, AnimationTemplate* ptr) {
		animationTemplates[type] = ptr;
	}

	//get the animation template specified, no error checking
	AnimationTemplate* getAnimationTemplate(unsigned int animationType) {
		return animationTemplates.count(animationType) ? animationTemplates[animationType] : nullptr;
	}

	//Will attempt to find a height value for any animation in this set
	//if the animationset is empty, will return 0
	int getFirstHeightValue() {
		if (maxHeight == 0) {
			//cache height
			for (auto i : animationTemplates) {
				maxHeight = max(i.second->getHeight(), maxHeight);
			}
		}
		return maxHeight;
	}

	bool checkTemplate(int frame) {
		return (bool)animationTemplates.count(frame);
	}

};



#endif

