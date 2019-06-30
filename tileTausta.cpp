#include "tileTausta.h"
#include "display.h"
#include "world.h"
#include <iostream>
#include <sstream>
#include <math.h> //ceil()

using namespace std;

//todo: lue nämä vaikka tiedostosta
const int TILE_W = 640;
const int TILE_H = 480;

void tileTausta::lataaKuvat(vector<std::string> tiedostot) {

    vector<SDL_Surface*> ladatutKuvat;

    /*Lataa kaikki eri kuvat vektoriin */
    for(int i=0; i<tiedostot.size(); i++) {
        SDL_Surface* tmp = IMG_Load(tiedostot.at(i).c_str() );
        if(tmp) {
            ladatutKuvat.push_back(tmp);
            cout << "Ladattiin kuva "<< tiedostot.at(i) << "\n";
        }
        else
            cout << "Kuvaa "<< tiedostot.at(i) << " ei voitu ladata\n";
    }

    /*Kopioi kuvia karttaan*/
    int kartanKoko = kartanLeveys_tiles * kartanKorkeus_tiles;
    for(int i=0; i<kartanKoko; i++) {
        SDL_Surface* sfc = ladatutKuvat.at(i % ladatutKuvat.size() );
        SDL_Surface* uusiKuva = SDL_ConvertSurface(sfc, sfc->format, sfc->flags);

        kartta.push_back(uusiKuva);
    }

    /*Poista ladatutKuvat*/
    while(ladatutKuvat.empty() == false) {
        SDL_FreeSurface(ladatutKuvat.at(0));
        ladatutKuvat.erase(ladatutKuvat.begin());
    }
}


void tileTausta::lataaKartta(string tiedosto){
    //Lataa kuvatiedosto, pilko se tileihin ja tallenna ne karttaan
    SDL_Surface* kokoKartta = IMG_Load(tiedosto.c_str() );
    if(kokoKartta == NULL)
        cout << "Kuvaa "<< tiedosto << " ei voitu ladata\n";

    //käytä oletuksena tämänkokoisia tilejä
    kartanLeveys_tiles = kokoKartta->w / 640;
    kartanKorkeus_tiles = kokoKartta->h / 480;

    //pitäisi olla 640x480 mutta voi olla parin pikselin virhe (voiko?)
    tilenLeveys_px = kokoKartta->w / kartanLeveys_tiles;
    tilenKorkeus_px = kokoKartta->h / kartanKorkeus_tiles;

    //huom! valmisTausta pitää luoda tämän jälkeen uudelleen

    //Kopioi tilet karttaan
    int kartanKoko = kartanLeveys_tiles * kartanKorkeus_tiles;
    SDL_SetSurfaceBlendMode(kokoKartta, SDL_BLENDMODE_NONE);

    for(int i=0; i<kartanKoko; i++) {
        int x = (i % kartanLeveys_tiles) * tilenLeveys_px;
        int y = (i / kartanKorkeus_tiles) * tilenKorkeus_px;

        SDL_Surface* uusiTile = SDL_CreateRGBSurface(kokoKartta->flags, tilenLeveys_px, tilenKorkeus_px, kokoKartta->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);

        SDL_SetSurfaceBlendMode(uusiTile, SDL_BLENDMODE_NONE);
/*
        Uint32 pixel = makePixel_rgba8888(255,255,255,0);
        SDL_FillRect(uusiTile, NULL, pixel);*/

        SDL_Rect srcRect = makeRect(x,y,tilenLeveys_px, tilenKorkeus_px);
        SDL_BlitSurface(kokoKartta, &srcRect, uusiTile, NULL);

        kartta.push_back(uusiTile);
    }

    cout << "Ladattiin kartta " << tiedosto << "\n";
    SDL_FreeSurface(kokoKartta);
}


tileTausta::tileTausta(string filename) {
//Lataa kartta yhdestä kuvatiedostosta ja pilko se tileiksi

    valmisTausta = NULL;

    lataaKartta(filename);

    valmisTausta = SDL_CreateRGBSurface(0, tilenLeveys_px, tilenKorkeus_px, 32, RMASK, GMASK, BMASK, AMASK);
}


