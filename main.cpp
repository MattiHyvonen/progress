#include "world.h"
#include "display.h"
#include "event.h"
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
using namespace std;


int main(){

    enum gameModeT{
        MENU,
        GAME
    };

    vector<level> levels;

    levels.push_back(loadLevel("levels/level1.lev") );
    levels.push_back(loadLevel("levels/level2.lev") );
    levels.push_back(loadLevel("levels/level3.lev") );
    levels.push_back(loadLevel("levels/level4.lev") );
    levels.push_back(loadLevel("levels/level5.lev") );
    levels.push_back(loadLevel("levels/level6.lev") );

    if(Mix_OpenAudio( 48000, MIX_DEFAULT_FORMAT, 2, 4096 )<0)
        cout << "audio problem" << SDL_GetError()<<"\n";

    bool runGame = true;
    bool runLevel= false;
    bool runMenu = true;
    bool fullscreen = true;
    gameModeT mode = MENU;
    bool skipLevel = false;

    Uint32 lastFrameTime = 0;

    userInput kb;

/*    Mix_Music* music = Mix_LoadMUS( "media/level1_intro.mp3" );
    if(music==NULL) cout << "ei voitu ladata biisiä: " << SDL_GetError()<<"\n";
*/
    while(runGame) {

        if(mode == MENU) {
            runMenu = true;
            //main menu
            titleDisplay title;
            setFullscreen(fullscreen);
            title.show();

            SDL_Delay(200);

            while(runMenu) {
                kb = getInput();
                if(kb.enter) {
                    title.enterPressed(true);
                    mode = GAME;
                    runMenu = false;
                    runLevel = true;
                }
                if(kb.esc) {
                    runGame = false;
                    runMenu = false;
                }

                title.show();
            }
        }

        if(mode == GAME) {

            Mix_Music* intro = NULL;
            Mix_Music* biisi = NULL;
            string introStr;
            string biisiStr;
            string nowPlaying;

            //setFullscreen(fullscreen);
            //actual game
            for (int level_i = 0; (level_i < levels.size()) && runGame && mode == GAME; level_i++) {

                introStr = levels[level_i].musicIntro;
                biisiStr = levels[level_i].musicLoop;

                world taso(levels[level_i]);

                /*lataa intro paitsi jos se soi nyt*/
                if(introStr != nowPlaying) {
                    Mix_FreeMusic(intro);
                    intro = Mix_LoadMUS(introStr.c_str() );
                    if(intro==NULL) cout << "ei voitu ladata introa: " << SDL_GetError()<<"\n";
                }

                if(biisiStr != nowPlaying) {
                    Mix_FreeMusic(biisi);
                    biisi = Mix_LoadMUS(biisiStr.c_str() );
                    if(biisi==NULL) cout << "ei voitu ladata biisiä: " << SDL_GetError()<<"\n";
                }

                //taso.setTime(levels[level_i].levelTime);
                runLevel = true;

                //soita intro paitsi jos se soi jo
                if(nowPlaying != introStr) {
                    Mix_PlayMusic(intro, 0);
                    nowPlaying = introStr;
                }

                while(runLevel) {

                    if(Mix_PlayingMusic() == 0) {
                        Mix_PlayMusic(biisi, -1);
                        nowPlaying = biisiStr;
                    }

                    runLevel = taso.movePlayer();

                    kb = getInput();
                    if(kb.esc) {
                        taso.setTime(30);
                        mode = MENU;
                        Mix_FadeOutMusic(2500);
                    }

                    /*if(kb.q) {
                        taso.setTime(30);
                        skipLevel = true;
                    }*/
                    /*
                    if(kb.f) {
                        fullscreen = !fullscreen;
                        setFullscreen(fullscreen);
                        SDL_Delay(200);
                    }*/

                    taso.draw();

                    //cout << "frame time: " << (SDL_GetTicks() - lastFrameTime) << "\n";
                    //int delayTime = bound(20 - (lastFrameTime-SDL_GetTicks()), 0, 20);
                    //SDL_Delay(delayTime);

                    lastFrameTime = SDL_GetTicks();
                }

                if(taso.isCleared() || skipLevel)
                    cout << "next level...\n";
                else {
                    mode = MENU;
                    runMenu = true;
                    cout << "You lose!\n";
                }
            }
        Mix_FreeMusic(biisi);
        Mix_FreeMusic(intro);
        mode = MENU;
        runMenu = true;
        }
    }

    Mix_CloseAudio();

    SDL_Quit();
}
