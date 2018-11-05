/*
 * main.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Anhad
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
using namespace std;

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
//#include <sys/time.h>
//#include <iomanip>
#include <cstdlib>

struct raycastData {
	uint8_t walltype;
	uint8_t wallsegment;
	double distance;
	bool side;
};

struct xyvector {
	double x;
	double y;

	xyvector operator + (xyvector& a){
		return (xyvector){this->x+a.x, this->y+a.y};
	}

	xyvector operator - (xyvector& a){
		return (xyvector){this->x-a.x, this->y-a.y};
	}

	xyvector& operator += (xyvector& a){
	      this->x += a.x;
	      this->y += a.y;
	      return *this;
	}

	xyvector& operator -= (xyvector& a){
		      this->x -= a.x;
		      this->y -= a.y;
		      return *this;
	}

	xyvector rotate(double angle){
		return (xyvector){this->x*cos(angle) - this->y*sin(angle), this->x*sin(angle) + this->y*cos(angle)};
	}

	xyvector rotate90cw(){
		return (xyvector){this->y, -this->x};
	}

	xyvector rotate90ccw(){
		return (xyvector){-this->y, this->x};
	}

	xyvector flip(){
		return (xyvector){-this->x, -this->y};
	}

	xyvector scale(double x, double y){
		return (xyvector){this->x * x, this->y * y};
	}

	xyvector scale(double a){
		return (xyvector){this->x * a, this->y * a};
	}

	double length(){
		return sqrt(this->x*this->x + this->y*this->y);
	}
};

struct entity {
	double x;
	double y;
	int type; // 0:player, 2:barrel, 3:health pack, 4:ammo pack
	double angle;
	int health;
	bool shooting;

	xyvector getvector(){
		return (xyvector){this->x, this->y};
	}
};

int width, height;

#define initdirection (xyvector){1, 0}
#define initplane (xyvector){0, -0.66} //-tan(fov/2)
#define initpos (xyvector){1.5, 10.5}

#endif /* MAIN_H_ */
