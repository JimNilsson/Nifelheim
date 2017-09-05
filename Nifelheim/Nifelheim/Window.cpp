#include "Window.h"
#include <exception>
#include <SDL_syswm.h>
Window::Window(uint32_t width, uint32_t height, bool fullscreen)
{
	_width = width;
	_height = height;
	//Set up window and rendering context
	_window = nullptr;
	_surface = nullptr;
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		throw std::exception("Could not initialize SDL Video");
	_window = SDL_CreateWindow("Nifelheim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (_window == nullptr)
		throw std::exception("Failed to create window");
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(_window, &info);
	_hwnd = info.info.win.window;
}

Window::~Window()
{
	
}

uint32_t Window::GetWidth()
{
	return _width;
}

uint32_t Window::GetHeight()
{
	return _height;
}

bool Window::GetWindowedMode()
{
	return !fullScreen;
}

HWND Window::GetHandle()
{
	return _hwnd;
}
