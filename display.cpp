#include "display.h"
#include "world.h"
#include "map.h"
#include <iostream>
#include <sstream>
using namespace std;


SDL_Renderer* gRenderer;
SDL_Window* gWindow;
TTF_Font* gFont;


Uint32 makePixel_rgba8888(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    Uint32 pixel = (r << 24) | (g << 16) | (b << 8) | a;
    return pixel;
}


Uint32 addPixelAlpha(Uint32 pixel, int a) {
    a += (pixel & 0xff);
    pixel = pixel & 0xffffff00;
    pixel = pixel | a;
    return pixel;
}


void addSurfaceAlpha(SDL_Surface* src, SDL_Surface* dst, int off_x, int off_y) {
    //add src's alpha to dst. Offset is src's offset relative to dst
    //cout << "addSurfaceAlpha(" << off_x << ", " << off_y << ")\n";

    int src_i, dst_i;
    int bpp = src->format->BytesPerPixel;
    Uint8* src_p;
    Uint8* dst_p;

    int w, h;

    int start_x = bound(-off_x, 0, src->w);
    int start_y = bound(-off_y, 0, src->h);

    int end_x = bound(dst->w - off_x, 0, src->w);
    int end_y = bound(dst->h - off_y, 0, src->h);

    //cout << "start_x: " << start_x << ", start_y: " << start_y <<"\n";
    //cout << "end_x: " << end_x << ", end_y: " << end_y << "\n\n";

    for(int y=start_y; y < end_y; y++) {
        for(int x=start_x; x < end_x; x++) {

            src_i = y * src->pitch + x * bpp;
            dst_i = (y + off_y)*dst->pitch + (x + off_x) * bpp;

            src_p = (Uint8 *)src->pixels + src_i;
            dst_p = (Uint8 *)dst->pixels + dst_i;

            int a= *(Uint32*)src_p & 0xff;

            *(Uint32*)dst_p = addPixelAlpha(*(Uint32*)dst_p, a);
        }
    }
}


bool initialize(){

    static bool initialized = false;

    if(initialized)
        return true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		//cout << "Init error: " << SDL_GetError() << '\n';
		return false;
	}

	SDL_ShowCursor(0);

	TTF_Init();
    gFont = TTF_OpenFont( "media/font.ttf", 64 );

    gWindow = SDL_CreateWindow("SDL",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_SHOWN);

    if (gWindow == NULL) {
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    initialized = true;
    return true;
}


void setFullscreen(bool isFullscreen) {
    SDL_SetWindowFullscreen(gWindow, isFullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN);
}


SDL_Rect makeRect(int x, int y, int w, int h) {
    SDL_Rect tulos;
    tulos.x = x;
    tulos.y = y;
    tulos.w = w;
    tulos.h = h;
    return tulos;
}


SDL_Rect makeRect(int x, int y) {
    SDL_Rect tulos;
    tulos.x = x;
    tulos.y = y;
    return tulos;
}


/* class visible ------------------------------------------------------------*/
visible::visible() {
}


SDL_Texture* visible::update(){ //update the texture and return it
    //cout << "visible::update\n";
    return texture;
}


/* class pixelDrawing : public visible --------------------------------------*/
pixelDrawing::pixelDrawing(string file){ //load initial image from file

    initialize();

    SDL_Surface* tmp = IMG_Load(file.c_str() );
    if(!tmp)
        cout << "Kuvaa "<<file<<" ei voitu ladata: " << SDL_GetError() << "\n";
    else{
        w = tmp->w;
        h = tmp->h;

        pixels = new Uint32[w*h];

        texture = SDL_CreateTexture(gRenderer,
                            SDL_PIXELFORMAT_RGBA8888,
                            SDL_TEXTUREACCESS_STREAMING,
                            w, h);

        if (texture != NULL)
            cout << "Ladattiin looginen kartta (" << w << ", " << h << ")\n";
        else
            cout << "Tekstuuria ei voitu luoda: "<< SDL_GetError() << "\n";

        SDL_FreeSurface(tmp);
    }
}


void pixelDrawing::putPixel(int x, int y, Uint32 pixel) {
    pixels[y*w + x] = pixel;
}


void pixelDrawing::putPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    putPixel(x, y, makePixel_rgba8888(r,g,b,0xff) );
}


SDL_Texture* pixelDrawing::update(){
    SDL_UpdateTexture(texture, NULL, pixels, w*sizeof(Uint32) );
    return texture;
}


