/*
 * rendering.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Anhad
 */

#ifndef GAMEFUNCTIONS_H_
#define GAMEFUNCTIONS_H_

uint32_t getpixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *) p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *) p;
		break;

	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}

vector<uint8_t> makeTextureArray(SDL_Surface *surface, int xinit, int y, int w,
		int h, bool mode) { //true = img, false = lvl
	vector<uint8_t> out;
	out.reserve(w * h * 3);
	uint8_t r, g, b;
	int x = xinit;
	while (y < h) {
		x = xinit;
		while (x < w) {
			SDL_GetRGB(getpixel(surface, x, y), surface->format, &r, &g, &b);
			//addr = (y*w+x)*3;
			if (mode) {
				out.push_back(r);
				out.push_back(g);
				out.push_back(b);
			} else {
				out.push_back(r / 4);
				out.push_back(g / 4);
				out.push_back(b);
			}
			x++;
		}
		y++;
	}
	return out;
}

raycastData raycastSingle(SDL_Renderer *renderer, xyvector &start, xyvector direction, bool &drawrays){
	SDL_SetRenderDrawColor(renderer, 0, 150, 150, 255);
	double xincrease = abs(1.0 / direction.x); //length the ray needs to travel to increase x coord by 1
	double yincrease = abs(1.0 / direction.y); //length the ray needs to travel to increase y coord by 1

	//xyvector ray = start;

	//which box of the map we're in
	int mapX = int(start.x), mapY = int(start.y), stepx, stepy;

	//length of ray from current position to next x or y-side
	double sideDistX;
	double sideDistY;

	bool side; //false = horizontal, true = vertical

	//calculate step and initial sideDist
	if (direction.x < 0) {
		stepx = -1;
		sideDistX = (start.x - mapX) * xincrease;
	} else {
		stepx = 1;
		sideDistX = (mapX + 1.0 - start.x) * xincrease;
	}

	if (direction.y < 0) {
		stepy = -1;
		sideDistY = (start.y - mapY) * yincrease;
	} else {
		stepy = 1;
		sideDistY = (mapY + 1.0 - start.y) * yincrease;
	}

	//perform DDA
	while (level[mapY][mapX] == 0) { // && ray.length() < renderdist
		//jump to next map square, OR in x-direction, OR in y-direction
		if (sideDistX < sideDistY) {
			sideDistX += xincrease;
			mapX += stepx;
			side = false;
		} else {
			sideDistY += yincrease;
			mapY += stepy;
			side = true;
		}
	}

	double distance, hitx, wallsegment;
	//Calculate distance of perpendicular ray (Euclidean distance will give fisheye effect!)
	if (side){
		distance = (mapY-start.y+(1-stepy)/2)/direction.y;
		hitx = start.x + distance * direction.x;
	} else {
		distance = (mapX-start.x+(1-stepx)/2)/direction.x;
		hitx = start.y + distance * direction.y;
	}

	if(drawrays){
		SDL_RenderDrawLine(renderer, int(start.x*minimapScale)+minimapX, int(start.y*minimapScale)+minimapY, int((start.x+direction.x*distance)*minimapScale + minimapX+3), int((start.y+direction.y*distance)*minimapScale + minimapY+3));
	}

	hitx -= floor(hitx);

	wallsegment = int(hitx*double(resolution));

	if((side && yincrease < 0) || (!side && xincrease > 0)){
		wallsegment = resolution - wallsegment - 1;
	}

	return (raycastData){level[mapY][mapX]-1, uint8_t(wallsegment), distance, side};
}

void minimap(SDL_Renderer *renderer, xyvector position, xyvector direction, xyvector plane, vector<vector<uint8_t>> &level, double vectorScale, SDL_Texture *walltex) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
	SDL_Rect dest = {minimapX, minimapY, levelwidth*minimapScale+6, levelheight*minimapScale+6};
	SDL_RenderFillRect(renderer, &dest);
	SDL_Rect src; src.w = resolution; src.h = resolution;

	for(int y = 0; y < levelheight; y++){
		for(int x = 0; x < levelwidth; x++){
			if(level[y][x] > 0){
				src.x = tileCoordsX[level[y][x]-1] * resolution;
				src.y = tileCoordsY[level[y][x]-1] * resolution;

				dest = {minimapX+x*minimapScale+3, minimapY+y*minimapScale+3, minimapScale, minimapScale};
				SDL_RenderCopy(renderer, walltex, &src, &dest);
				//SDL_SetRenderDrawColor(renderer, 100+10*level[y][x], 100+10*level[y][x], 100+10*level[y][x], 255);
				//SDL_RenderFillRect(renderer, &dest);
			}
		}
	}

	direction = direction.scale(minimapScale*vectorScale);
	position = position.scale(minimapScale*vectorScale);
	plane = plane.scale(minimapScale*vectorScale);

	//dest = {3+player.x, 3+player.y, minimapScale, minimapScale};
	//SDL_SetRenderDrawColor(renderer, 0, 255, 0, int(255/3));
	//SDL_RenderFillRect(renderer, &dest);

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	SDL_RenderDrawLine(renderer, position.x+direction.x+plane.x+minimapX, position.y+direction.y+plane.y+minimapY, position.x+direction.x-plane.x+minimapX, position.y+direction.y-plane.y+minimapY);

	for(int x = 0; x < 4; x++){
		SDL_RenderDrawLine(renderer, position.x+minimapX, position.y+minimapY, position.x+direction.x+minimapX, position.y+direction.y+minimapY);
		//dest = {player.x+direction.x+3, player.y+direction.y+3, minimapScale, minimapScale};
		//SDL_SetRenderDrawColor(renderer, 255, 0, 0, int(255/3));
		//SDL_RenderFillRect(renderer, &dest);
		direction = direction.rotate90cw();
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	}

	dest.x = position.x+minimapX-3;
	dest.y = position.y+minimapY-3;
	dest.w = 6;
	dest.h = 6;

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &dest);

	for(uint8_t i = 0; i < entities.size(); i++){
		/*switch(sprites[i].type){
			case 0:
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255*(i+1)/sprites.size());
				break;
			case 1:
				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255*(i+1)/sprites.size());
				break;
			case 2:
				SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255*(i+1)/sprites.size());
				break;
		}*/
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255*(i+1)/entities.size());
		dest.x = int(entities[i].x*minimapScale)+minimapX-3;
		dest.y = int(entities[i].y*minimapScale)+minimapY-3;
		dest.w = 6;
		dest.h = 6;

		SDL_RenderFillRect(renderer, &dest);
	}
}

void sortentities(xyvector position){
	entity temp;
	for (uint8_t i = 0; i < entities.size()-1; i++){
		// Last i elements are already in place
		for (uint8_t j = 0; j < entities.size()-i-1; j++){
			if ((entities[j].getvector()-position).length() < (entities[j+1].getvector()-position).length()){
				temp = entities[j];
				entities[j] = entities[j+1];
				entities[j+1] = temp;
			}
		}
	}
}

bool calculateCollision(xyvector v){
	if(level[int(v.y)][int(v.x)] != 0){
		return true;
	}
	for(uint8_t i = 0; i < entities.size(); i++){
		if((entities[i].getvector()-v).length() < 0.1){
			return true;
		}
	}
	return false;
}

//int

#endif /* GAMEFUNCTIONS_H_ */
