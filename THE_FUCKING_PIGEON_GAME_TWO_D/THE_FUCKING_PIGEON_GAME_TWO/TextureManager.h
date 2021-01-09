#include "Game.h"
#include "SDL_image.h"
#pragma once

class TextureManager
{
public:
	static SDL_Texture* LoadTexture(const char* fileName);
	static void Draw(SDL_Texture * tex, SDL_Rect source, SDL_Rect destination);
};