int pixelDrawing::getW(){return w;}
int pixelDrawing::getH(){return h;}


/* class surfaceDrawing : public visible ------------------------------------*/
void surfaceDrawing::loadFromFile(string file) {
    if(initialize() ){
        if(surface != NULL) {
            SDL_FreeSurface(surface);
            surface = NULL;
        }

        SDL_Surface* tmp = IMG_Load(file.c_str() );

        if(tmp == NULL)
            cout << "Kuvaa " << file << " ei voitu ladata: " << SDL_GetError() << "\n";
        else {

            surface = SDL_CreateRGBSurface(0, tmp->w, tmp->h, 32, RMASK, GMASK, BMASK, 0);
            SDL_BlitSurface(tmp, NULL, surface, NULL);

            SDL_FreeSurface(tmp);

            if(surface == NULL)
                cout << "Kuvaa " << file << " ei voitu ladata: " << SDL_GetError() << "\n";
            else
                cout << "Ladattiin kuva \""<< file <<"\".\n";
        }
    }
}


surfaceDrawing::surfaceDrawing(string file){
    if(initialize() ){
        surface = NULL;
        //loadFromFile(file);
        surface = SDL_CreateRGBSurface(0, 640, 480, 32, RMASK, GMASK, BMASK, AMASK);

        texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640,480 );
        if(texture == NULL)
            cout << "Couldn't create texture! " << SDL_GetError() << "\n";

    }
}


void surfaceDrawing::drawImage(SDL_Surface* image, int x, int y) {
    bool error = false;
    if(image != NULL) {

        x -= image->w/2;
        y -= image->h/2;

        SDL_Rect blitRect = makeRect(x,y);
        if(SDL_BlitSurface(image, NULL, surface, &blitRect) <0)
            error = true;
    }
    else
        error = true;

    if (error)
        cout << "ei voitu piirtää kuvaa: " << SDL_GetError() << "\n";
}


void surfaceDrawing::drawImageStretched(SDL_Surface* image){
    bool error = false;
    if(image != NULL) {
        if(SDL_BlitScaled(image, NULL, surface, NULL) <0) error=true;
    }
    else
        error = false;

    if(error)
        cout << "ei voitu piirtää kuvaa: " << SDL_GetError() <<"\n";
}


void surfaceDrawing::drawImageScaled(SDL_Surface* image, int x, int y, int w, int h) {
    SDL_Rect dstRect = makeRect(x,y,w,h);
    drawImageScaled(image, &dstRect);
}


void surfaceDrawing::drawImageScaled(SDL_Surface* image, SDL_Rect* dstRect){
    bool error = false;
    if(image!= NULL) {
        if(SDL_BlitScaled(image, NULL, surface, dstRect) <0) error=true;
    }
    else
        error = false;

    if(error)
        cout << "ei voitu piirtää kuvaa: " << SDL_GetError() <<"\n";
}


SDL_Texture* surfaceDrawing::update() {
    if(SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch) <0)
        cout << "update: " << SDL_GetError() << "\n";
    return texture;
}


/* class textDrawing : public visible ---------------------------------------*/

textDrawing::textDrawing() {
    texture = NULL;
    initialize();
    textColor = { 0, 255, 0 };
    cout << "ok\n";
}

void textDrawing::setText(std::string newText) {
    cout << "set text: "<< newText << "...";
    text = newText;
    cout << "ok\n";
}

SDL_Texture* textDrawing::update() {

    if(texture != NULL)
        SDL_DestroyTexture(texture);

    SDL_Color bgColor = {80,80,80};

    //SDL_Surface* tmp = TTF_RenderText_Solid( gFont, text.c_str(), textColor);
    SDL_Surface* tmp = TTF_RenderText_Shaded(gFont, text.c_str(), textColor, bgColor);
    texture = SDL_CreateTextureFromSurface( gRenderer, tmp );

    SDL_FreeSurface(tmp);

    return texture;
}


/* class progressDisplay---------------------------------------------------*/
void progressDisplay::loadImages(vector<string> spawnFiles) {
    //lataa spawnattavat kuvat
    for(int i=1; i < spawnFiles.size(); i++) {

        spawns.lataa(spawnFiles[i] );
    }


    for(int i=1; i <= 24; i++) {
        stringstream ss;
        string filename;

        ss << "media/alphaRay";
        ss << i;
        ss << ".png";
        ss >> filename;

        ray.lataa(filename);
    }


    sprites.lataa("media/aivo.png");
    sprites.lataa("media/raha1.png");

}


