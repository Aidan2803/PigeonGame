#define _CRT_SECURE_NO_WARNINGS

#include "Game.h"
#include "GameObject.h"
#include "TextureManager.h"

#include <SDL_ttf.h>

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <cstdlib>

#define NUMB_OF_TEXARR 4		//number of different enemy textures
#define NUMB_OF_BALLOONS 3		//number of different balloon textures
#define NUMB_OF_ARROWS 4				//number of arrows in settings
#define NUMB_OF_MAPS 2					//number of possible maps
#define NUMB_OF_SKINS 2 				//number of possible skins
#define NUMB_OF_UBERENEMYLEVELS 6 				//number of uber enemy down skins
#define MAX_MENUTYPEID 3		//number of menu options
#define MAX_ROWS_SETTINGS 2		//number of rows in settings

using namespace std;

GameObject *background;

SDL_Renderer* Game::renderer = nullptr;

Game::Game(){}
Game::~Game(){}

/*Global vars*/

int d = 0, a = 0, SIZE_E = 0, SIZE_S = 0, SIZE_B = 0, PROJECTILE_AM_FOR_TEXT = 0;
int directionFromKey = 0;
int integScore = 0;
int menuTypeId = 0, settingsTypeLRM = 0, settingsTypeLRS = 0, settingsTypeUD = 0, up_down_raw = 0, left_right_map = 0, left_right_skin = 0;	//menu conntrol 
int apply_change[3] = {0 , 0, 0};

bool isMenu = false, isSettings = false, isNight = false, menuClosed = false, settingsOpen = false, chooseMade = false, isMove = true;
bool gameOver = false;

/*Global aarrays(with texture names)*/

string assetsTex[NUMB_OF_TEXARR] = { "assets/enemy_a.png", "assets/enemy2_a.png", "assets/enemy3_a.png", "assets/enemy4_a.png"};
string assetsDownTex[NUMB_OF_TEXARR] = { "assets/enemy_down_a.png", "assets/enemy2_down_a.png", "assets/enemy3_down_a.png","assets/enemy4_down_a.png" };
string ballonsTex[NUMB_OF_BALLOONS] = { "assets/balloon_r_a.png", "assets/balloon_b_a.png", "assets/balloon_g_a.png"};
string minMapArr[NUMB_OF_MAPS] = { "assets/back_min.png", "assets/back_nau_min.png" };
string mapArr[NUMB_OF_MAPS] = { "assets/back.png", "assets/back_nau.png" };
string mapArrNight[NUMB_OF_MAPS] = { "assets/back_night.png", "assets/back_nau_night.png" };
string skinsArr[NUMB_OF_SKINS] = { "assets/player_a.png", "assets/player2_a.png" };
string uberEnemyDownArr[NUMB_OF_UBERENEMYLEVELS] = {"assets/uber_enemy_attack5_a.png", "assets/uber_enemy_attack4_a.png", "assets/uber_enemy_attack3_a.png", "assets/uber_enemy_attack2_a.png", "assets/uber_enemy_attack1_a.png", "assets/uber_enemy_attack0_a.png" };

class Player : public GameObject {						/*    !!!!!!!!!!! MAKE IN SEPARATED FILE !!!!!    */	//îr not
private:
	int frameWidth, frameHeight;
	unsigned int current_time_p, last_time_p;
public:
	int healthPoints = 5;
	bool beenHitted = false;
	Player(const char* texturesheet, int x, int y) : GameObject(texturesheet, x, y) {}	
	Player(const char* texturesheet, int x, int y, int imageWidth, int imageHeight, int frameAm) {

		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		xpos = x;
		ypos = y;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
	}
	~Player() {}

	void moveDirection(int directionFromKey) {
		if (directionFromKey == 0) {
			if (isMenu == false) {
				xpos += 3;
			}			
			angle = 180;		
			flipType = SDL_FLIP_VERTICAL;		
		}
		else if (directionFromKey == 1) {
			if (isMenu == false) {
				xpos -= 3;
			}
			angle = 360;
			flipType = SDL_FLIP_NONE;
		}
	}

	void setFlipType(SDL_RendererFlip flipType) {
		this->flipType = flipType;
	}

	void Update(int h, int w) {		
		if (!gameOver) {
			current_time_p = SDL_GetTicks();
			if (current_time_p > last_time_p + 800 && current_time_p < last_time_p + 850) {
				srcRect.x += frameWidth;
				last_time_p = current_time_p;
				if (srcRect.x == frameWidth * 2) {
					srcRect.x = 0;
				}
			}
		}	

		srcRect.h = h;
		srcRect.w = w;		
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;
	}
};

class Enemy : public GameObject {	
private:
	int frameWidth, frameHeight, imgEdge;
	int current_time_e, last_time_e;
	int idTex, dir; // dir == 0 - move right, dir == 1 - move left
public:
	bool projectileHit;		//to check if we`ve changed the texture to enemy_down

	Enemy(){
		srand(time(NULL));
		xpos = 0;
		ypos = 575;
		projectileHit = false;
	}

	Enemy(const char* texturesheet, int x, int y) : GameObject(texturesheet, x, y) {
		xpos = x;
		ypos = y;
		projectileHit = false;
		current_time_e = 0;
		last_time_e = 0;
	}

	void setAnimation(const char* texturesheet, int imageWidth, int imageHeight, int frameAm, int newTexId) {

		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		idTex = newTexId;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
		imgEdge = imageWidth;
	}

	~Enemy() {}
	
	void Update(int h, int w, int speed) {
		current_time_e = clock();
		if (menuClosed == true) {
			last_time_e = current_time_e;
		}
		if (current_time_e > last_time_e + 200 && current_time_e < last_time_e + 1050) {
			srcRect.x += srcRect.w;
			last_time_e = current_time_e;
			if (srcRect.x >= imgEdge) {
				srcRect.x = 0;
			}
		}

		srcRect.h = h;
		srcRect.w = w;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;
	
		if (dir == 0) {
			xpos += speed;
		}
		else if (dir == 1) {			
			xpos -= speed;
		}				
	}

