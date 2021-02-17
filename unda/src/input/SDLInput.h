#pragma once

#include <SDL.h>
#include "IEventHandler.h"


class SDLInput : public IEventHandler
{
public:
	bool handleInput();


	void onKeyDown(unsigned int keyCode, bool isRepeat);
	void onKeyUp(unsigned int keyCode, bool isRepeat);

	void onMouseDown(unsigned int mouseButton, unsigned short int numClicks);
	void onMouseUp(unsigned int mouseButton, unsigned short int numClicks);

	void onMouseMove(int mousePosX, int mousePosY, int deltaX, int deltaY);
private:
	int lastMouseX = 0, lastMouseY = 0;
	bool isRunning = false;
};