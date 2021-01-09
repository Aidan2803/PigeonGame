#include "Game.h"
#pragma once
class GameObject{

public:
	GameObject();
	GameObject(const char* texturesheet, int x, int y);
	~GameObject();

	virtual void Update(int h, int w, int speed);
	void UpdateBack(bool back);	
	void Render();
	void changeTex(const char* texturesheet);

	int xpos, ypos;
	
protected:

	SDL_Texture* objTexture;
	SDL_Rect srcRect, destRect;
	SDL_RendererFlip flipType = SDL_FLIP_NONE;	

	int angle = 0;
};


