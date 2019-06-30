#include "world.h"
#include "event.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;


double bound(double value, double min, double max) {
    if(value < min) value=min;
    if(value > max) value=max;
    return value;
}


double wrap(double value, double min, double max) {
    if(max>min) {
        while(value < min) value+= (max-min);
        while(value > max) value-= (max-min);
    }
    else cout << "wrap: bad bounds " << min << ", "<< max <<  "\n";
    return value;
}


int wrap(int value, int max) {
    value %= max;
    if(value<0) value+=max;
    return value;
}


player::player(int init_x, int init_y) :
                x(init_x), y(init_y),
                v_x(0), v_y(0),
                a(1),
                spawnDelay(3),
                m(10000)
{}


world::world(string visualMap, string logMap, string trashfile, vector<string> spawnFiles) :
                            ikkuna(visualMap, logMap, trashfile, spawnFiles),
                            world_w(ikkuna.getW() ),
                            world_h(ikkuna.getH() ),
                            aivo(world_w/2, world_h/2),
                            totalYield(0),
                            time(0)
{
    logicalMap = loadMap(logMap.c_str() );

    worldToLogScale = (double)logicalMap.getW() / world_w;

    cout << "Luotiin taso: " << world_w << ", " << world_h << "\n";
}


world::world(level l) :
                            ikkuna(l.visualMapName, l.logicalMapName, l.trashImageName, l.spawns),
                            world_w(ikkuna.getW() ),
                            world_h(ikkuna.getH() ),
                            aivo(world_w/2, world_h/2),
                            totalYield(0),
                            time(l.levelTime)
{
    logicalMap = loadMap(l.logicalMapName.c_str() );

    worldToLogScale = (double)logicalMap.getW() / world_w;

    cout << "Luotiin taso: " << world_w << ", " << world_h << "\n";
}


 level loadLevel(string levelFile) {

    level newLevel;

    ifstream file;
    file.open (levelFile.c_str() );

    string word;

    while(getline(file, word) ) {
        file >> word;

        if(word == "logical")
            file >> newLevel.logicalMapName;

        else if(word == "visual")
            file >> newLevel.visualMapName;

        else if(word == "trash")
            file >> newLevel.trashImageName;

        else if(word == "time")
            file >> newLevel.levelTime;

        else if(word == "intro")
            file >> newLevel.musicIntro;

        else if(word == "loop")
            file >> newLevel.musicLoop;

        else if(word == "spawns") {
            while(getline(file, word) ) {
                file >> word;
                if(word != "")
                    newLevel.spawns.push_back(word);
            }
        }
    }
    file.close();
    return newLevel;
}