	void checkScreenEdgesRespawn() {		
		if (xpos > 800)  {
			if (dir == 0) {
				xpos = (rand() % 200) * (-1);
			}	
			if (projectileHit == true) {
				changeTex(assetsTex[idTex].c_str());
				projectileHit = false;
			}
		}
		else if (xpos < -16) {
			if (dir == 1) {
				xpos = 850 + rand() % 1000;
			}	
			if (projectileHit == true) {
				changeTex(assetsTex[idTex].c_str());
				projectileHit = false;
			}
		}
		
	}

	void defineDirrection() {
		for (int i = 0; i < SIZE_E; i++) {
			if (xpos < 320) {	//move right
				dir = 0;
				flipType = SDL_FLIP_NONE;
			}
			else if (xpos >= 320) { //move left
				dir = 1;				
				flipType = SDL_FLIP_HORIZONTAL;
			}
		}
	}

	int getIdTex() {
		return idTex;
	}
};

class UberEnemy : public GameObject{
private:
	int frameWidth, frameHeight, imgEdge;
	int current_time_ue, last_time_ue;
	
	bool dir; // dir == 0 - move right, dir == 1 - move left
public:
	bool uberProjectileHit;
	int uberHP;

	UberEnemy(const char* texturesheet, int x, int y, int imageWidth, int imageHeight, int frameAm) {
		xpos = x;
		ypos = y;
		uberProjectileHit = false;
		current_time_ue = 0;
		last_time_ue = 0;
		uberHP = 5;

		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
		imgEdge = imageWidth;
	}

	void Update(int h, int w) {
		current_time_ue = clock();
		if (menuClosed == true) {
			last_time_ue = current_time_ue;
		}
		if (current_time_ue > last_time_ue + 200 && current_time_ue < last_time_ue + 1050) {
			srcRect.x += srcRect.w;
			last_time_ue = current_time_ue;
			if (srcRect.x >= imgEdge) {
				srcRect.x = 0;
			}
		}

		srcRect.h = h;
		srcRect.w = w;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;

	}

	void changeDirection() {
		if (xpos < 60) {
			dir = false;
			flipType = SDL_FLIP_NONE;
		}
		else if (xpos > 740) {
			dir = true;
			flipType = SDL_FLIP_HORIZONTAL;
		}		
	}

	void move(int speed) {
		if (isMove) {
			if (!dir) {
				xpos += speed;
			}
			else if (dir) {
				xpos -= speed;
			}
			if (uberHP <= 0) {
				xpos += speed + 5;
				if (xpos >= 10000) {
					uberProjectileHit = false;
					changeTex("assets/uber_enemy_a.png");
					uberHP = 5;					
					xpos = -100;
				}
			}
		}		
	}
	
};

UberEnemy *uberEnemy;

class Projectile : public GameObject {
public:
	int projectile_xpos = 0;
	int projectile_ypos = 0;

	bool isDown = false;

	Projectile(const char* texturesheet, int x, int y) : GameObject(texturesheet, x, y) {
		projectile_xpos = x;
		projectile_ypos = y;
	}		

	~Projectile() {}

	void set_coord(int x, int y) {
		projectile_xpos = x;
		projectile_ypos = y;
	}

	void Update(int h, int w, int speed) {

		srcRect.h = h;
		srcRect.w = w;
		srcRect.x = 0;
		srcRect.y = 0;

		destRect.x = projectile_xpos;
		destRect.y = projectile_ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;

		projectile_ypos += speed;
	}	
};

Player *player;

class Text {
private:
	SDL_Color color;
	SDL_Color overColor;
	SDL_Surface* surfaceMessageScore;
	SDL_Texture* MessageScore;	
	SDL_Rect msgRectScore;

	SDL_Surface* surfaceMessageProjectileAm;
	SDL_Texture* MessageProjectileAm;
	SDL_Rect msgRectProjectileAm;

	SDL_Surface* surfaceMessageHP;
	SDL_Texture* MessageHP;
	SDL_Rect msgRectHP;

	SDL_Texture* projectileImgTexture;
	SDL_Texture* heartImgTexture;
	SDL_Rect srcRectProjectileImg, destRectProjectileImg;
	SDL_Rect srcRectHertImg, destRectHertImg;

	SDL_Surface* surfaceMessageGameOver;
	SDL_Texture* MessageGameOver;
	SDL_Rect msgRectGameOver;

	TTF_Font* Consolas;	

	stringstream score;
	stringstream projectileAmountChar;	
	stringstream HP;	
	stringstream gameOver_str;

public:	
	Text(){
		Consolas = TTF_OpenFont("assets/Consola.ttf", 14);
		if (!Consolas) {
			cout << "Can`t open font" << endl;
		}
		else {
			cout << "Font`ve been opened" << endl;
		}
		if (isNight == false) {
			color = { 0, 0, 0 };
			projectileImgTexture = TextureManager::LoadTexture("assets/projectile.png");
		}
		else {
			color = { 255, 255, 255 };
			projectileImgTexture = TextureManager::LoadTexture("assets/projectile2.png");
		}
		overColor = { 225, 220, 78 };
		heartImgTexture = TextureManager::LoadTexture("assets/heart.png");
		
		score << "Score: " << integScore;
		surfaceMessageScore = TTF_RenderText_Solid(Consolas, score.str().c_str(), color);	//score 
		MessageScore = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageScore);

