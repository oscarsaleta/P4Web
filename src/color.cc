#include "color.h"

P4RGBITEM XFigToRGB[NUMXFIGCOLORS] =
{
    // 8 pure colours:

    {  0,  0,  0},      // BLACK
    {  0,  0,255},      // BLUE
    {  0,255,  0},      // GREEN
    {  0,255,255},      // CYAN
    {255,  0,  0},      // RED
    {255,  0,255},      // MAGENTA
    {255,255,  0},      // YELLOW
    {255,255,255},      // WHITE

    // shaded colours (ordered from dark to light):

    {  0,  0,143},      // BLUE1
    {  0,  0,176},      // BLUE2
    {  0,  0,209},      // BLUE3
    {135,207,255},      // BLUE4
    {  0,143,  0},      // GREEN1
    {  0,176,  0},      // GREEN2
    {  0,209,  0},      // GREEN3
    {  0,143,143},      // CYAN1
    {  0,176,176},      // CYAN2
    {  0,209,209},      // CYAN3
    {143,  0,  0},      // RED1
    {176,  0,  0},      // RED2
    {209,  0,  0},      // RED3
    {143,  0,143},      // MAGENTA1
    {176,  0,176},      // MAGENTA2
    {209, 32,209},      // MAGENTA3
    {128, 48,  0},      // BROWN1
    {161, 64,  0},      // BROWN2
    {191, 97,  0},      // BROWN3
    {255,128,128},      // PINK1
    {255,161,161},      // PINK2
    {255,191,191},      // PINK3
    {255,224,224},      // PINK4
    {255,214,  0}       // GOLD
};


