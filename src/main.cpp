#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <nlohmann/json.hpp>
#include <fstream>
#include <Eigen/Dense>  
#include <stddef.h>     // Required for: NULL
#include <math.h>    

#include "raylib.h"
#include "TUI.h"
#include "Application.h"
#include "graphics.h"
#include "rlgl.h"
 
#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif
#define LETTER_BOUNDRY_SIZE     0.25f
#define TEXT_MAX_LAYERS         32
#define LETTER_BOUNDRY_COLOR    VIOLET
bool SHOW_LETTER_BOUNDRY = false;
bool SHOW_TEXT_BOUNDRY = false;
using json = nlohmann::json;


int main(void){

    terminal_interface();
    json config = json_parser();
    std::cout << config.dump(4) << std::endl;
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1500;
    const int screenHeight = 900;

    SetConfigFlags(FLAG_MSAA_4X_HINT|FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "openEM");

    bool spin = true;        // Spin the camera?
    bool multicolor = false; // Multicolor mode

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ -10.0f, 15.0f, -10.0f };   // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };          // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type

    int camera_mode = CAMERA_ORBITAL;

    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };

    // Use the default font
    Font font = GetFontDefault();
    float fontSize = 0.8f;
    float fontSpacing = 0.05f;
    float lineSpacing = -0.1f;

    // Set the text (using markdown!)
    char text[64] = "Radiation Pattern";
    Vector3 tbox = { 0 };
    int layers = 1;
    int quads = 0;
    float layerDistance = 0.01f;

    WaveTextConfig wcfg;
    wcfg.waveSpeed.x = wcfg.waveSpeed.y = 3.0f; wcfg.waveSpeed.z = 0.5f;
    wcfg.waveOffset.x = wcfg.waveOffset.y = wcfg.waveOffset.z = 0.35f;
    wcfg.waveRange.x = wcfg.waveRange.y = wcfg.waveRange.z = 0.45f;

    float time = 0.0f;

    // Setup a light and dark color
    Color light = MAROON;
    Color dark = RED;

    // Load the alpha discard shader
    Shader alphaDiscard = LoadShader(NULL, TextFormat("resources/shaders/glsl%i/alpha_discard.fs", GLSL_VERSION));

    // Array filled with multiple random colors (when multicolor mode is set)
    Color multi[TEXT_MAX_LAYERS] = { 0 };

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, camera_mode);

        // Handle font files dropped
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            // NOTE: We only support first ttf file dropped
            if (IsFileExtension(droppedFiles.paths[0], ".ttf"))
            {
                UnloadFont(font);
                font = LoadFontEx(droppedFiles.paths[0], (int)fontSize, 0, 0);
            }
            else if (IsFileExtension(droppedFiles.paths[0], ".fnt"))
            {
                UnloadFont(font);
                font = LoadFont(droppedFiles.paths[0]);
                fontSize = (float)font.baseSize;
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        // Handle Events
        if (IsKeyPressed(KEY_F1)) SHOW_LETTER_BOUNDRY = !SHOW_LETTER_BOUNDRY;
        if (IsKeyPressed(KEY_F2)) SHOW_TEXT_BOUNDRY = !SHOW_TEXT_BOUNDRY;
        if (IsKeyPressed(KEY_F3))
        {
            // Handle camera change
            spin = !spin;
            // we need to reset the camera when changing modes
            camera = (Camera3D){ 0 };
            camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };          // Camera looking at point
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
            camera.fovy = 45.0f;                                    // Camera field-of-view Y
            camera.projection = CAMERA_PERSPECTIVE;                 // Camera mode type

            if (spin)
            {
                camera.position = (Vector3){ -10.0f, 15.0f, -10.0f };   // Camera position
                camera_mode = CAMERA_ORBITAL;
            }
            else
            {
                camera.position = (Vector3){ 10.0f, 10.0f, -10.0f };   // Camera position
                camera_mode = CAMERA_FREE;
            }
        }

        // Handle clicking the cube
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Ray ray = GetMouseRay(GetMousePosition(), camera);

            // Check collision between ray and box
            RayCollision collision = GetRayCollisionBox(ray,
                            (BoundingBox){(Vector3){ cubePosition.x - cubeSize.x/2, cubePosition.y - cubeSize.y/2, cubePosition.z - cubeSize.z/2 },
                                          (Vector3){ cubePosition.x + cubeSize.x/2, cubePosition.y + cubeSize.y/2, cubePosition.z + cubeSize.z/2 }});
            if (collision.hit)
            {
                // Generate new random colors
                light = GenerateRandomColor(0.5f, 0.78f);
                dark = GenerateRandomColor(0.4f, 0.58f);
            }
        }

        // Handle text layers changes
        if (IsKeyPressed(KEY_HOME)) { if (layers > 1) --layers; }
        else if (IsKeyPressed(KEY_END)) { if (layers < TEXT_MAX_LAYERS) ++layers; }

        // Handle text changes
        if (IsKeyPressed(KEY_LEFT)) fontSize -= 0.5f;
        else if (IsKeyPressed(KEY_RIGHT)) fontSize += 0.5f;
        else if (IsKeyPressed(KEY_UP)) fontSpacing -= 0.1f;
        else if (IsKeyPressed(KEY_DOWN)) fontSpacing += 0.1f;
        else if (IsKeyPressed(KEY_PAGE_UP)) lineSpacing -= 0.1f;
        else if (IsKeyPressed(KEY_PAGE_DOWN)) lineSpacing += 0.1f;
        else if (IsKeyDown(KEY_INSERT)) layerDistance -= 0.001f;
        else if (IsKeyDown(KEY_DELETE)) layerDistance += 0.001f;
        else if (IsKeyPressed(KEY_TAB))
        {
            multicolor = !multicolor;   // Enable /disable multicolor mode

            if (multicolor)
            {
                // Fill color array with random colors
                for (int i = 0; i < TEXT_MAX_LAYERS; i++)
                {
                    multi[i] = GenerateRandomColor(0.5f, 0.8f);
                    multi[i].a = GetRandomValue(0, 255);
                }
            }
        }

        // Handle text input
        int ch = GetCharPressed();
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            // Remove last char
            int len = TextLength(text);
            if (len > 0) text[len - 1] = '\0';
        }
        else if (IsKeyPressed(KEY_ENTER))
        {
            // handle newline
            int len = TextLength(text);
            if (len < sizeof(text) - 1)
            {
                text[len] = '\n';
                text[len+1] ='\0';
            }
        }
        else
        {
            // append only printable chars
            int len = TextLength(text);
            if (len < sizeof(text) - 1)
            {
                text[len] = ch;
                text[len+1] ='\0';
            }
        }

        // Measure 3D text so we can center it
        tbox = MeasureTextWave3D(font, text, fontSize, fontSpacing, lineSpacing);

        quads = 0;                      // Reset quad counter
        time += GetFrameTime();         // Update timer needed by `DrawTextWave3D()`
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);
                DrawCubeV(cubePosition, cubeSize, dark);
                DrawCubeWires(cubePosition, 2.1f, 2.1f, 2.1f, light);

                DrawGrid(10, 2.0f);

                // Use a shader to handle the depth buffer issue with transparent textures
                // NOTE: more info at https://bedroomcoders.co.uk/posts/198
                BeginShaderMode(alphaDiscard);

                    // Draw the 3D text above the red cube
                    rlPushMatrix();
                        rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                        rlRotatef(90.0f, 0.0f, 0.0f, -1.0f);

                        for (int i = 0; i < layers; i++)
                        {
                            Color clr = light;
                            if (multicolor) clr = multi[i];
                            DrawTextWave3D(font, text, (Vector3){ -tbox.x/2.0f, layerDistance*i, -4.5f }, fontSize, fontSpacing, lineSpacing, true, &wcfg, time, clr);
                        }

                        // Draw the text boundry if set
                        if (SHOW_TEXT_BOUNDRY) DrawCubeWiresV((Vector3){ 0.0f, 0.0f, -4.5f + tbox.z/2 }, tbox, dark);
                    rlPopMatrix();

                    // Don't draw the letter boundries for the 3D text below
                    bool slb = SHOW_LETTER_BOUNDRY;
                    SHOW_LETTER_BOUNDRY = false;
                    //-------------------------------------------------------------------------

                    SHOW_LETTER_BOUNDRY = slb;
                EndShaderMode();

            EndMode3D();

            // Draw 2D info text & stats
            //-------------------------------------------------------------------------
            quads += TextLength(text)*2*layers;
            char *tmp = (char *)TextFormat("%2i layer(s) | %s camera | %4i quads (%4i verts)", layers, spin? "ORBITAL" : "FREE", quads, quads*4);
            int width = MeasureText(tmp, 10);
            DrawText(tmp, screenWidth - 20 - width, 10, 10, DARKGREEN);
            tmp = "YAGI-UDA";
            width = MeasureText(tmp, 10);
            DrawText(tmp, screenWidth - 20 - width, 25, 10, SKYBLUE);
            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadFont(font);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
