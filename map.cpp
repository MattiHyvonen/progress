#include "map.h"
#include "world.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

#define rmask 0xff000000
#define bmask 0x00ff0000
#define gmask 0x0000ff00
#define amask 0x000000ff

double suckFromSquare(mapSquareT& theSquare) {
    double yield =  theSquare.resources * 0.01 * theSquare.suckers;

    if(theSquare.resources < yield)
        yield = theSquare.resources;

    theSquare.resources -= yield;

    if(yield<0) yield=0;

    return yield;
}


double forceSuckFromSquare(mapSquareT& theSquare) { //suck regardless of suckers in map
    double yield =  theSquare.resources * 0.01;
    if(yield<0) yield=0;
    theSquare.resources -= yield;

    return yield;
}


mapSquareT& map::squareAt(int i) {
    return squares[i];
}


mapSquareT& map::squareAt(int x, int y) {
    return squareAt(y*mapW+x);
}


mapSquareT::mapSquareT() {
    resources = 0;
    suckers = 0;
    friction = 0.05;
    buildings = false;
    isUpdated = true;
}


int map::getW() {
    return mapW;
}


int map::getH() {
    return mapH;
}


int map::getSize() {
    return mapSize;
}


void map::resize(int w, int h) {
    mapW = w;
    mapH = h;
    mapSize = w*h;
    squares.resize(mapSize);
}


map::map(int w, int h) {
    srand(time((NULL)));
    resize(w,h);
}


void map::randomize() {
    for(int i=0; i<mapSize; i++) {
        squares[i].resources = rand()%256;

    }
}


double map::suckAt(int x, int y, int r){ //suck from a square of given diameter centered at (x,y)

    int start_i = y - r;
    int end_i = y + r;
    int start_j = x - r;
    int end_j = x + r;

    start_i = bound(start_i, 0, mapH);
    end_i = bound(end_i, start_i, mapH);
    start_j = bound(start_j, 0, mapW);
    end_j = bound(end_j, start_j, mapW);
    int yieldSum = 0;

    int rowOffset;

    for(int i = start_i; i < end_i; i++){//per row
        rowOffset = i*mapW;

        for(int j=start_j; j < end_j; j++) { //per square
            yieldSum += suckFromSquare(squares[rowOffset+j] );
            squares[rowOffset+j].isUpdated = true;
        }
    }

    //cout << start_j << ":" << end_j <<" , "<<start_i << ":"<<end_i<<"\n";

    return yieldSum;
}


double map::forceSuckAt(int x, int y, int r) {  //suck regardless of suckers in map
    int start_i = y - r;
    int end_i = y + r;
    int start_j = x - r;
    int end_j = x + r;

    start_i = bound(start_i, 0, mapH);
    end_i = bound(end_i, start_i, mapH);
    start_j = bound(start_j, 0, mapW);
    end_j = bound(end_j, start_j, mapW);
    int yieldSum = 0;

    int rowOffset;

    for(int i = start_i; i < end_i; i++){//per row
        rowOffset = i*mapW;

        for(int j=start_j; j < end_j; j++) { //per square
            yieldSum += forceSuckFromSquare(squares[rowOffset+j] );
            squares[rowOffset+j].isUpdated = true;
        }
    }

    //cout << start_j << ":" << end_j <<" , "<<start_i << ":"<<end_i<<"\n";

    return yieldSum;
}


void map::addSuckersAt(int x, int y, int r){ //add a bunch of suckers

    int start_i = y - r;
    int end_i = y + r;
    int start_j = x - r;
    int end_j = x + r;

    start_i = bound(start_i,0, mapH);
    end_i = bound(end_i, start_i, mapH);
    start_j = bound(start_j, 0, mapW);
    end_j = bound(end_j, start_j, mapW);

    int rowOffset;

    for(int i = start_i; i < end_i; i++){//per row
        rowOffset = i*mapW;

        for(int j=start_j; j < end_j; j++) { //per square
            squares[rowOffset+j].suckers++;
            squares[rowOffset+j].friction *= 0.995;
            squares[rowOffset+j].isUpdated = true;
        }
    }
}


void map::recursiveAddSuckersAt(int x, int y, int r) {
   //add suckers at diameter
   addSuckersAt(x, y, r);
   if(r>1) recursiveAddSuckersAt(x,y,r*0.5);
}


void map::addBuildingsAt(int x, int y, int r) {
    int start_i = y - r;
    int end_i = y + r;
    int start_j = x - r;
    int end_j = x + r;

    start_i = bound(start_i,0, mapH);
    end_i = bound(end_i, start_i, mapH);
    start_j = bound(start_j, 0, mapW);
    end_j = bound(end_j, start_j, mapW);

    int rowOffset;

    for(int i = start_i; i < end_i; i++){//per row
        rowOffset = i*mapW;

        for(int j=start_j; j < end_j; j++) { //per square
            squares[rowOffset+j].buildings = true;
            squares[rowOffset+j].friction = 0.01;
            squares[rowOffset+j].isUpdated = true;
        }
    }
}



map loadMap(const char* filename) {
 SDL_Surface* tmp = IMG_Load(filename);
 map result(tmp->w, tmp->h);

 for(int i=0; i<tmp->w * tmp->h; i++) {
  Uint8 *p = (Uint8 *)tmp->pixels + i * 3;
  Uint32 pixel = *(Uint32*)p;

  Uint8 b = pixel >> 16;
  Uint8 g = pixel >> 8;
  Uint8 r = pixel & 0xff;

  result.squareAt(i).resources = g;
  result.squareAt(i).friction = (double)r / 1000;
  result.squareAt(i).suckers = 0;

  //cout << (int)r << ", " << (int)g << ", " << (int)b << "\n";

 }

 SDL_FreeSurface(tmp);

 return result;
}

