/* 
	Simple functions to convert HSV color space values to RGB and vice versa.
*/

#define MIN(a,b) (a < b ? a : b)
#define MINTHREE(a,b,c) MIN(MIN(a,b),c)

#define MAX(a,b) (a > b ? a : b)
#define MAXTHREE(a,b,c) MAX(MAX(a,b),c)

void HSVtoRGB(int* r, int* g, int* b, int h, int s, int v)
{
    int f;
    long p, q, t;

    if (s == 0)
    {
        *r = *g = *b = v;
        return;
    }

    f = ((h % 60) * 255) / 60;
    h /= 60;

    p = (v * (256 - s)) / 256;
    q = (v * (256 - (s * f) / 256)) / 256;
    t = (v * (256 - (s * (256 - f)) / 256)) / 256;

    switch (h) {
    case 0:
        *r = v;
        *g = t;
        *b = p;
        break;
    case 1:
        *r = q;
        *g = v;
        *b = p;
        break;
    case 2:
        *r = p;
        *g = v;
        *b = t;
        break;
    case 3:
        *r = p;
        *g = q;
        *b = v;
        break;
    case 4:
        *r = t;
        *g = p;
        *b = v;
        break;
    default:
        *r = v;
        *g = p;
        *b = q;
        break;
    }
}

void RGBtoHSV(int* h, int* s, int* v, int r, int g, int b) {
    int rgbMin, rgbMax;

    rgbMin = MINTHREE(r, g, b);
    rgbMax = MAXTHREE(r, g, b);

    *v = rgbMax;
    
    if (*v == 0) {
        *h = *s = 0;
        return;
    }

    *s = 255 * ((long)(rgbMax - rgbMin)) / *v;
    if (*s == 0) {
        *h = 0;
        return;
    }

    if (rgbMax == r)      *h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g) *h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else                  *h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
}