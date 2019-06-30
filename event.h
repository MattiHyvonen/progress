#ifndef EVENT_H
#define EVENT_H

struct userInput{
public:

    userInput();
    /*system quit*/
    bool quit;

    /*keyboard*/
    bool esc;
    bool up;
    bool down;
    bool left;
    bool right;
    bool space;
    bool enter;
    bool q;
    bool f;
    bool z;
    bool x;
    bool m;

    /*mouse*/
    int mouse_x;
    int mouse_y;
    bool lclick;
    bool rclick;
};

userInput& getInput();

#endif
