#include "event.h"
#include <SDL2/SDL.h>


userInput::userInput() {
    /*system quit*/
    quit = false;

    /*keyboard*/
    esc = false;
    up = false;
    down = false;
    left = false;
    right = false;
    q = false;
    f = false;
    z = false;
    x = false;
    m = false;
    enter = false;

}

userInput& getInput() {
    static SDL_Event e;
    static userInput ui;

    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT)
            ui.quit = true;

        else if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
            case SDLK_ESCAPE:
                ui.esc = true;
                break;
            case SDLK_UP:
                ui.up = true;
                break;
            case SDLK_DOWN:
                ui.down = true;
                break;
            case SDLK_LEFT:
                ui.left = true;
                break;
            case SDLK_RIGHT:
                ui.right = true;
                break;
            case SDLK_SPACE:
                ui.space = true;
                break;
            case SDLK_RETURN:
                ui.enter = true;
                break;
            case  SDLK_q:
                ui.q = true;
                break;
            case SDLK_f:
                ui.f = true;
                break;
            case SDLK_z:
                ui.z = true;
                break;
            case SDLK_x:
                ui.x = true;
                break;
            case SDLK_m:
                ui.m = true;
                break;

            }
        }

        else if(e.type == SDL_KEYUP) {
            switch(e.key.keysym.sym) {
            case SDLK_ESCAPE:
                ui.esc = false;
                break;
            case SDLK_UP:
                ui.up = false;
                break;
            case SDLK_DOWN:
                ui.down = false;
                break;
            case SDLK_LEFT:
                ui.left = false;
                break;
            case SDLK_RIGHT:
                ui.right = false;
                break;
            case SDLK_SPACE:
                ui.space = false;
                break;
            case SDLK_RETURN:
                ui.enter = false;
                break;
            case SDLK_q:
                ui.q = false;
                break;
            case SDLK_f:
                ui.f = false;
                break;
            case SDLK_z:
                ui.z = false;
                break;
            case SDLK_x:
                ui.x = false;
                break;
            case SDLK_m:
                ui.m = false;
                break;
            }
        }

        Uint32 state = SDL_GetMouseState(&(ui.mouse_x), &(ui.mouse_y) );
        ui.lclick = (state & SDL_BUTTON(SDL_BUTTON_LEFT) );
        ui.rclick = (state & SDL_BUTTON(SDL_BUTTON_RIGHT) );

    }

    return ui;
}
