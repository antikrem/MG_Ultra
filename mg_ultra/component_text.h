/*A Component that is used for rendering text, including allignment
Implementation is done by using an standard unified graphics layout
The font used is the animation set,
each animation type is a character
first uv is the topleft index of the character
Should be used in conjunction with ComponentPosition
*/
#ifndef __COMPONENT_TEXT__
#define __COMPONENT_TEXT__

#include "component.h"
#include "str_kit.h"

#include "texture.h"

#include "scriptable_class.h"

enum AlignmentText {
	AT_TopLeft,
	AT_MiddleLeft,
	AT_BottomLeft,
	AT_TopCentre,
	AT_MiddleCentre,
	AT_BottomCentre,
	AT_TopRight,
	AT_MiddleRight,
	AT_BottomRight
};

class ComponentText : public Component, public ScriptableClass {
	//Only rendered if true
	atomic<bool> visible = true;

	//cache lock
	mutex lock;

	//scales the text
	atomic<float> scale = 1.0f;

	//text and font change lock change mutex
	mutex textLock;
	//The string the text represents
	string text;
	//The name of the "font", which will be the animationSet name
	string animationSet;

	//set to true if this is rendered in 3d
	atomic<bool> renderIn3D = true;

	//Alignment that will be used with ComponentPosition to specify position
	AlignmentText alignmentText;
	//The internal padding, x is horizontal between characters, y is vertical between lines
	Point2 padding;

	//States will be cached to reduce performance requirements
	vector<AnimationState> stateCache;
	//If false, the state cache is outdated and needs to be updated
	atomic<bool> validCache = true;

	//converts a char to a character index
	//returns -1 if char is not convertable
	static int char2Ani(char character) {
		return ((int)character < 32 || (int)character > 126) ? -1 : ((int)character - 31);
	}

	//create the cache
	vector<AnimationState> createCache(AnimationsMaster* animationsMaster) {
		Point2 runningOffset(0, 0); //running offset
		Point2 padding(0, 0);
		vector<AnimationState> cacheBuffer; //Buffer that is used to cache
		string text;
		string fontName;

		{
			lock_guard<mutex>lck(textLock);
			text = this->text;
			fontName = animationSet;
			padding = this->padding;
		}

		//lock
		animationsMaster->lockShared();
		auto aniSet = animationsMaster->getAnimationSetTemplate(fontName);
		if not(aniSet) {
			animationsMaster->unlockShared();
			return cacheBuffer;
		}

		//set vertical buffer
		float verticalBuffer = (float)aniSet->getFirstHeightValue();

		for (unsigned int i = 0; i < text.length(); i++) {
			//check for control strings
			if (str_kit::stringCompareAtLocation(text, i, string("\n"))) {
				runningOffset.x = 0;
				runningOffset.y -= scale * verticalBuffer;
				runningOffset.y -= scale * padding.y;
			}
			//else, append the character with an offset, and update running offset
			else if (char2Ani(text[i]) > -1) {
				AnimationState state;
				state.animationSetName = fontName;
				state.animationType = char2Ani(text[i]);
				state.currentFrame = 0;
				state.offset = Point3(runningOffset, 0);
				//update offset
				auto aniTemp = aniSet->getAnimationTemplate(state.animationType);
				if not(aniTemp) {
					continue;
				}

				//todo report error
				runningOffset += Point2((float)aniTemp->getWidth(), 0) * scale.load();
				runningOffset.x += scale * padding.x;
				cacheBuffer.push_back(state);
			}
			//todo catch error case

		}

		animationsMaster->unlockShared();
		return cacheBuffer;
	}

public:
	ComponentText(float paddingX, float paddingY) : padding(paddingX, paddingY) {

	}

	ComponentText() : padding(0, 0) {

	}

	ComponentText(string font) : padding(0, 0) {
		setFont(font);
	}

	bool getVisible() {
		return visible.load();
	}

	void setVisible(bool visible) {
		this->visible.store(visible);
	}

	//What needs to be called by system_text
	void textUpdate(AnimationsMaster* animationsMaster) {
		//only necessary if the chache is invalid
		if not(validCache.load()) {
			auto cacheBuffer = createCache(animationsMaster);
			lock_guard<mutex> lck(lock);
			stateCache = cacheBuffer;
			validCache.store(true);
		}
	}

	vector<AnimationState> getStates(Point3 position) {
		vector<AnimationState> buffer;
		vector<AnimationState> buffer1;
		{
			lock_guard<mutex> lck(lock);
			buffer = stateCache;
		}
		for (AnimationState i : buffer) {
			AnimationState toPush = i;
			toPush.centerPostion = position + i.offset;
			buffer1.push_back(toPush);
		}
		return buffer1;
	}

	void setText(string text) {
		lock_guard<mutex>lck(textLock);
		this->text = text;
		validCache.store(false);
	}

	void setFont(string fontName) {
		lock_guard<mutex>lck(textLock);
		this->animationSet = fontName;
		validCache.store(false);
	}

	void setPadding(float paddingX, float paddingY) {
		lock_guard<mutex>lck(textLock);
		padding = Point2(paddingX, paddingY);
		validCache.store(false);
	}

	void setScale(float scale) {
		validCache.store(false);
		this->scale = scale;
	}

	float getScale() {
		return scale;
	}

	//set render in 3d
	void setRenderIn3D(bool renderIn3D) {
		this->renderIn3D = renderIn3D;
	}

	//get render in 3d
	bool getRenderIn3D() {
		return renderIn3D;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentText"].setClass(kaguya::UserdataMetatable<ComponentText, Component>()
			.setConstructors<ComponentText()>()
			.addFunction("set_visible", &ComponentText::setVisible)
			.addFunction("set_text", &ComponentText::setText)
			.addFunction("set_scale", &ComponentText::setScale)
			.addFunction("get_scale", &ComponentText::getScale)
			.addFunction("set_font", &ComponentText::setFont)
			.addFunction("set_render_in_3D", &ComponentText::setRenderIn3D)
			.addFunction("get_render_in_3D", &ComponentText::getRenderIn3D)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentText, string>
			)
			.addStaticFunction("type", &getType<ComponentText>)
			.addStaticFunction("cast", &Component::castDown<ComponentText>)
		);
	}

};

#endif