		projectileAmountChar << "" << PROJECTILE_AM_FOR_TEXT;
		surfaceMessageProjectileAm = TTF_RenderText_Solid(Consolas, projectileAmountChar.str().c_str(), color);	//projectile amount 
		MessageProjectileAm = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageProjectileAm);

		HP << "" << player->healthPoints;
		surfaceMessageHP = TTF_RenderText_Solid(Consolas, HP.str().c_str(), color);	//projectile amount 
		MessageHP = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageHP);

		gameOver_str << "Game Over";
		surfaceMessageGameOver = TTF_RenderText_Solid(Consolas, gameOver_str.str().c_str(), overColor);	//score 
		MessageGameOver = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageGameOver);
	}

	void UpdateScore() {

		SDL_FreeSurface(surfaceMessageScore);
		SDL_DestroyTexture(MessageScore);		
		
		score.str("");
		score << "Score: " << integScore;						//score update
		surfaceMessageScore = TTF_RenderText_Solid(Consolas, score.str().c_str(), color);
		MessageScore = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageScore);		
	}

	void UpdateProjectileAmount() {
		SDL_FreeSurface(surfaceMessageProjectileAm);
		SDL_DestroyTexture(MessageProjectileAm);

		srcRectProjectileImg.h = 12;
		srcRectProjectileImg.w = 12;
		srcRectProjectileImg.y = 0;
		srcRectProjectileImg.x = 0;

		destRectProjectileImg.x = 730;
		destRectProjectileImg.y = 0;
		destRectProjectileImg.h = srcRectProjectileImg.h * 2;
		destRectProjectileImg.w = srcRectProjectileImg.w * 2;

		projectileAmountChar.str("");
		projectileAmountChar << "" << PROJECTILE_AM_FOR_TEXT;						//projectile amount update
		surfaceMessageProjectileAm = TTF_RenderText_Solid(Consolas, projectileAmountChar.str().c_str(), color);
		MessageProjectileAm = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageProjectileAm);
	}

	void UpdateHP() {
		SDL_FreeSurface(surfaceMessageHP);
		SDL_DestroyTexture(MessageHP);

		srcRectHertImg.h = 14;
		srcRectHertImg.w = 12;
		srcRectHertImg.y = 0;
		srcRectHertImg.x = 0;

		destRectHertImg.x = 730;
		destRectHertImg.y = 40;
		destRectHertImg.h = srcRectHertImg.h * 2;
		destRectHertImg.w = srcRectHertImg.w * 2;

		HP.str("");
		HP << "" << player->healthPoints;						//score update
		surfaceMessageHP = TTF_RenderText_Solid(Consolas, HP.str().c_str(), color);
		MessageHP = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageHP);
	}

	void Update() {
		msgRectScore.x = 5;
		msgRectScore.y = 0;
		msgRectScore.w = 150;
		msgRectScore.h = 50;

		msgRectProjectileAm.x = 770;
		msgRectProjectileAm.y = 0;
		msgRectProjectileAm.w = 30;
		msgRectProjectileAm.h = 30;

		msgRectHP.x = 770;
		msgRectHP.y = 40;
		msgRectHP.w = 30;
		msgRectHP.h = 30;

		if (gameOver) {
			msgRectGameOver.x = 300;
			msgRectGameOver.y = 230;
			msgRectGameOver.w = 200;
			msgRectGameOver.h = 100;
		}		
	}

	void Renderer() {
		SDL_RenderCopy(Game:: renderer, MessageScore, NULL, &msgRectScore);
		SDL_RenderCopy(Game::renderer, MessageProjectileAm, NULL, &msgRectProjectileAm);
		SDL_RenderCopy(Game::renderer, projectileImgTexture, &srcRectProjectileImg, &destRectProjectileImg);
		SDL_RenderCopy(Game::renderer, heartImgTexture, &srcRectHertImg, &destRectHertImg);
		SDL_RenderCopy(Game::renderer, MessageHP, NULL, &msgRectHP);
		if (gameOver) { SDL_RenderCopy(Game::renderer, MessageGameOver, NULL, &msgRectGameOver); }		
	}
};

class MenuText {
private:

	SDL_Color color[2];

	SDL_Surface* surfaceMessagePlay;
	SDL_Texture* MessagePlay;
	SDL_Rect msgRectPlay;

	SDL_Surface* surfaceMessageSettings;
	SDL_Texture* MessageSettings;
	SDL_Rect msgRectSettings;

	SDL_Surface* surfaceMessageExit;
	SDL_Texture* MessageExit;
	SDL_Rect msgRectExit;

	TTF_Font* Consolas;

	stringstream Play;
	stringstream Settings;
	stringstream Exit;	

public:
	MenuText() {
		Consolas = TTF_OpenFont("assets/Consola.ttf", 24);
		if (!Consolas) {
			cout << "Can`t open font" << endl;
		}
		else {
			cout << "Font`ve been opened" << endl;
		}
		if (isNight == false) {
			color[0] = { 0, 0, 0 };
			color[1] = { 255, 180, 0 };
			//colorHover = { 0, 246, 255 };			
		}
		else {
			color[0] = { 255, 255, 255 };		
			color[1] = { 225, 220, 78 };
		}
		Play << "Play";
		Settings << "Settings";
		Exit << "Exit";

		surfaceMessagePlay = TTF_RenderText_Solid(Consolas, Play.str().c_str(), color[0]);
		MessagePlay = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessagePlay);

		surfaceMessageSettings = TTF_RenderText_Solid(Consolas, Settings.str().c_str(), color[0]);
		MessageSettings = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageSettings);

		surfaceMessageExit = TTF_RenderText_Solid(Consolas, Exit.str().c_str(), color[0]);
		MessageExit = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageExit);
	}
	~MenuText() { cout << "destruct" << endl; }

	void switchUpdate() {
		int id[3];
		for (int i = 0; i < MAX_MENUTYPEID; i++) {
			id[i] = 0;
		}
		for (int i = 0; i < MAX_MENUTYPEID; i++) {
			id[menuTypeId] = 1;
		}
		surfaceMessagePlay = TTF_RenderText_Solid(Consolas, Play.str().c_str(), color[id[0]]);
		surfaceMessageSettings = TTF_RenderText_Solid(Consolas, Settings.str().c_str(), color[id[1]]);
		surfaceMessageExit = TTF_RenderText_Solid(Consolas, Exit.str().c_str(), color[id[2]]);
		
		MessagePlay = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessagePlay);
		MessageSettings = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageSettings);
		MessageExit = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageExit);
	}

	void Update() {
		msgRectPlay.x = 320;
		msgRectPlay.y = 200;
		msgRectPlay.w = 150;
		msgRectPlay.h = 50;

		msgRectSettings.x = 300;
		msgRectSettings.y = 330;
		msgRectSettings.w = 200;
		msgRectSettings.h = 50;

		msgRectExit.x = 320;
		msgRectExit.y = 450;
		msgRectExit.w = 150;
		msgRectExit.h = 50;
	}
	
	void Renderer() {
		if (isMenu == true) {
			SDL_RenderCopy(Game::renderer, MessagePlay, NULL, &msgRectPlay);
			SDL_RenderCopy(Game::renderer, MessageSettings, NULL, &msgRectSettings);
			SDL_RenderCopy(Game::renderer, MessageExit, NULL, &msgRectExit);
		}
	}		
};

