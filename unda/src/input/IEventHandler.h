#pragma once

#include <stdint.h>

class IEventHandler
{
public:
	IEventHandler() = default;
	virtual ~IEventHandler() = default;

	virtual void onKeyDown(uint32_t keyCode, bool isRepeat) {}
	virtual void onKeyUp(uint32_t keyCode, bool isRepeat) {}
	
	virtual void onMouseDown(uint32_t mouseButton, uint8_t numClicks) {}
	virtual void onMouseUp(uint32_t mouseButton, uint8_t numClicks) {}
	
	virtual void onMouseMove(int32_t mousePosX, int32_t mousePosY, int32_t deltaX, int32_t deltaY) {}

private:

};

