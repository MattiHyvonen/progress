#ifndef MAP_H
#define MAP_H

#include <vector>


struct mapSquareT{
    double resources;
    double suckers;
    double friction; // <= 1, used as factor of speed
    bool buildings;
    bool isUpdated;

    mapSquareT();
};

double suckFromSquare(mapSquareT&);
double forceSuckFromSquare(mapSquareT&);

class map{
protected:
    std::vector<mapSquareT> squares;
    int mapW;
    int mapH;
    int mapSize;
    bool buildings;

public:
    int getW();
    int getH();
    int getSize();

    mapSquareT& squareAt(int x, int y);
    mapSquareT& squareAt(int i);
    void resize(int w, int h);
    map(int w, int h);
    map(){}
    void randomize();
    double suckAt(int x, int y, int r); //suck from a square of given diameter centered at (x,y)
    double forceSuckAt(int x, int y, int r);  //suck regardless of suckers in map
    void addSuckersAt(int x, int y, int r); //add a bunch of suckers
    void recursiveAddSuckersAt(int x, int y, int r); //add a bunch of suckers
    void addBuildingsAt(int x, int y, int r);
};

map loadMap(const char* filename);

#endif
