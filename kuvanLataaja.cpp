#include "kuvanLataaja.h"
#include <iostream>

using namespace std;

SDL_Surface* kuvanLataaja::haeKuva(const int& i){
    if(i >=0 && i<kuvat.size() )
        return kuvat.at(i);
    else {
        cout << "kuvanLataaja::haeKuva - ei kuvaa "<<i<<" \n";
        return NULL;
    }
}

int kuvanLataaja::lataa(const string& filename){
    SDL_Surface* loaded = IMG_Load(filename.c_str() );
    SDL_Surface* tmp = SDL_CreateRGBSurface(0, loaded->w, loaded->h, 32, RMASK, GMASK, BMASK, AMASK);
    SDL_SetSurfaceBlendMode(loaded, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(loaded, NULL, tmp, NULL);
    SDL_FreeSurface(loaded);

    if(tmp != NULL) {
        kuvat.push_back(tmp);
        cout << "Ladattiin kuva "<<filename<<".\n";
        return kuvat.size()-1;
    }
    else {
        cout << "Kuvaa " <<filename<<" ei voitu ladata.\n";
        return -1;
    }
}

kuvanLataaja::~kuvanLataaja() {
    while(!kuvat.empty()) {
        if(kuvat[0] != NULL)
            SDL_FreeSurface(kuvat[0]);

        kuvat.erase(kuvat.begin());
    }
}

int kuvanLataaja::size() {
    return kuvat.size();
}