tileTausta::tileTausta(int w, int h) :
            kartanLeveys_tiles(w),
            kartanKorkeus_tiles(h),
            tilenLeveys_px(TILE_W),
            tilenKorkeus_px(TILE_H)  {
//Lataa kartta useista kuvatiedostoista, nimet muotoa "media/tile1.jpg" jne.

    valmisTausta = NULL;
    valmisTausta = SDL_CreateRGBSurface(0, tilenLeveys_px, tilenKorkeus_px, 32, RMASK, GMASK, BMASK, AMASK);

    /*lataa kuvat karttaan*/
    vector<string> tiedostot;
    for(int i=1; i <= 64; i++) {

        stringstream ss;
        string filename;
        ss << "media/tile";
        ss << i;
        ss << ".jpg";
        ss >> filename;
        tiedostot.push_back(filename);
    }
    lataaKuvat(tiedostot);
}


tileTausta::~tileTausta() {
    if(valmisTausta !=NULL)
        SDL_FreeSurface(valmisTausta);

    while(kartta.empty() == false) {
        SDL_FreeSurface(kartta.at(0) );
        kartta.erase(kartta.begin() );
    }
}


SDL_Surface* tileTausta::haeTileKohdassa(int x, int y, bool kerro) { //x, y maailmakoordinaatteja pikseleinä

    int tile_x = x / tilenLeveys_px;
    int tile_y = y / tilenKorkeus_px;

    /*rajatarkistus*/
    while(tile_x < 0)   tile_x += kartanLeveys_tiles;
    tile_x %= kartanLeveys_tiles;

    while(tile_y < 0)   tile_y += kartanKorkeus_tiles;
    tile_y %= kartanKorkeus_tiles;

    if(kerro) cout << "tile[" << tile_x << ":" << tile_y << "]\n";

    return kartta.at(tile_y * kartanLeveys_tiles + tile_x);
}

SDL_Surface* tileTausta::haeTileKohdassa(int x, int y){
    return haeTileKohdassa( x, y, false);
}


void tileTausta::piirraKuvaTaustaan(SDL_Surface* kuva, int x, int y) { //piirtää annetun kuvan tileihin annettuun kohtaan

    bool kerro = false;

    x -= kuva->w/2;
    y -= kuva->h/2;

    x = wrap(x, 0, haeLeveys_px() );
    y = wrap(y, 0, haeKorkeus_px() );

    //tilet joihin piirretään
    vector<SDL_Surface*> tiles;
    //ceil(): pyöristys ylöspäin. Rajat: vähintään 2, enintään 10 tileä
    int dstTiles_w = bound(ceil(((float)kuva->w/tilenLeveys_px) +1), 2, 10);
    int dstTiles_h = bound(ceil(((float)kuva->h/tilenKorkeus_px) +1), 2, 10);

    tiles.resize(dstTiles_h * dstTiles_w);

    //haetaan tarvittava määrä tilejä
    for(int ty=0; ty<dstTiles_h; ty++) {
        for(int tx=0; tx<dstTiles_w; tx++) {
            if(kerro) cout << ty*dstTiles_w + tx << ":";
            tiles[ty*dstTiles_w + tx] = haeTileKohdassa(x + tx*tilenLeveys_px, y + ty*tilenKorkeus_px, kerro);
        }
    }
    if(kerro) cout << "ok\n";
    x = wrap(x, tilenLeveys_px);
    y = wrap(y, tilenKorkeus_px);

    //Kohta johon kuva piirretään
    int offset_x;
    int offset_y;

    //SDL_SetSurfaceBlendMode(kuva, SDL_BLENDMODE_NONE);

    //piirretään tilet alkaen vasemmasta yläkulmasta
    for(int ty=0; ty<dstTiles_h; ty++) {
        for(int tx=0; tx<dstTiles_w; tx++) {

            offset_x = x - tx*tilenLeveys_px;
            offset_y = y - ty*tilenKorkeus_px;

            int ti = ty*dstTiles_w + tx;
            //SDL_SetSurfaceBlendMode(tiles[ti], SDL_BLENDMODE_NONE);

            if(kerro) cout << ti << ": drawTo(" << offset_x << ", " << offset_y << ")\n";

            SDL_Rect blitRect = makeRect(offset_x, offset_y);
            if(SDL_BlitSurface(kuva, NULL, tiles[ti], &blitRect) <0)
                cout << "Ei voitu piirtää!\n";

        }
    }
    if(kerro) cout << "\n";
}