class SettingsText {
private:

	SDL_Color color[2];
	TTF_Font* Consolas;

	SDL_Surface* surfaceMessageMaps;
	SDL_Texture* MessageMaps;
	SDL_Rect msgRectMaps;

	SDL_Surface* surfaceMessageSkins;
	SDL_Texture* MessageSkins;
	SDL_Rect msgRectSkins;

	SDL_Surface* surfaceMessageAccept;
	SDL_Texture* MessageAccept;
	SDL_Rect msgRectAccept;

	SDL_Surface* surfaceMessageESC;
	SDL_Texture* MessageESC;
	SDL_Rect msgRectESC;

	stringstream Maps;
	stringstream Skins;
	stringstream Accept;
	stringstream ESC;

public:
	SettingsText() {
		Consolas = TTF_OpenFont("assets/Consola.ttf", 24);

		if (isNight == false) {
			color[0] = { 0, 0, 0 };
			color[1] = { 255, 180, 0 };
		}
		else {
			color[0] = { 255, 255, 255 };
			color[1] = { 225, 220, 78 };
		}
		Maps << "Maps";
		Skins << "Skins";
		Accept << "Change has been confirmed !";
		ESC << "Press ESC to open main menu.";

		surfaceMessageMaps = TTF_RenderText_Solid(Consolas, Maps.str().c_str(), color[0]);
		MessageMaps = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageMaps);

		surfaceMessageSkins = TTF_RenderText_Solid(Consolas, Skins.str().c_str(), color[0]);
		MessageSkins = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageSkins);

		surfaceMessageESC = TTF_RenderText_Solid(Consolas, ESC.str().c_str(), color[1]);
		MessageESC = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageESC);
	}
	~SettingsText() { cout << "destruct" << endl; }

	void switchUpdate() {
		if (up_down_raw == 0) {
			surfaceMessageMaps = TTF_RenderText_Solid(Consolas, Maps.str().c_str(), color[1]);	
			surfaceMessageSkins = TTF_RenderText_Solid(Consolas, Skins.str().c_str(), color[0]);			
		}
		else if (up_down_raw == 1) {
			surfaceMessageMaps = TTF_RenderText_Solid(Consolas, Maps.str().c_str(), color[0]);
			surfaceMessageSkins = TTF_RenderText_Solid(Consolas, Skins.str().c_str(), color[1]);
		}
		MessageMaps = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageMaps);
		MessageSkins = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageSkins);
	}

	void acceptUpdate() {		
		surfaceMessageAccept = TTF_RenderText_Solid(Consolas, Accept.str().c_str(), color[1]);
		MessageAccept = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessageAccept);
	}

	void Update() {
		msgRectMaps.x = 350;
		msgRectMaps.y = 85;
		msgRectMaps.w = 150;
		msgRectMaps.h = 50;

		msgRectSkins.x = 350;
		msgRectSkins.y = 350;
		msgRectSkins.w = 150;
		msgRectSkins.h = 50;

		msgRectAccept.x = 210;
		msgRectAccept.y = 40;
		msgRectAccept.w = 450;
		msgRectAccept.h = 50;

		msgRectESC.x = 230;
		msgRectESC.y = 550;
		msgRectESC.w = 450;
		msgRectESC.h = 50;
	}

	void Renderer() {
		SDL_RenderCopy(Game::renderer, MessageMaps, NULL, &msgRectMaps);
		SDL_RenderCopy(Game::renderer, MessageSkins, NULL, &msgRectSkins);
		SDL_RenderCopy(Game::renderer, MessageAccept, NULL, &msgRectAccept);
		SDL_RenderCopy(Game::renderer, MessageESC, NULL, &msgRectESC);
	}
};

class Food : public GameObject{
private:
	int xpos;
	int ypos;

	int frameWidth, frameHeight, imgEdge;
public:
	unsigned int current_time_f, last_time_f, last_time_f_a;

	Food() {
		xpos = 0;
		ypos = 0;
	}
	Food(const char* texturesheet, int x, int y, int imageWidth, int frameAm, int imageHeight) {
		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		xpos = x;
		ypos = y;
		current_time_f = last_time_f = 0;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
		imgEdge = imageWidth;
	}

	void Update(int h, int w) {
		int randi = 0;
		/*spawning food*/
		randi = 4000 + rand() % 6000;
		current_time_f = clock();
		if (menuClosed == true) {
			last_time_f = current_time_f;
		}		
		if (current_time_f > last_time_f + randi && current_time_f < last_time_f + randi + 550) {
			xpos = rand() % 785;
			ypos = player->ypos + 15;
			cout << "in if " << xpos << endl;			
			last_time_f = current_time_f;
		}

		current_time_f = clock();
		if (menuClosed == true) {
			last_time_f_a = current_time_f;
		}
		if (current_time_f > last_time_f_a + 200 && current_time_f < last_time_f_a + 1050) {
			srcRect.x += 15;
			last_time_f_a = current_time_f;
			if (srcRect.x >= imgEdge) {
				srcRect.x = 0;
			}
		}

		srcRect.h = h;
		srcRect.w = w;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;		
		}	

	void checkForPlayerAddProjectile();
};

