#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

#include "tileTausta.h"
#include "kuvanLataaja.h"
#include "map.h" //required for progressDisplay::drawMap's argument

#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff

bool initialize();

void setFullscreen(bool isFullscreen);

SDL_Rect makeRect(int x, int y, int w, int h);
SDL_Rect makeRect(int x, int y);

Uint32 makePixel_rgba8888(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void addSurfaceAlpha(SDL_Surface* src, SDL_Surface* dst, int off_x, int off_y);

class visible{
protected:
    SDL_Texture* texture;
public:
    visible();
    virtual SDL_Texture* update(); //update the texture and return it
};


class pixelDrawing : public visible{ //mini map
protected:
    Uint32* pixels;
    int w, h;
public:
    pixelDrawing(std::string file); //load initial image from file
    void putPixel(int x, int y, Uint32 pixel);
    void putPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b);
    //void drawDot(int x, int y, int diameter, int r, int g, int b);
    SDL_Texture* update();

    int getW();
    int getH();
};


class surfaceDrawing : public visible{ //blit images on a main SDL_Surface. Update converts that to a texture
protected:
    SDL_Surface* surface;

public:
    void loadFromFile(std::string file);

    surfaceDrawing(std::string file);

    void drawImage(SDL_Surface* image, int x, int y);
    void drawImageStretched(SDL_Surface* image);
    void drawImageScaled(SDL_Surface* image, int x, int y, int w, int h);
    void drawImageScaled(SDL_Surface* image, SDL_Rect* dstRect);

    SDL_Texture* update();

};


class textDrawing : public visible{ //score
protected:
    std::string text;
    SDL_Color textColor;
public:
    textDrawing();
    void setText(std::string);

    SDL_Texture* update();

};

class progressDisplay{
protected:

    tileTausta tausta;
    tileTausta roska;

    pixelDrawing miniMap;
    surfaceDrawing maa;
    textDrawing score;
    textDrawing loadDialog;

    kuvanLataaja spawns;
    kuvanLataaja sprites;

    kuvanLataaja ray;

    void loadImages(std::vector<std::string>);

    double viewZoom;
    double displayPos_x;
    double displayPos_y;


public:
    progressDisplay(std::string visibleMap, std::string logicalMap, std::string trashfile, std::vector<std::string> spawnFiles);

    void show(int xoff, int yoff); //kokoaa visible-olioista kuvan ja näyttää sen ikkunassa
    void showLogical();

    void setZoom(double z);
    void centerView(int x, int y);
    void spawn(int x, int y, int r);
    void piirraTausta();
    void piirraAivo(int size);
    int getW(); //hakee taustasta maailman leveyden
    int getH(); //hakee taustasta maailman korkeuden
    void drawMap(map& theMap);
    void drawDot(int logical_x, int logical_y, int dotSize);
    void piirraRoskaa(int world_x, int world_y, int r);
    void kuihdutaTaustaa(int world_x, int world_y, int r);
    void kerroPisteet(int pisteet);
    void setDialog(bool on_off);
    void clearDialog();

};


class titleDisplay : public visible{
protected:
    textDrawing title;
    textDrawing start;
    textDrawing demoText;

    SDL_Texture* bg;
public:
    titleDisplay();
    void show();
    void enterPressed(bool);

};

#endif
