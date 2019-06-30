#ifndef TILETAUSTA_H
#define TILETAUSTA_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class tileTausta{
protected:
    int kartanLeveys_tiles;
    int kartanKorkeus_tiles;
    int tilenLeveys_px;
    int tilenKorkeus_px;

    std::vector<SDL_Surface*> kartta;

    SDL_Surface* valmisTausta;

public:
    tileTausta(int w, int h);
    tileTausta(std::string filename);
    ~tileTausta();

    void lataaKuvat(std::vector<std::string> tiedostot); //lataa karttaan kuvat annetuista tiedostonimistä
    void lataaKartta(std::string tiedosto); //pilko kuva annetunkokoisiin tileihin ja tallenna karttaan
    SDL_Surface* haeTileKohdassa(int x, int y); //x, y kartan pikseleinä
    SDL_Surface* haeTileKohdassa(int x, int y, bool kerro);

    void piirraKuvaTaustaan(SDL_Surface* kuva, int x, int y); //piirtää annetun kuvan tileihin annettuun kohtaan
    SDL_Surface* haeValmisTausta(int x, int y); //koostaa tileistä taustan annetusta kohdasta. x, y kartan pikseleinä. Koko on sama kuin tilen!
    SDL_Surface* haeValmisTausta(int x, int y, int w, int h, bool kerro); //koostaa tileistä annetunkokoisen taustan annetusta kohdasta. x, y kartan pikseleinä.
    SDL_Surface* haeValmisTausta(SDL_Rect taustaRect, bool kerro); //koostaa tileistä annetunkokoisen taustan annetusta kohdasta. x, y kartan pikseleinä.

    void addAlpha(int x, int y, SDL_Surface* alphaMask);

    int haeLeveys_px(); //hakee kartan mitat pikseleinä
    int haeKorkeus_px(); //hakee kartan mitat pikseleinä
};

#endif