progressDisplay::progressDisplay(string visibleMap, string logicalMap, string trashfile, vector<string> spawnFiles) :
            miniMap(logicalMap),
            maa(visibleMap),
            tausta(visibleMap),
            roska(trashfile),
            viewZoom(1),
            displayPos_x(getW()/2),
            displayPos_y(getH()/2)
{
    initialize();
    loadImages(spawnFiles);
    clearDialog();
}


void progressDisplay::show(int xoff, int yoff) {

    xoff = wrap(xoff, miniMap.getW()-1);
    yoff = wrap(yoff, miniMap.getH()-1);

    static SDL_Rect mapRect = makeRect(5, 5, 100,75);
    static SDL_Rect scoreRect = makeRect(1000, 5, 200, 30);
    static SDL_Rect dialogRect = makeRect(500, 350, 400, 150);

    SDL_Texture* mmTexture = miniMap.update();
    SDL_Texture* repeatingMap = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, miniMap.getW(), miniMap.getH());

    SDL_SetRenderTarget(gRenderer, repeatingMap);
    SDL_Rect target = makeRect(-xoff, -yoff, miniMap.getW(), miniMap.getH() );

    //cout << "(" << target.x << ":" << target.y << "), " << target.w << "x" << target.h << "\n";

    if(SDL_RenderCopy(gRenderer, mmTexture, NULL, &target)< 0)
        cout << "renderCopy: " << SDL_GetError() << "\n";
    target.x += target.w;
    if(SDL_RenderCopy(gRenderer, mmTexture, NULL, &target)< 0)
        cout << "renderCopy: " << SDL_GetError() << "\n";
    target.y += target.h;
        if(SDL_RenderCopy(gRenderer, mmTexture, NULL, &target)< 0)
        cout << "renderCopy: " << SDL_GetError() << "\n";
    target.x -= target.w;
        if(SDL_RenderCopy(gRenderer, mmTexture, NULL, &target)< 0)
        cout << "renderCopy: " << SDL_GetError() << "\n";

    //cout << "drew 4x map\n";

    SDL_SetRenderTarget(gRenderer, NULL);

    SDL_RenderCopy(gRenderer, maa.update(), NULL, NULL);
    SDL_RenderCopy(gRenderer, repeatingMap , NULL, &mapRect);
    SDL_RenderCopy(gRenderer, score.update(), NULL, &scoreRect);
    SDL_RenderCopy(gRenderer, loadDialog.update(), NULL, &dialogRect);
    //cout << "copied textures\n";
    SDL_RenderPresent(gRenderer);

    SDL_DestroyTexture(repeatingMap);
    //cout << "rendered\n";
}


void progressDisplay::showLogical() {

    SDL_RenderCopy(gRenderer, miniMap.update(), NULL, NULL);
    SDL_RenderPresent(gRenderer);
}



void progressDisplay::setZoom(double z) {

    static smooth zoomSetter(50);
    zoomSetter.add(bound(z, 0.01, 10) );

    viewZoom = zoomSetter.get();
}


void progressDisplay::centerView(int x, int y) {
    //cout << "center view: (" << x << ", " << y << ")\n";
    displayPos_x = wrap(x-320*viewZoom, getW() );
    displayPos_y = wrap(y-240*viewZoom, getH() );
}


void progressDisplay::spawn(int x, int y, int r) {

    int spawn_i = bound(((sqrt(r)-5)/3), 0, spawns.size()-1 );
    //cout << "spawn: " << x << ", " << y << ", " << spawn_i;
    roska.piirraKuvaTaustaan(spawns.haeKuva(spawn_i), x, y);
}


void progressDisplay::piirraTausta() {
    //cout << "piirraTausta(" << displayPos_x << ", " << displayPos_y << ")...\n";
    SDL_Rect displayRect = makeRect(displayPos_x, displayPos_y, 640*viewZoom, 480*viewZoom);

    //piirrä tausta
    maa.drawImageStretched(tausta.haeValmisTausta(displayRect, false) );

    //cout << "ok\n";
}


void progressDisplay::piirraAivo(int size) {
    //piirrä aivo
    int aivonKoko = size / viewZoom;
    maa.drawImageScaled(sprites.haeKuva(0), 320 - (aivonKoko/2), 240 - (aivonKoko/2), aivonKoko, aivonKoko);

}


