/*
 * preferences.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Anhad
 */

#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#define CeilingColor {0, 0, 0}
#define FloorColor {128, 128, 128}

#define fov 60 //default: 60
#define resolution 128 //don't change this
#define renderdist 14
#define framecap 16.6 //set to 0 for unlimited framerate, otherwise framecap = 1000/capped fps. Default: 1000/60 fps = 16

#define movespeed 2 //default: 1 square/second
#define xsensitivity 0.2 //default: 0.2
#define ysensitivity 2 //default: 0.2

#define scanresolution (width) //Number of vertical columns to be calculated and drawn. Larger number = slower, max = width

#define minimapScale 10
#define gunscale 800
#define minimapX 0
#define minimapY 20

#define noclip false

#endif /* PREFERENCES_H_ */
