#include "TextureManager.h"
#include <iostream>

 SDL_Texture* TextureManager::LoadTexture(const char* texture) {

	SDL_Surface* tempSurface = IMG_Load(texture);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
	if (tex == NULL) {
		std::cout << "NO TEX\n";
	}
	SDL_FreeSurface(tempSurface);

	return tex;
}
 