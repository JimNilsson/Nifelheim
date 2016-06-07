#ifndef _WINDOW_H_
#define _WINDOW_H_
#include <SDL.h>
#include <d3d11.h>
class Window
{
private:
	uint32_t _width;
	uint32_t _height;
	bool fullScreen;
	SDL_Window* _window;
	SDL_Surface* _surface;
	HWND _hwnd;

public:
	Window(uint32_t width = 800, uint32_t height = 600, bool fullscreen = false);
	~Window();

	uint32_t GetWidth();
	uint32_t GetHeight();
	bool GetWindowedMode();
	HWND GetHandle();
	

};


#endif