class MenuObj : public GameObject {
protected:
	int frameWidth, frameHeight, imgEdge;
	unsigned int current_time_o, last_time_o;
public:
	MenuObj() {}
	MenuObj(const char* texturesheet, int x, int y) : GameObject(texturesheet, x, y) {}
	MenuObj(const char* texturesheet, int x, int y, int imageWidth, int imageHeight, int frameAm) {

		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		xpos = x;
		ypos = y;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
	}

	void Update(int h, int w) {

		srcRect.h = h;
		srcRect.w = w;
		srcRect.x = 0;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;
	}

	void Update(int h, int w, int speed) {

		srcRect.h = h;
		srcRect.w = w;
		srcRect.x = 0;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;

		xpos -= speed;
	}

	void checkForEdgesCloudsMenu() {
		if (xpos <= -200) {
			xpos = 900;
		}
	}

	void setFlipType() {
		flipType = SDL_FLIP_HORIZONTAL;
	}
};

class Balloon : public MenuObj {	//remake it like cloud, delete balloon class
public:	
	Balloon(const char* texturesheet, int x, int y, int imageWidth, int imageHeight, int frameAm) {

		objTexture = TextureManager::LoadTexture(texturesheet);
		std::cout << "objText = " << objTexture << std::endl;

		xpos = x;
		ypos = y;

		frameWidth = imageWidth / frameAm;
		frameHeight = imageHeight;
		imgEdge = imageWidth;
	}

	void Update(int h, int w, int speed) {

		current_time_o = SDL_GetTicks();
		if (current_time_o > last_time_o + 300 && current_time_o < last_time_o + 350) {
			srcRect.x += frameWidth;
			last_time_o = current_time_o;
			if (srcRect.x >= imgEdge) {
				srcRect.x = 0;
			}
		}

		srcRect.h = h;
		srcRect.w = w;
		srcRect.y = 0;

		destRect.x = xpos;
		destRect.y = ypos;
		destRect.h = srcRect.h * 2;
		destRect.w = srcRect.w * 2;

		ypos -= speed;
	}

	void checkForEdgesBaloonsMenu() {
		if (ypos <= -164) {	
			ypos = 882 + rand() % 300;
			xpos = rand() % 710;
		}
	}
};

vector<Balloon> balloonVect;

void fillbaloonVect() {
	srand(time(NULL));

	int amOfBallons = 3 + rand() % 10;
	int tempId = 0;

	for (int i = 0; i < amOfBallons; i++) {
		tempId = rand() % NUMB_OF_BALLOONS;
		Balloon newBalloon(ballonsTex[tempId].c_str(), rand() % 770, 800 + rand() % 200, 90, 82, 3);
		balloonVect.push_back(newBalloon);
	}

	SIZE_B = balloonVect.size();
}

Projectile *projectile;
GameObject *stone;

Text *text;

Food *food;

MenuObj *cloud1;
MenuObj *cloud2;

MenuText *menuText;

SettingsText *settingsText;

vector<Enemy> enemyVect;
vector<Projectile> projectileVect;
vector<Projectile>::iterator iterSh = projectileVect.begin();

vector<MenuObj> mapsVect;
vector<MenuObj> skinsVect;
vector<MenuObj> arrowVect;

int destX = 0, destY = 0;

void throwStone() {		
	if (stone->xpos > 640 && stone->ypos < 0) {
		cout << "CRAWLING" << endl;
		destX = player->xpos;
		destY = player->ypos;

		stone->xpos = uberEnemy->xpos;
		stone->ypos = uberEnemy->ypos;	
	}	
	else if (stone->xpos < 0 && stone->ypos < 0) {
		cout << "IN MY SKIIIN" << endl;
		destX = player->xpos;
		destY = player->ypos;

		stone->xpos = uberEnemy->xpos;
		stone->ypos = uberEnemy->ypos;	
	}
	cout << "destX = " << destX << "destY = " << destY << endl;
	if (uberEnemy->xpos < destX) {
		stone->xpos += 10;
	}
	else if (uberEnemy->xpos > destX) {
		stone->xpos -= 10;
	}	
	else if (uberEnemy->xpos == destX) {
		stone->xpos += 0;
	}
	stone->ypos -= 12;	
}

void Food::checkForPlayerAddProjectile() {
	int addX = 0;
	if (directionFromKey == 0) {
		addX = 32;
	}
	if (player->xpos + addX >= xpos && player->xpos + addX <= xpos + 15) {
		cout << "worked" << endl;
		cout << "player x = " << player->xpos << endl;
		Projectile newProjectile("assets/projectile2.png", -100, 900);
		projectileVect.push_back(newProjectile);
		SIZE_S = projectileVect.size();
		PROJECTILE_AM_FOR_TEXT++;
		text->UpdateProjectileAmount();
		xpos = -50;
	}
}

void fillEnemyVector() {
	srand(time(NULL));

	int amOfEnemy = 8 + rand() % 15;

	for (int i = 0; i < amOfEnemy; i++) {
		Enemy newEnemy;
		enemyVect.push_back(newEnemy);
	}
}

void coordCreateGiveTex() {	
	int tempId = 0;

	SIZE_E = enemyVect.size();

	for (int i = 0; i < SIZE_E; i++) {
		enemyVect[i].xpos = -100 + rand() % 750;		//make system of reset coords for enemies to prevent overlay
		enemyVect[i].defineDirrection();
		tempId = rand() % NUMB_OF_TEXARR;
		cout << tempId << endl;
		enemyVect[i].setAnimation(assetsTex[tempId].c_str(), 40, 31, 3, tempId);
	}
}