bool world::movePlayer() {

    //cout << "move...\n";

    const int MAX_V = 100;
    const double MIN_A = 0.3;
    const double MAX_A = 10;
    const int MAX_R = 1000;
    const int MIN_R = 40;
    const double MOVE_COST = 0.99;
    const double MIN_SIZE = 40;
    const double MAX_SIZE = 1000;
    const double MIN_ZOOM = 0.6;
    const double MAX_ZOOM = 3;
    const int MIN_M = 100;

    static int spawnDelay = 0;

    if(time > 0) {

        time--;

        int logx = bound(worldToLogical(aivo.x), 0, logicalMap.getW()-1 );
        int logy = bound(worldToLogical(aivo.y), 0, logicalMap.getH()-1 );

        spawnDelay--;

        double e = aivo.m * (abs(aivo.v_x) + abs(aivo.v_y));
        aivo.a = bound( 0.002* sqrt(aivo.m), MIN_A, MAX_A);
        aivo.size = bound(0.4*sqrt(aivo.m) + MIN_SIZE, MIN_SIZE, MAX_SIZE);
        aivo.r = bound(0.08*sqrt(e)+MIN_SIZE, MIN_R, MAX_R);

        mapSquareT s = logicalMap.squareAt(logx, logy);

        // Moving & energy spending

        if(time > 60) {

                        userInput kb = getInput();



                        if(kb.up | kb.down) aivo.m *= MOVE_COST;
                        if(kb.left | kb.right) aivo.m *= MOVE_COST;

                        if(kb.up) aivo.v_y = bound(aivo.v_y - aivo.a, -MAX_V, MAX_V);
                        if(kb.down) aivo.v_y = bound(aivo.v_y + aivo.a, -MAX_V, MAX_V);
                        if(kb.left) aivo.v_x = bound(aivo.v_x - aivo.a, -MAX_V, MAX_V);
                        if(kb.right) aivo.v_x = bound(aivo.v_x + aivo.a, -MAX_V, MAX_V);


                        if(kb.x) { //jarru
                            aivo.v_x *= 0.85;
                            aivo.v_y *= 0.85;
                            aivo.m += (abs(aivo.v_x) + abs(aivo.v_y));
                        }


                        // Building
                        if(kb.z || kb.space) {

                            double buildCost = 0.024*aivo.r*aivo.r;
                            //cout << "build\n";
                            if(spawnDelay <=0 && aivo.m > buildCost) {

                                spawnDelay = bound(5-(5*((abs(aivo.v_x) + abs(aivo.v_y) )) / (2*MAX_V)), 0, 5);

                                logicalMap.recursiveAddSuckersAt(logx, logy, worldToLogical(aivo.r) ) ;

                                //muuta roskan alfaa
                                ikkuna.kuihdutaTaustaa(aivo.x, aivo.y, aivo.r);

                                //piirrä rakennus
                                ikkuna.spawn(aivo.x, aivo.y, aivo.r);

                                aivo.m -= buildCost;
                                aivo.v_x *= 0.85;
                                aivo.v_y *= 0.85;
                            }
                            logicalMap.addBuildingsAt(logx, logy, worldToLogical(aivo.r/3) );
                        }
        }

        aivo.v_x *= bound(1-s.friction, 0, 1);
        aivo.v_y *= bound(1-s.friction, 0, 1);

        aivo.x = wrap(aivo.x + aivo.v_x, 0, world_w);
        aivo.y = wrap(aivo.y + aivo.v_y, 0, world_h);

        // Sucking
        int yield = 0;
        yield += logicalMap.suckAt(logx, logy, aivo.r*5);
        yield += logicalMap.forceSuckAt(logx, logy, worldToLogical((aivo.size/2) ));
        aivo.m += yield;
        totalYield += yield;

        // Set view
        ikkuna.centerView(aivo.x, aivo.y);

        if(time > 60)
            ikkuna.setZoom( bound(aivo.r / 100, MIN_ZOOM, MAX_ZOOM) );
        else
            ikkuna.setZoom(5);

        return true;
    }

    //else time <= 0
    else
        return false;
}


void world::draw() {

    static bool logicalMode = false;
    static int spawnDelay = 0;
    //cout << "draw...\n";

    ikkuna.kerroPisteet(totalYield);

    if(time < 30)
        ikkuna.setDialog(isCleared() );

    userInput kb = getInput();

    ikkuna.piirraRoskaa(aivo.x, aivo.y, aivo.r);
    ikkuna.piirraTausta();
    ikkuna.drawMap(logicalMap);

    ikkuna.drawDot( worldToLogical(aivo.x), worldToLogical(aivo.y), worldToLogical(aivo.size) );

    spawnDelay--;

    if( (kb.z||kb.space) && (spawnDelay <=0) ) {
        spawnDelay = 4;
//        ikkuna.spawn(aivo.x, aivo.y, aivo.r);
    }

    ikkuna.piirraAivo(aivo.size);

    int cx = worldToLogical(aivo.x) - logicalMap.getW()/2 ;
    int cy = worldToLogical(aivo.y) - logicalMap.getH()/2 ;

    ikkuna.show(cx, cy);
    //cout << " ok\n\n";
}

double world::worldToLogical(double world_c) {
    return world_c * worldToLogScale;
}


double world::logicalToWorld(double log_c) {
    return log_c / worldToLogScale;
}


bool world::isCleared() {
    if(totalYield > 3000000)
        return true;
    else return false;
}


void world::setTime(int t){
    time = t;
}

