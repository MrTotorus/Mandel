#ifndef __helperFunctions_h__
#define __helperFunctions_h__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void HSV_to_RGB(uint32_t *r, uint32_t *g, uint32_t *b, uint32_t h, uint32_t s, uint32_t v) {		// credit to ProgrammerSought
	// R,G,B from 0-255, H from 0-359, S,V from 0-100
	int i;
	float RGB_min, RGB_max;
	RGB_max = v*2.55f;
	RGB_min = RGB_max*(100 - s) / 100.0f;

	i = h / 60;
	int difs = h % 60; // factorial part of h

					   // RGB adjustment amount by hue 
	float RGB_Adj = (RGB_max - RGB_min)*difs / 60.0f;

	switch (i) {
	case 0:
		*r = RGB_max;
		*g = RGB_min + RGB_Adj;
		*b = RGB_min;
		break;
	case 1:
		*r = RGB_max - RGB_Adj;
		*g = RGB_max;
		*b = RGB_min;
		break;
	case 2:
		*r = RGB_min;
		*g = RGB_max;
		*b = RGB_min + RGB_Adj;
		break;
	case 3:
		*r = RGB_min;
		*g = RGB_max - RGB_Adj;
		*b = RGB_max;
		break;
	case 4:
		*r = RGB_min + RGB_Adj;
		*g = RGB_min;
		*b = RGB_max;
		break;
	default:		// case 5:
		*r = RGB_max;
		*g = RGB_min;
		*b = RGB_max - RGB_Adj;
		break;
	}
}

double map_value(double x, double in_min, double in_max, double out_min, double out_max) {		// credit to Arduino
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;					// Map an input to a specific range
}


#endif