void checkForEdgesPlayer() {
	if (player->xpos >= 832) {
		player->xpos = -32;
	}
	else if (player->xpos <= -32) {
		player->xpos = 832;
	}
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {	
	srand(time(NULL));

	int flags = 0;

	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0 ) {
		std::cout << "Subsystems initialised\n";

		TTF_Init();

		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window) {
			std::cout << "window created\n";
		}
		renderer = SDL_CreateRenderer(window, -1, 0);		

		SYSTEMTIME st;
		GetSystemTime(&st);	
		if (st.wHour == 21) {
			st.wHour = 0;
		}
		else if (st.wHour == 22) {
			st.wHour = 1;
		}
		else if (st.wHour == 23) {
			st.wHour = 2;
		}
		else {		
			st.wHour += 3;		
		}

		if (st.wHour >= 0 && st.wHour <= 5 ) {
			isNight = true;
		}	
		else if (st.wHour >= 20 && st.wHour <= 23) {
			isNight = true;
		}
		else{
			isNight = false;
		}

		if (renderer){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		}

		isRunning = true;
		isMenu = true;
	}
	else {
		isRunning = false;
	}
	if(isMenu == true) {	
		player = new Player("assets/player_a.png", 360, 250, 64, 32, 2);		//creating player object
		if (isNight == true) {
			background = new GameObject("assets/menu_back_night.png", 0, 0);
			cloud1 = new MenuObj("assets/cloud_night.png", 650, 30);
			cloud2 = new MenuObj("assets/cloud_night.png", 230, 420);
		}
		else if(isNight == false){
			background = new GameObject("assets/menu_back.png", 0, 0);
			cloud1 = new MenuObj("assets/cloud.png", 650, 30);
			cloud2 = new MenuObj("assets/cloud.png", 230, 420);
			fillbaloonVect();
		}
		menuText = new MenuText();
	}		
}

void settingsFileApply() {
	int i = 0;
	char buff[2] = {' ', ' ' };
	int buff_i[2] = { 0, 0 };
	string filename = "settings.txt";

	ofstream file_write;
	ifstream file_read;
	
	file_read.open(filename);

	if (file_read.is_open()) {
		while (file_read.get(buff[i])) {
			i++;
		}
		buff_i[0] = atoi(buff) / 10;
		buff_i[1] = atoi(buff) % 10;
	}
	file_read.close();
	file_write.open(filename);
	if (apply_change[2] == 1) {					//if user opened settings
		if (file_write.is_open()) {		
			if (apply_change[0] == 1 && apply_change[1] == 1) {		//if user changed maps and skins value				
				file_write << settingsTypeLRM;
				file_write << settingsTypeLRS;
			}
			else if (apply_change[0] == 1) {					//if user changed maps value
				file_write << settingsTypeLRM;
				file_write << char(buff_i[1]);							//in file buff[1] is skins value
			}
			else if (apply_change[1] == 1) {						//if user changed skins value
				file_write << char(buff_i[0]);					//in file buff[0] is maps value
				file_write << settingsTypeLRS;
			}
		}
	}
	file_write.close();
	if(apply_change[2] == 0){	//if user didnt open settings			
		settingsTypeLRM = buff_i[0];
		settingsTypeLRS = buff_i[1];					
	}	
}			//bug

void additionalInitAfterMenu() {
	text = new Text();		//game text(score etc)
	
	if (isNight == false) {
		background = new GameObject(mapArr[settingsTypeLRM].c_str(), 0, 0);
	}
	else if (isNight == true) {		
		background = new GameObject(mapArrNight[settingsTypeLRM].c_str(), 0, 0);
		cout << "HRER" << endl;
	}
	player->changeTex(skinsArr[settingsTypeLRS].c_str());

	player->xpos = 300;		//was 100 !!!!!!!
	player->ypos = 100;
	Projectile newProjectile("assets/projectile.png", -100, 900);						//creating first projectile object	
	projectileVect.push_back(newProjectile);									//putting first projectile object in projectile vector
	stone = new GameObject("assets/stone.png", -10, -10);
	SIZE_S = projectileVect.size();
	PROJECTILE_AM_FOR_TEXT = SIZE_S;
	text->UpdateProjectileAmount();

	//creating score text object
	food = new Food("assets/seed_a.png", -50, player->ypos, 60, 4, 20);			//creating food object
	uberEnemy = new UberEnemy("assets/uber_enemy_a.png", 200, 575, 48, 31, 3);
	fillEnemyVector();
	coordCreateGiveTex();
}

void settingsInit() {
	int maps_am = NUMB_OF_MAPS, skins_am = NUMB_OF_SKINS, xpos = 0, ypos = 0;	
	
	//string *mapArr = new string[maps_am]; use when there will be auto system for setting textures
	//programer set amount of background and skins in variables, programm create as many objects in vector as many
	//background and skins there are. in future make it automatically set amount.	

	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			ypos = 200;
		}
		else if (i >= 2) {
			ypos = 450;
		}
		if (i % 2 == 0) {
			xpos = 250;
		}
		else {
			xpos += 300;
		}
		MenuObj newArrow("assets/arrow.png", xpos, ypos);
		arrowVect.push_back(newArrow);
		for (int i = 0; i < NUMB_OF_ARROWS; i++) {
			if (i % 2 == 0) {
				arrowVect[i].setFlipType();
			}
		}
	}		
	
	for (int i = 0; i < maps_am; i++) {			//window for maps choose
		MenuObj newMap(minMapArr[i].c_str(), 320, 140);
		mapsVect.push_back(newMap);
	}
	for (int i = 0; i < skins_am; i++) {		//window for skins choose
		MenuObj newSkin(skinsArr[i].c_str(), 390, 435);
		skinsVect.push_back(newSkin);
	}
}