void progressDisplay::drawMap(map& theMap) {
    for(int y=0; y < miniMap.getH(); y++) {
        for(int x=0; x < miniMap.getW(); x++) {
            if(theMap.squareAt(x,y).isUpdated) {
                char r,g,b;

                if(theMap.squareAt(x,y).buildings) {
                    r = 80;
                    g = 80;
                    b = 80;
                }
                else {
                    r = theMap.squareAt(x,y).friction * 1000;
                    g = theMap.squareAt(x,y).resources;
                    b = theMap.squareAt(x,y).suckers * 20;
                }

                miniMap.putPixel(x, y, r, g, b);
                theMap.squareAt(x,y).isUpdated = false;
            }
        }
    }
}


void progressDisplay::drawDot(int logical_x, int logical_y, int dotSize) {

    //cout << "drawDot(" << logical_x << ", " << logical_y << ")\n";
    dotSize = bound(dotSize, 8, 100);

    int A = dotSize * dotSize;
    int half = dotSize / 2;

    //int start_y = bound(logical_y - half, 0, miniMap.getH()-half-1);
    //int start_x = bound(logical_x - half, 0, miniMap.getW()-half-1);

    int start_x = bound(logical_x - half, 0, miniMap.getW()-1);
    int end_x = bound(logical_x + half, 0, miniMap.getW()-1);
    int start_y = bound(logical_y - half, 0, miniMap.getH()-1);
    int end_y = bound(logical_y + half, 0, miniMap.getH()-1);

    for(int y=start_y; y<end_y; y++) {
        for(int x=start_x; x < end_x; x++) {
            miniMap.putPixel(x, y, 255, 0, 0);
        }
    }
}


int progressDisplay::getW(){
    return tausta.haeLeveys_px();
}


int progressDisplay::getH() {
    return tausta.haeKorkeus_px();
}


void progressDisplay::piirraRoskaa(int world_x, int world_y, int r) {
    SDL_Rect srcRect = makeRect( wrap(world_x-r, getW() ),
                                 wrap(world_y-r, getH() ),
                                 r*2, r*2 );
    //cout << "roska: ("<< srcRect.x << ", " << srcRect.y << "), r=" << r <<"\n";

    SDL_Surface* roskaSfc = roska.haeValmisTausta(srcRect, true);//ei saa vapauttaa tätä

    SDL_SetSurfaceBlendMode(roskaSfc, SDL_BLENDMODE_BLEND);

    tausta.piirraKuvaTaustaan(roskaSfc, world_x, world_y);
}


void progressDisplay::kuihdutaTaustaa(int world_x, int world_y, int r) {

    int ri = bound( (r / 25)-1, 0, ray.size()-1);
    cout << "ri=" << ri << "\n";
    roska.addAlpha(world_x-r, world_y-r, ray.haeKuva(ri) );
}


void progressDisplay::kerroPisteet(int pisteet) {
    stringstream ss;
    string newText;

    ss << "Score=";
    ss << (pisteet/1000);
    ss >> newText;

    score.setText(newText);
}


void progressDisplay::setDialog(bool success) {

    string newText;
    if(success) newText = "You are successful!";
    else        newText = "You fail!";

    loadDialog.setText(newText);
}


void progressDisplay::clearDialog() {
    loadDialog.setText("");
}


titleDisplay::titleDisplay() {
    initialize();

    SDL_Surface* loaded = IMG_Load("media/title_bg.jpg");
    bg = SDL_CreateTextureFromSurface(gRenderer, loaded);
    SDL_FreeSurface(loaded);

    title.setText("Progress");
    start.setText("Press ENTER to start");
    demoText.setText("exhibition DEMO");
}


void titleDisplay::show() {
    static SDL_Rect titleRect = makeRect(220, 100, 1000, 200);
    static SDL_Rect demoRect = makeRect(800, 300, 420, 50);
    static SDL_Rect menuRect = makeRect(520, 480, 400, 120);

    SDL_RenderCopy(gRenderer, bg, NULL, NULL);
    SDL_RenderCopy(gRenderer, title.update(), NULL, &titleRect);
    SDL_RenderCopy(gRenderer, demoText.update(), NULL, &demoRect);
    SDL_RenderCopy(gRenderer, start.update(), NULL, &menuRect);

    SDL_RenderPresent(gRenderer);
}


void titleDisplay::enterPressed(bool pressed) {
    if(pressed) {
        start.setText("Loading level...");
    }
    else
        start.setText("Press ENTER to start");
}
/*-------------------------------------------------------------------------*/
