#ifndef WORLD_H
#define WORLD_H

#include "display.h"
#include "map.h"

#include <vector>

class smooth{
protected:
    int maxValues;
    std::vector<double> values;
    double sum;
    double result;

public:
    smooth(int maxSize) : maxValues(maxSize) {}
    smooth() : maxValues(50) {}

    double get() {return result;}

    void add(double newValue) {
        //add to end, erase from beginning
        values.push_back(newValue);

        if(values.size() > maxValues)
            values.erase(values.begin());

        //calculate sum
        sum=0;
        for(int i=0; i<values.size(); i++)
            sum += values[i];

        //update result
        result = sum / values.size();
    }
};


double bound(double value, double min, double max);
double wrap(double value, double min, double max);
int wrap(int value, int max);


struct player{
    double x;
    double y;
    double v_x;
    double v_y;

    double a;
    double r;

    int spawnDelay;

    double m;
    int size;

    player(int init_x, int init_y);
};


struct level{
    std::string logicalMapName;
    std::string visualMapName;
    std::string trashImageName;

    std::vector<std::string> spawns;

    std::string musicIntro;
    std::string musicLoop;

    int levelTime;

};


level loadLevel(std::string filename);


class world{
protected:
    map logicalMap;

    progressDisplay ikkuna;

    double worldToLogScale;

    int world_w;
    int world_h;

    player aivo;
    double totalYield;
    int time;

public:
    world(std::string visualMap, std::string logMap, std::string trashfile, std::vector<std::string> spawnFiles);
    world(level);

    double worldToLogical(double world_c);
    double logicalToWorld(double log_c);

    bool movePlayer(); //palauttaa false jos aika on loppunut
    void draw();

    bool isCleared();
    void setTime(int);

};



#endif