void Game::handleEvents() {
	int projectile_shift_x = 0;
	int projectile_shift_y = 0;

	SDL_Event event;
	SDL_PollEvent(&event);	
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (isMenu == false) {
			switch (event.key.keysym.sym) {
			case SDLK_d:
				directionFromKey = 0;
				break;
			case SDLK_a:
				directionFromKey = 1;
				break;
			case SDLK_SPACE:
				if (directionFromKey == 0) {
					projectile_shift_x = 25;
					projectile_shift_y = 35;
				}
				else if (directionFromKey == 1) {
					projectile_shift_x = 45;
					projectile_shift_y = 35;
				}
				if (SIZE_S != 0) {
					if (projectileVect[0].isDown == false) {
						projectileVect[0].set_coord(player->xpos + projectile_shift_x, player->ypos + projectile_shift_y);
						PROJECTILE_AM_FOR_TEXT -= 1;
						text->UpdateProjectileAmount();
						projectileVect[0].isDown = true;
					}
				}
				else {
					PROJECTILE_AM_FOR_TEXT = 0;
					text->UpdateProjectileAmount();
				}
				break;
			default:
				break;
			}
		}
			else if (isMenu == true) {
				switch (event.key.keysym.sym) {
					case SDLK_UP:
						if (!isSettings) {
							menuTypeId--;
							if (menuTypeId < 0) {
								menuTypeId = MAX_MENUTYPEID - 1;
							}
						}
						else {
							if (up_down_raw == 0) {
								up_down_raw = MAX_ROWS_SETTINGS - 1;
							}
							else {
								up_down_raw--;
							}
						}
						break;
					case SDLK_DOWN:
						if (!isSettings) {
							menuTypeId++;
							if (menuTypeId > MAX_MENUTYPEID - 1) {
								menuTypeId = 0;
							}
						}
						else {	
							up_down_raw++;
							if (up_down_raw > MAX_ROWS_SETTINGS - 1) {
								up_down_raw = 0;
							}
						}
						break;
					case SDLK_RIGHT:
						if (isSettings) {
							if (up_down_raw == 0) {
								left_right_map++;
								if (left_right_map > NUMB_OF_MAPS - 1) {
									left_right_map = 0;
								}
								arrowVect[1].changeTex("assets/arrow_ac.png");
							}
							else if (up_down_raw == 1) {
								left_right_skin++;
								if (left_right_skin > NUMB_OF_SKINS - 1) {
									left_right_skin = 0;
								}
								arrowVect[3].changeTex("assets/arrow_ac.png");
							}							

						}
						break;
					case SDLK_LEFT:
						if (isSettings) {
							cout << "issettings" << endl;
							if (up_down_raw == 0) {
								left_right_map--;
								if (left_right_map < 0) {
									left_right_map = NUMB_OF_MAPS - 1;
								}
								arrowVect[0].changeTex("assets/arrow_ac.png");
							}
							else if (up_down_raw == 1) {
								left_right_skin--;
								if (left_right_skin < 0) {
									left_right_skin = NUMB_OF_SKINS - 1;
								}
								arrowVect[2].changeTex("assets/arrow_ac.png");
							}							
						}
						break;
					case SDLK_RETURN:
						if (!isSettings) {
							if (menuTypeId == 0) {
								isMenu = false;
								menuClosed = true;
								settingsFileApply();
								return;
							}
							else if (menuTypeId == 1) {
								isSettings = true;
								settingsOpen = true;
								apply_change[2] = 1;
								settingsInit();
							}
							else if (menuTypeId == 2) {
								settingsFileApply();
								isRunning = 0;
							}							
						}
						else {
							if (up_down_raw == 0) { 
								settingsTypeLRM = left_right_map;
								apply_change[up_down_raw] = 1;
							}
							else if (up_down_raw == 1) {
								settingsTypeLRS = left_right_skin;
								apply_change[up_down_raw] = 1;
							}
							chooseMade = true;
						}
						
						break;
					case SDLK_ESCAPE:
						if (isSettings) {
							isSettings = false;
						}
						break;
					default:
						break;
				}
			}
		}	
		else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
				if (isSettings) {
					if (up_down_raw == 0) {
						arrowVect[1].changeTex("assets/arrow.png");
					}
					else if (up_down_raw == 1) {
						arrowVect[3].changeTex("assets/arrow.png");
					}				
				}
				break;
			case SDLK_LEFT:
				if (isSettings) {
					if (up_down_raw == 0) {
						arrowVect[0].changeTex("assets/arrow.png");
					}
					else if (up_down_raw == 1) {
						arrowVect[2].changeTex("assets/arrow.png");
					}
				}				
				break;
			default:
				break;
			}
		}
}

bool uberEnemyHit = true;

void hitOutCatcher() {			
	int id = 0;
	if (SIZE_S != 0) {
		for (int k = 0; k < SIZE_S; k++) {
			if (projectileVect[k].projectile_xpos > 800 || projectileVect[k].projectile_ypos > 640) {
				if (projectileVect[k].isDown == true) {
					projectileVect.erase(projectileVect.begin());
					SIZE_S = projectileVect.size();
					projectileVect[k].isDown = false;
				}
			}
			for (int i = 0; i < SIZE_E; i++) {
				for (int j = 0; j < 40; j += 8) {
					int hittedEnemyI = 0;
					if (projectileVect[k].projectile_xpos <= enemyVect[i].xpos + 36 && projectileVect[k].projectile_xpos >= enemyVect[i].xpos) {		//remember in var i of enemy who was hitted
						if (projectileVect[k].projectile_ypos >= enemyVect[i].ypos && projectileVect[k].projectile_ypos < 600) {						//and change tex only for this hitted enemy
							hittedEnemyI = i;
							if (projectileVect[k].isDown == true) {								
								projectileVect.erase(projectileVect.begin());							
								SIZE_S = projectileVect.size();	
								projectileVect[k].isDown = false;
							}							
							if (enemyVect[hittedEnemyI].projectileHit == false) {
								id = enemyVect[hittedEnemyI].getIdTex();
								enemyVect[hittedEnemyI].changeTex(assetsDownTex[id].c_str());  //and change tex only for this hitted enemy
								integScore++;
								text->UpdateScore();
								enemyVect[hittedEnemyI].projectileHit = true;
							}
						}
					}
					else if (projectileVect[k].projectile_xpos <= uberEnemy->xpos + 36 && projectileVect[k].projectile_xpos >= uberEnemy->xpos) {
						if (projectileVect[k].projectile_ypos >= uberEnemy->ypos && projectileVect[k].projectile_ypos < 600) {		
							cout << "HITTED" << endl;
							if (projectileVect[k].isDown) {
								projectileVect.erase(projectileVect.begin());
								SIZE_S = projectileVect.size();
								projectileVect[k].isDown = false;
							}
							if (uberEnemyHit) {
								uberEnemyHit = false;
								integScore += 5;
								uberEnemy->changeTex(uberEnemyDownArr[uberEnemy->uberHP].c_str());
								if (uberEnemy->uberHP > 0) {
									uberEnemy->uberHP--;
								}
								text->UpdateScore();
								uberEnemy->uberProjectileHit = true;
							}												
						}
					}
				}
			}
			if (SIZE_S == 0) { 
				return; 
			}
		}
	}	
	uberEnemyHit = true;
}

