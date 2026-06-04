#pragma once
#include <iostream>
#include <cmath>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif
#define LETTER_BOUNDRY_SIZE     0.25f
#define TEXT_MAX_LAYERS         32
#define LETTER_BOUNDRY_COLOR    VIOLET

extern bool SHOW_LETTER_BOUNDRY;
extern bool SHOW_TEXT_BOUNDRY;

#include "raylib.h"
#include "rlgl.h"
typedef struct WaveTextConfig {
    Vector3 waveRange;
    Vector3 waveSpeed;
    Vector3 waveOffset;
} WaveTextConfig;

// Draw a codepoint in 3D space
void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);

// Draw a 2D text in 3D space
void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint);

// Draw a 2D text in 3D space and wave the parts that start with '~~' and end with '~~'
// This is a modified version of the original code by @Nighten found here https://github.com/NightenDushi/Raylib_DrawTextStyle
void DrawTextWave3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, WaveTextConfig *config, float time, Color tint);

// Measure a text in 3D ignoring the `~~` chars
Vector3 MeasureTextWave3D(Font font, const char *text, float fontSize, float fontSpacing, float lineSpacing);

// Generates a nice color with a random hue
Color GenerateRandomColor(float s, float v);