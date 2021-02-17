#pragma once


class IEventHandler
{
public:
	IEventHandler() = default;
	virtual ~IEventHandler() = default;

	virtual void onKeyDown(unsigned int keyCode, bool isRepeat) {}
	virtual void onKeyUp(unsigned int keyCode, bool isRepeat) {}
	
	virtual void onMouseDown(unsigned int mouseButton, unsigned short int numClicks) {}
	virtual void onMouseUp(unsigned int mouseButton, unsigned short int numClicks) {}
	
	virtual void onMouseMove(int mousePosX, int mousePosY, int deltaX, int deltaY) {}

private:
};

