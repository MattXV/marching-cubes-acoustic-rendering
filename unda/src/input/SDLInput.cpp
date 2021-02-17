#include "SDLInput.h"

bool SDLInput::handleInput()
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type)
		{
		case SDL_KEYDOWN:
			onKeyDown(e.key.keysym.scancode, e.key.repeat);
			break;
		case SDL_KEYUP:
			onKeyUp(e.key.keysym.scancode, e.key.repeat);
			break;
		case SDL_MOUSEBUTTONDOWN:
			onMouseDown(e.button.button, e.button.clicks);
			break;
		case SDL_MOUSEBUTTONUP:
			onMouseUp(e.button.button, e.button.clicks);
			break;
		case SDL_MOUSEMOTION:
			onMouseMove(e.motion.x, e.motion.y, e.motion.x - lastMouseX, e.motion.y - lastMouseY);
			lastMouseX = e.motion.x;
			lastMouseY = e.motion.y;
			break;
		case SDL_QUIT:
			return false;
			break;
		default:
			return true;
		}

		if (e.type == SDL_QUIT) {
			return false;
		}
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return false;
		}
		
	}
	return true;
}

void SDLInput::onKeyDown(unsigned int keyCode, bool isRepeat)
{
}

void SDLInput::onKeyUp(unsigned int keyCode, bool isRepeat)
{
}

void SDLInput::onMouseDown(unsigned int mouseButton, unsigned short int numClicks)
{
}

void SDLInput::onMouseUp(unsigned int mouseButton, unsigned short int numClicks)
{
}

void SDLInput::onMouseMove(int mousePosX, int mousePosY, int deltaX, int deltaY)
{
}
