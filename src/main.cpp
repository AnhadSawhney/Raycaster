//============================================================================
// Name        : main.cpp
// Author      : Anhad Sawhney
//============================================================================

#include "main.h"
#include "preferences.h"
#include "level.h"
#include "gamefunctions.h"
#include "networkingfunctions.h"

int main(int argc, char* argv[]) {
	setup();

	bool RunApp = true, fpscap = true, drawmap = true, drawrays = drawmap;
	double playerangle = 0;
	double yangle = 0;
	xyvector position = initpos, direction = initdirection, plane = initplane;
	//Initialization
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	//cout << fixed;
	//cout << setprecision(2);

	//Window
	SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
		  SDL_WINDOW_SHOWN
		| SDL_WINDOW_OPENGL
		| SDL_WINDOW_BORDERLESS
		| SDL_WINDOW_INPUT_FOCUS
		| SDL_WINDOW_MOUSE_FOCUS
		| SDL_WINDOW_FULLSCREEN_DESKTOP
		| SDL_WINDOW_ALWAYS_ON_TOP);

	SDL_GetWindowSize(window, &width, &height);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //Renderer
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	TTF_Font* font = TTF_OpenFont("resources/ARCADE_N.ttf", 15); //this opens a font style and sets a size

	SDL_Color FontColor = {255, 255, 0};

	int mousex = 0, mousey = 0;

	stringstream sstm;
	//********************************BEGIN LOADING***************************************************
	bool error = false;

	sstm.str(""); //*****************************************LOAD WALLS*******************************
	sstm << "resources/walls" << resolution << ".png";
	SDL_Texture* walltex = SDL_CreateTextureFromSurface(renderer, IMG_Load(sstm.str().c_str()));
	error |= !walltex;
	cout << "Loaded Image: " << sstm.str() << endl;

	SDL_Texture* CrossRtex = SDL_CreateTextureFromSurface(renderer, IMG_Load("resources/CrossHairR.png"));
	error |= !CrossRtex;
	cout << "Loaded Red Crosshair" << endl;

	SDL_Texture* CrossGtex = SDL_CreateTextureFromSurface(renderer, IMG_Load("resources/CrossHairG.png"));
	error |= !CrossGtex;
	cout << "Loaded Green Crosshair" << endl;

	/*SDL_Surface* CrossB = IMG_Load("Resources/CrossHairB.png");
	SDL_Texture* CrossBtex = SDL_CreateTextureFromSurface(renderer, CrossB);
	SDL_FreeSurface(CrossB);
	error |= !CrossB;
	cout << "Loaded Blue Crosshair" << endl;*/

	vector<SDL_Texture*> gunframes(13);

	for(int i = 0; i<13; i++){
		sstm.str("");
		sstm << "resources/frames/frame_" << i << ".png";
		gunframes[i] = SDL_CreateTextureFromSurface(renderer, IMG_Load(sstm.str().c_str()));
		error |= !gunframes[i];
		cout << "Loaded gun frame: " << i << endl;
	}

	vector<SDL_Texture*> barrelframes(4);

	for(int i = 0; i<4; i++){
		sstm.str("");
		sstm << "resources/frames/barrel_" << i << ".png";
		barrelframes[i] = SDL_CreateTextureFromSurface(renderer, IMG_Load(sstm.str().c_str()));
		error |= !barrelframes[i];
		cout << "Loaded barrel frame: " << i << endl;
	}

	SDL_Texture* playertex = SDL_CreateTextureFromSurface(renderer, IMG_Load("Resources/player.png"));
	error |= !playertex;
	cout << "Loaded Player" << endl;

	if (error) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FATAL ERROR!",
				IMG_GetError(), window);
		RunApp = false;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);

	double elapsedTime = SDL_GetTicks(), startTimer;
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	bool firing = false;

	//bool aimedatentity = false;
	//sstm << fixed;
	SDL_Rect src, dest, crosshairRect;

	crosshairRect.x = width / 2 - 14;
	crosshairRect.y = height / 2 - 14;
	crosshairRect.w = 28;
	crosshairRect.h = 28;

	//float *aimresults;

	//Event Handler
	SDL_Event event;

	//Lock Mouse
	SDL_SetRelativeMouseMode(SDL_TRUE);

	cout << "Done Loading" << endl;

	//int leftover = width - scanresolution;

	raycastData data;
	int xcount = 0;
	int fade;
	uint8_t tick = 0;
	uint8_t tick2 = 0;
	uint8_t tick3 = 0;

	sstm.str("");
	sstm.precision(2);
	sstm << fixed;

	vector<double> zbuffer(scanresolution);

	//While application is running
	while (RunApp) {
		//RunApp = false;
		startTimer = SDL_GetTicks();

		//cout << recvInfo();
		if(tick3 == 30){
			for(uint8_t i = 0; i < entities.size(); i++){
				if(entities[i].type == 0){
					if(rand() % 2 == 0){
						entities[i].x += 0.1;
					} else {
						entities[i].x -= 0.1;
					}

					if(rand() % 2 == 0){
						entities[i].y += 0.1;
					} else {
						entities[i].y -= 0.1;
					}

					entities[i].angle += rand() % 20 - 10;

					entities[i].angle = abs(entities[4].angle);
				}
			}


			tick3 = 0;
		}
		tick3++;

		if(playerangle < 0){
			playerangle += 360;
		} else if(playerangle >= 360){
			playerangle -= 360;
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		dest.x = 0; dest.w = width;
		dest.h = abs(yangle); dest.y = height+yangle;

		SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
		SDL_RenderFillRect(renderer, &dest);

		dest.h = 1;

		for(int i = 0; i < height/2; i++){
			SDL_SetRenderDrawColor(renderer, 200, 200, 200, int(255*i/(height/2)));
			dest.y = yangle + i+height/2;
			SDL_RenderDrawRect(renderer, &dest);
		}

		xcount = 0;
		for(int x = 0; x < scanresolution; x++) {
			//calculate ray position and direction
			//xyvector scaledplane = plane.scale(2 * x / double(scanresolution) - 1);
			data = raycastSingle(renderer, position, (xyvector){direction.x + plane.x*(2*x/double(scanresolution)-1), direction.y + plane.y*(2*x/double(scanresolution)-1)}, drawrays); //direction vector of the ray

			zbuffer[x] = data.distance;

			dest.w = width/scanresolution;
			//if(x % (scanresolution/(scanresolution-width)+1) == 0){
			//	dest.w++;
			//}
			if(data.distance <= renderdist) {
				dest.x = xcount;

				dest.h = height/data.distance;
				dest.y = int(height - dest.h)/2+yangle;

				src.w = 1;
				src.h = resolution;

				src.x = tileCoordsX[data.walltype] * resolution + data.wallsegment;
				src.y = tileCoordsY[data.walltype] * resolution;

				SDL_RenderCopy(renderer, walltex, &src, &dest);

				fade = int(255*data.distance/renderdist);

				if(!data.side){
					fade -= 50;
				}

				if(fade < 0){
					fade = 0;
				}
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade);
				SDL_RenderFillRect(renderer, &dest);
			}
			xcount += dest.w;
		}

		sortentities(position);

		for(uint8_t i = 0; i < entities.size(); i++) {//******************************************entity DRAWING*********************************8
			//translate entity position to relative to camera
			xyvector translatedentity = entities[i].getvector() - position;

			//transform entity with the inverse camera matrix
			// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
			// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
			// [ planeY   dirY ]                                          [ -planeY  planeX ]

			double invDet = 1.0 / (plane.x * direction.y - direction.x * plane.y); //required for correct matrix multiplication

			xyvector transform;
			transform.x = (direction.y * translatedentity.x - direction.x * translatedentity.y);
			transform.y = (-plane.y * translatedentity.x + plane.x * translatedentity.y); //this is actually the depth inside the screen, that what Z is in 3D

			transform = transform.scale(invDet);

			int entitiescreenX = int((width / 2) * (1 + transform.x / transform.y));

			dest.h = abs(int(height / (transform.y*1.5)));

			dest.y = int(height-(dest.h/1.5))/2 + yangle;

			/*dest.w = abs( int (height / (transform.y)));
			dest.x = -dest.w / 2 + entitiescreenX;
			src.x = 0; src.y = 0; src.w = resolution; src.h = resolution;
			SDL_RenderCopy(renderer, barreltex, &src, &dest);*/

			//calculate width of the entity
			int entityWidth = abs( int (height / (transform.y*1.5)));
			int drawStart = -entityWidth / 2 + entitiescreenX;
			//if(drawStart < 0) drawStart = 0;
			int drawEnd = entityWidth / 2 + entitiescreenX;
			//if(drawEnd >= width) drawEnd = width - 1;

			dest.w = 1; //entityWidth/resolution

			src.x = 0;
			src.y = 0;
			src.w = 1;
			src.h = resolution;

			double x = 0;
			uint8_t type = entities[i].type;
			int health = entities[i].health;

			int angle = (int)(entities[i].angle);
			for(int i = drawStart; i < drawEnd; i++){
				if(transform.y > 0 && i > 0 && i < width && transform.y < zbuffer[i]){
					dest.x = i;
					if(type == 0){ //entities[i].type == 0
						src.y = resolution * int((playerangle - angle)/45);
						SDL_RenderCopy(renderer, playertex, &src, &dest);
					} else {
						SDL_RenderCopy(renderer, barrelframes[int(health/100.0*3.0)], &src, &dest);
					}
				}
				x += (double)(resolution)/entityWidth;
				src.x = x;
			}
		}


		if(drawmap){   // ************************MINIMAP***********************************************
			minimap(renderer, position, direction, plane, level, 1, walltex); //elapsedTime/1000*movespeed
		}

		if(firing){
			SDL_QueryTexture(gunframes[tick2], NULL, NULL, &dest.w, &dest.h);
			dest.w += gunscale;
			dest.h += gunscale;
			dest.x = width - dest.w + 80;
			dest.y = height - dest.h + 165;
			SDL_RenderCopy(renderer, gunframes[tick2], NULL, &dest);
			tick++;
			if(tick == 2){
				tick = 0;
				tick2++;
				if(tick2 == 13){
					tick2 = 0;
					firing = false;
				}
			}
		} else {
			tick2 = 0;
			SDL_QueryTexture(gunframes[tick2], NULL, NULL, &dest.w, &dest.h);
			dest.w += gunscale;
			dest.h += gunscale;
			dest.x = width - dest.w + 80;
			dest.y = height - dest.h + 165;
			SDL_RenderCopy(renderer, gunframes[tick2], NULL, &dest);
		}

		sstm.str(""); // *******************************************DRAW INFO TEXT*****************************
		sstm << (int)1000/elapsedTime << " fps, x: " << position.x << ", y: " << position.y << ", X ang: " << playerangle << ", Y ang: " << yangle;

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, sstm.str().c_str(), FontColor);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		dest.x = 0; dest.y = 0;
		SDL_QueryTexture(Message, NULL, NULL, &dest.w, &dest.h);
		SDL_RenderCopy(renderer, Message, NULL, &dest);

		dest.x = 0; dest.y = height - 20;
		SDL_QueryTexture(Message, NULL, NULL, &dest.w, &dest.h);

		SDL_RenderCopy(renderer, CrossGtex, NULL, &crosshairRect);

		SDL_PumpEvents();
		SDL_GetRelativeMouseState(&mousex, &mousey);
		playerangle -= mousex*xsensitivity;
		yangle -= mousey*ysensitivity;
		direction = initdirection.rotate(playerangle*M_PI/180);
		plane = initplane.rotate(playerangle*M_PI/180);
		keystate = SDL_GetKeyboardState(NULL);

		xyvector hitbox = direction.scale(elapsedTime/1000*movespeed);

		/*if(drawmap){   // ************************MINIMAP***********************************************
			minimap(renderer, position, hitbox, level);
		}*/

		if((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) && (noclip || !calculateCollision(position+hitbox))){
			position += hitbox;
		}

		hitbox = hitbox.rotate90cw();

		if((keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) && (noclip || !calculateCollision(position+hitbox))){
			position += hitbox;
		}

		hitbox = hitbox.rotate90cw();

		if((keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) && (noclip || !calculateCollision(position+hitbox))){
			position += hitbox;
		}

		hitbox = hitbox.rotate90cw();

		if((keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) && (noclip || !calculateCollision(position+hitbox))){
			position += hitbox;
		}


		while (SDL_PollEvent(&event)) {
			switch(event.type){
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
						case SDLK_q:
							RunApp = false;
							break;
						case SDLK_e:
							for(uint8_t i = 0; i < entities.size(); i++){
								cout <<  entities[i].x << " " << entities[i].y << " " << entities[i].type << " " << entities[i].angle << endl;
							}
							break;
					}
					break;
				case SDL_QUIT:
					RunApp = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					firing = true;
					break;
			}
		}

		SDL_RenderPresent(renderer);

		elapsedTime = SDL_GetTicks() - startTimer;
		if (fpscap && elapsedTime < framecap) { // **********************FPS CAP**********************************
			SDL_Delay(framecap - elapsedTime);
			elapsedTime = framecap;
		}
	}
	//Clean up
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	return 0;
}