SDL_Surface* tileTausta::haeValmisTausta(int x, int y) {
//koostaa tileistä valmiin taustan annetusta kohdasta. x, y maailmakoordinaatteja pikseleinä. Koko on sama kuin tilen!

    SDL_Surface* keskiTile = haeTileKohdassa(x, y);
    SDL_Surface* oikeaTile = haeTileKohdassa(x + tilenLeveys_px, y);
    SDL_Surface* alaTile = haeTileKohdassa(x, y + tilenKorkeus_px);
    SDL_Surface* kulmaTile = haeTileKohdassa(x + tilenLeveys_px, y + tilenKorkeus_px);

    while(x < 0)
        x += tilenLeveys_px;
    x %= tilenLeveys_px;

    while(y < 0)
        y += tilenKorkeus_px;
    y %= tilenKorkeus_px;

    /*Lasketaan kohta, johon keskiTile piirretään. Se on negatiivinen eli ruudun vasemman ja yläreunan ulkopuolella */
    int offset_x = x * -1;
    int offset_y = y * -1;

    /*Piirretään keskiTile joka on oikeasti vasemmassa yläkulmassa oleva tile */
    SDL_Rect blitRect = makeRect(offset_x, offset_y);
    SDL_BlitSurface(keskiTile, NULL, valmisTausta, &blitRect);

    //cout << "xy: " << offset_x << " : " << offset_y << "\n";

    /*piirretään oikeaTile oikealle puolelle*/
    offset_x += tilenLeveys_px;
    blitRect = makeRect(offset_x, offset_y);
    SDL_BlitSurface(oikeaTile, NULL, valmisTausta, &blitRect);

    /*kulmaTile oikealle ja alas*/
    offset_y += tilenKorkeus_px;
    blitRect = makeRect(offset_x, offset_y);
    SDL_BlitSurface(kulmaTile, NULL, valmisTausta, &blitRect);

    /*alaTile alapuolelle, eli kulmatilestä nähden vasemmalle*/
    offset_x -= tilenLeveys_px;
    blitRect = makeRect(offset_x, offset_y);
    SDL_BlitSurface(alaTile, NULL, valmisTausta, &blitRect);
    /**/
    return valmisTausta;

}


SDL_Surface* tileTausta::haeValmisTausta(SDL_Rect taustaRect, bool kerro) {
    return haeValmisTausta(taustaRect.x, taustaRect.y, taustaRect.w, taustaRect.h, kerro);
}


SDL_Surface* tileTausta::haeValmisTausta(int x, int y, int w, int h, bool kerro) {
kerro=false;
//koostaa tileistä annetunkokoisen taustan annetusta kohdasta. x, y maailmakoordinaatteja pikseleinä.

    //jos valmisTaustan koko muuttuu, luodaan se uudestaan
    //if(valmisTausta->w != w || valmisTausta->h != h) {
        if(valmisTausta != NULL) SDL_FreeSurface(valmisTausta);
        valmisTausta = SDL_CreateRGBSurface(0,w,h,32,RMASK, GMASK, BMASK, AMASK);
        if(valmisTausta->w != w || valmisTausta->h != h)
            cout << "virhe: " << SDL_GetError()<< "\n";
    //}

    if(kerro) cout << "tausta: " << valmisTausta->w << "x" << valmisTausta->h << "\n";
    if(kerro) cout << "tile size: " << tilenLeveys_px << "x" << tilenKorkeus_px << "\n";

    //piirrettävät tilet
    vector<SDL_Surface*> tiles;

    //ceil(): pyöristys ylöspäin
    int shownTiles_w = ceil(((float)w/tilenLeveys_px) +1);
    int shownTiles_h = ceil(((float)h/tilenKorkeus_px) +1);

    //rajatarkistus
    if(shownTiles_h < 2 || shownTiles_w < 2)
        cout << "haevalmistausta, virhe: " << shownTiles_w << ", " << shownTiles_h << "\n";

    tiles.resize(shownTiles_h * shownTiles_w);

    //haetaan tarvittava määrä tilejä
    for(int ty=0; ty<shownTiles_h; ty++) {
        for(int tx=0; tx<shownTiles_w; tx++) {
            if(kerro) cout << ty*shownTiles_w + tx << ":";
            tiles[ty*shownTiles_w + tx] = haeTileKohdassa(x + tx*tilenLeveys_px, y + ty*tilenKorkeus_px, kerro);
        }
    }

    while(x < 0)
        x += tilenLeveys_px;
    x %= tilenLeveys_px;

    while(y < 0)
        y += tilenKorkeus_px;
    y %= tilenKorkeus_px;

    //Lasketaan kohta, johon vasen yläkulma piirretään. Se on negatiivinen eli ruudun vasemman ja yläreunan ulkopuolella
    int offset_x;
    int offset_y;

    SDL_SetSurfaceBlendMode(valmisTausta, SDL_BLENDMODE_NONE);

    //piirretään tilet alkaen vasemmasta yläkulmasta
    for(int ty=0; ty<shownTiles_h; ty++) {
        for(int tx=0; tx<shownTiles_w; tx++) {

            offset_x = x*-1 + tx*tilenLeveys_px;
            offset_y = y*-1 + ty*tilenKorkeus_px;

            int ti = ty*shownTiles_w + tx;
            SDL_SetSurfaceBlendMode(tiles[ti], SDL_BLENDMODE_NONE);

            if(kerro) cout << ti << ": draw(" << offset_x << ", " << offset_y << ")\n";

            SDL_Rect blitRect = makeRect(offset_x, offset_y);
            if(SDL_BlitSurface(tiles[ti], NULL, valmisTausta, &blitRect) <0)
                cout << "Ei voitu piirtää!\n";

        }
    }
    if(kerro) cout << "\n";
    return valmisTausta;
}


