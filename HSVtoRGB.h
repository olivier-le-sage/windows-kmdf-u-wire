/*
	A simple function to convert HSV color space values to RGB.
	Taken from: http://www.nunosantos.net/archives/114
*/

#pragma once
void HSVtoRGB(int* r, int* g, int* b, int h, int s, int v);
void RGBtoHSV(int* h, int* s, int* v, int r, int g, int b);