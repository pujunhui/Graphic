#pragma once 

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>

#define NOMINMAX 
#include <algorithm> 
#include <windows.h>

#define PI              3.14159265358979323
#define DEG2RAD(theta)  (0.01745329251994329 * (theta))
#define FRACTION(v)     ((v)-(int)(v))

struct RGBA {
    byte mB;
    byte mG;
    byte mR;
    byte mA;

    RGBA(
        byte r = 255,
        byte g = 255,
        byte b = 255,
        byte a = 255
    ) {
        mR = r;
        mG = g;
        mB = b;
        mA = a;
    }
};

#define ARRAY_BUFFER 0
#define ELEMENT_ARRAY_BUFFER 1
