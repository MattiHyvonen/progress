#ifndef KUVANLATAAJA_H
#define KUVANLATAAJA_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>


#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff


class kuvanLataaja{
protected:
    std::vector<SDL_Surface*> kuvat;
public:
    ~kuvanLataaja();
    SDL_Surface* haeKuva(const int& i);

    int lataa(const std::string& filename);
    int size();
};

#endif
