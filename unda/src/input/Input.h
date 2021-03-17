#pragma once
#include "KeyCodes.h"

namespace unda {
	class Input { 
	public:
		inline static bool isKeyDown(int keyCode) { return singletonInstance->isKeyDownImplementation(keyCode); }
		inline static bool isKeyUp(int keyCode) { return singletonInstance->isKeyUpImplementation(keyCode); }

		inline static bool isMouseButtonDown(int button) { return singletonInstance->isMouseButtonDownImplementation(button); }
		inline static bool isMouseButtonUp(int button) { return singletonInstance->isMouseButtonUpImplementation(button); }
		inline static std::pair<double, double> getMousePosition() { return singletonInstance->getMousePositionImplementation(); }

		inline Input* getInstance() { return singletonInstance; }
		static void setWindow(void* newWindow) { window = newWindow; }
		static void setInstance(Input* newInstance) { singletonInstance = newInstance; }

		static void keyCallBack(int keyCode) { singletonInstance->keyCallBackImplementation(keyCode); }
		static void registerKeyCallBack(int keyCode, std::function<void()> newCallBack) { singletonInstance->registerKeyCallBackImplementation(keyCode, newCallBack); }

	protected:
		virtual bool isKeyDownImplementation(int keycode) = 0;
		virtual bool isKeyUpImplementation(int keycode) = 0;

		virtual bool isMouseButtonDownImplementation(int button) = 0;
		virtual bool isMouseButtonUpImplementation(int button) = 0;

		virtual std::pair<double, double> getMousePositionImplementation() = 0;
		virtual void keyCallBackImplementation(int keyCode) = 0;
		virtual void registerKeyCallBackImplementation(int keyCode, std::function<void()> newCallBack) = 0;

		static void* window;
	private:
		static Input* singletonInstance;
	};
}