void hitPlayer() {
	if (stone->ypos <= player->ypos + 36 && stone->ypos > player->ypos) {		
		if (stone->xpos >= player->xpos && stone->xpos <= player->xpos + 32) {
			if (!(player->beenHitted)) {
				player->healthPoints--;
				player->beenHitted = true;
			}								
		}
	}
	else {
		player->beenHitted = false;
	}
}

void ifDead() {
	if (player->healthPoints == 0) {
		gameOver = true;
		player->setFlipType(SDL_FLIP_HORIZONTAL);
		if (player->ypos >= 590) {
			player->ypos += 0;
		}
		else { 
			player->ypos += 6;
			player->xpos += 3;
		}		
	}
}

int curTimeUpdate = 0, lastTimeUpdate = 0;

void Game::update() {
	/*actions in game*/
	if (isMenu == false) {
		if (menuClosed == true) {
			delete menuText;
			additionalInitAfterMenu();	
		}
		if (!gameOver) {
			player->moveDirection(directionFromKey);
		}		
		hitOutCatcher();			
		checkForEdgesPlayer();
		text->Update();
		text->UpdateHP();
		for (int i = 0; i < SIZE_E; i++) {
			enemyVect[i].checkScreenEdgesRespawn();
		}
		background->UpdateBack(true);
		player->Update(36, 32);
		if (!gameOver) {
			uberEnemy->changeDirection();
		}	
		if (uberEnemy->uberProjectileHit && uberEnemy->uberHP > 0) {			
			throwStone();
		}
		hitPlayer();
		ifDead();
		stone->Update(4, 4, 0);
		uberEnemy->move(3);		//3 - speed
		uberEnemy->Update(31, 16);
		if (SIZE_S != 0) {
			for (int i = 0; i < SIZE_S; i++) {
				projectileVect[i].Update(4, 4, 8);
			}
		}
		food->checkForPlayerAddProjectile();
		if (!gameOver) {
			food->Update(20, 15);
		}	
		for (int i = 0; i < SIZE_E; i++) {
			enemyVect[i].Update(31, 16, 2);
		}
		menuClosed = false;
	}
	/* actions in menu */
	else if (isMenu == true) {		
		background->UpdateBack(true);
		player->moveDirection(0);
		player->Update(36, 32);
		cloud1->checkForEdgesCloudsMenu();
		cloud2->checkForEdgesCloudsMenu();
		cloud1->Update(56, 100, 5);
		cloud2->Update(56, 100, 5);
		if (isNight == false) {
			for (int i = 0; i < SIZE_B; i++) {
				balloonVect[i].checkForEdgesBaloonsMenu();
			}
			for (int i = 0; i < SIZE_B; i++) {
				balloonVect[i].Update(82, 30, 4);
			}
		}
		menuText->switchUpdate();
		/* actions in settings menu */
		if (isSettings) {
			settingsText = new SettingsText;
			if (settingsOpen) {
				settingsOpen = false;
				settingsInit();
				mapsVect[0].Update(80, 100);
				skinsVect[0].Update(32, 32);
			}
			settingsText->switchUpdate();
			if (chooseMade) {				
				settingsText->acceptUpdate();			
				curTimeUpdate = clock();
				if (curTimeUpdate - lastTimeUpdate > 5500) {
					lastTimeUpdate += curTimeUpdate - lastTimeUpdate + 1000;					
				}
				if (curTimeUpdate > lastTimeUpdate + 1500 ) {		//fix
					lastTimeUpdate = curTimeUpdate; 
					chooseMade = false;
				}				
			}
			settingsText->Update();
			for (int i = 0; i < NUMB_OF_ARROWS; i++) {
				arrowVect[i].Update(20, 20);
			}
			for (int i = 0; i < NUMB_OF_MAPS; i++) {			
				if (up_down_raw == 0) {
					mapsVect[left_right_map].Update(80, 100);
				}				
			}
			for (int i = 0; i < NUMB_OF_SKINS; i++) {
				if (up_down_raw == 1) {
					skinsVect[left_right_skin].Update(32, 32);
				}
			}
			
			/*settings init if just entered settings*/
			/*settings updates*/
		}
		menuText->Update();
	}
}

void Game::render() {
	SDL_RenderClear(renderer);
	/* actions in game */
	if (isMenu == false) {
		background->Render();
		text->Renderer();
		player->Render();
		for (int i = 0; i < SIZE_E; i++) {
			enemyVect[i].Render();
		}
		uberEnemy->Render();
		if (SIZE_S != 0) {
			for (int i = 0; i < SIZE_S; i++) {
				projectileVect[i].Render();
			}
		}
		food->Render();
		stone->Render();
	}	
	/* actions in menu */
	else if (isMenu == true) {
		background->Render();		
		cloud1->Render();
		cloud2->Render();
		if (isNight == false) {
			for (int i = 0; i < SIZE_B; i++) {
				balloonVect[i].Render();
			}
		}
		/* actions is settings */
		if (isSettings) {
			settingsText->Renderer();
			for (int i = 0; i < NUMB_OF_ARROWS; i++) {
				arrowVect[i].Render();
			}
			for (int i = 0; i < NUMB_OF_MAPS; i++) {
				mapsVect[left_right_map].Render();
			}
			for (int i = 0; i < NUMB_OF_SKINS; i++) {
				skinsVect[left_right_skin].Render();
			}
		}
		if (!isSettings) {
			player->Render();
			menuText->Renderer(); 
		}		
	}
	
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	TTF_Quit();
	SDL_Quit();
	std::cout << "game cleaned\n";
}