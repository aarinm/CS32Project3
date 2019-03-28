#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool checkDir(Direction dir, Actor* act);
    void exitOverlap(int x, int y);
    void pitFlameProtocol(int x, int y, Actor *act);
    void infect(int x, int y);
    void infectedCitizenProtocol(int x, int y);
    void moveCitizen(int x, int y, Citizen* c);
    void landmineFlameProtocol(int x, int y, Direction dir);
    bool getVacGoodie(int x, int y);
    bool getGCGoodie(int x, int y);
    bool getLMGoodie(int x, int y);
    void explode(int x, int y, Direction dir, Landmine* act);
    void addALandmine(int x, int y);
    void smartZDir(int x, int y, SmartZombie* sz);
    void penFlames();
    void goVomit(int x, int y, Direction dir, Zombie* z);
    virtual ~StudentWorld();
private:
    std::list<Actor*> gameObjects;
    Penelope* m_penelope;
    bool canExit;
    bool checkAllCDead();
    bool overlap(int x1, int x2, int y1, int y2);
    Actor* closeZombie(int x, int y, int& dist_z, Citizen* c, Actor* currClose, bool& areThereZombies);
    void deadCitizenProtocol(Actor* act);
    void deadPenelopeProtocol();
    void deadZombieProtocol(Actor* act);
    bool goodieOverlap(int x, int y);
};

#endif // STUDENTWORLD_H_
