#include "GameObject.h"
#include "TextureManager.h"

#include <iostream>
#include <SDL_ttf.h>


GameObject::GameObject() {
	std::cout << "gameObj constr" << std::endl;
	xpos = 0;
	ypos = 0;
}
GameObject::GameObject(const char* texturesheet, int x, int y){

	objTexture = TextureManager::LoadTexture(texturesheet);
	std::cout << "objText = " << objTexture << std::endl;

	xpos = x;
	ypos = y;
}
GameObject::~GameObject() {}

void GameObject::Update(int h, int w, int speed) {
	
	srcRect.h = h;
	srcRect.w = w;
	srcRect.x = 0;
	srcRect.y = 0;

	destRect.x = xpos;
	destRect.y = ypos;
	destRect.h = srcRect.h * 2;
	destRect.w = srcRect.w * 2;

	xpos += speed;
}

void GameObject::UpdateBack(bool back) {

	srcRect.h = 640;
	srcRect.w = 800;
	srcRect.x = 0;
	srcRect.y = 0;

	destRect.x = xpos;
	destRect.y = ypos;
	destRect.h = srcRect.h;
	destRect.w = srcRect.w;
}

void GameObject::changeTex(const char* texturesheet) {
	std::cout << "in change tex\n";
	objTexture = TextureManager::LoadTexture(texturesheet);
	std::cout << "after change tex\n";
	std::cout << "objText2 = " << objTexture;
}

void  GameObject::Render() {
	SDL_RenderCopyEx(Game::renderer, objTexture, &srcRect, &destRect, angle, NULL, flipType);
}