int tileTausta::haeLeveys_px() {return kartanLeveys_tiles * tilenLeveys_px;}
int tileTausta::haeKorkeus_px() {return kartanKorkeus_tiles * tilenKorkeus_px;}


void tileTausta::addAlpha(int x, int y, SDL_Surface* alphaMask) {

    bool kerro = false;
/*
                                //skaalaa alfamaski ----------------- tee valmiiksi erikokoisia
    SDL_Surface* tmpA = SDL_CreateRGBSurface(0, alphaMask->w, alphaMask->h, 32, RMASK, GMASK, BMASK, AMASK);
    SDL_SetSurfaceBlendMode(alphaMask, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(alphaMask, NULL, tmpA, NULL);
*/

    //x, y keskikohdan koordinaatit maailmassa --> laske yläkulman paikka
    x = wrap(x, haeLeveys_px() );
    y = wrap(y, haeKorkeus_px() );

    //tilet joihin piirretään
    vector<SDL_Surface*> tiles;

    //ceil(): pyöristys ylöspäin. Rajat: vähintään 1, enintään 16
    int dstTiles_w = bound(ceil(((float)alphaMask->w/tilenLeveys_px)) +1, 2, 16);
    int dstTiles_h = bound(ceil(((float)alphaMask->h/tilenKorkeus_px)) +1, 2, 16);

    tiles.resize(dstTiles_h * dstTiles_w);

    //haetaan tarvittava määrä tilejä
    for(int ty=0; ty<dstTiles_h; ty++) {
        for(int tx=0; tx<dstTiles_w; tx++) {
            if(kerro) cout << ty*dstTiles_w + tx << ":";
            tiles[ty*dstTiles_w + tx] = haeTileKohdassa(x + tx*tilenLeveys_px, y + ty*tilenKorkeus_px, kerro);
        }
    }
    if(kerro) cout << "ok\n";

    // nyt x, y koordinaatit tilen sisällä:
    x = wrap(x, tilenLeveys_px);
    y = wrap(y, tilenKorkeus_px);

    //Kohta johon kuva piirretään
    int offset_x;
    int offset_y;

    for(int ty=0; ty<dstTiles_h; ty++) {
        for(int tx=0; tx<dstTiles_w; tx++) {

            offset_x = x - tx*tilenLeveys_px;
            offset_y = y - ty*tilenKorkeus_px;

            int ti = ty*dstTiles_w + tx;

            if(kerro) cout << ti << ": drawTo(" << offset_x << ", " << offset_y << ")\n";

            addSurfaceAlpha(alphaMask, tiles[ti], offset_x, offset_y);
        }
    }
    if(kerro) cout << "\n";

}


