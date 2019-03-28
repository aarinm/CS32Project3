#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
using namespace std;
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* sw)
    :GraphObject(imageID, startX, startY, dir, depth)
    {
        m_studentWorld = sw;
        isDead = false;
    }
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const { return m_studentWorld; }
    bool getIfDead() const {return isDead;}
    void setDead() {isDead = true;}
    virtual bool canBeInfected() const {return false;}
    virtual bool canBeDamaged() const {return true;}
    virtual bool canBlock() const {return false;}
    virtual bool canBlockFlames() const {return false;}
    virtual bool canBePickedUp() const {return false;}
    virtual void setInfect(bool b) {}
    virtual void deathPoints() {};
    virtual ~Actor() {}
private:
    StudentWorld* m_studentWorld;
    bool isDead;
    int iid;
};

class Agent: public Actor
{
public:
    Agent(int imageID, double someX, double someY, StudentWorld* sw)
    :Actor(imageID, someX, someY, right, 0, sw)
    {
        isParalyzed = true;
    }
    virtual void doSomething() = 0;
    virtual bool getIfParalyzed() {return isParalyzed;}
    virtual void switchParalysis() {isParalyzed = !isParalyzed;}
    virtual bool canBlock() const {return true;}
    virtual ~Agent(){}
private:
    bool isParalyzed;
};

class Zombie: public Agent
{
public:
    Zombie(double someX, double someY, StudentWorld* sw)
    :Agent(IID_ZOMBIE, someX, someY, sw)
    {
        movementPlanDistance = 0;
    }
    virtual void doSomething() = 0;
    virtual int getMPD() {return movementPlanDistance;}
    virtual void setMPD (int mpd) {movementPlanDistance = mpd;}
protected:
    virtual bool DS1();
    virtual void DS2();
private:
    int movementPlanDistance;
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(double someX, double someY, StudentWorld* sw)
    :Zombie(someX, someY, sw)
    {}
    virtual void doSomething();
    virtual void deathPoints();
};

class SmartZombie: public Zombie
{
public:
    SmartZombie(double someX, double someY, StudentWorld* sw)
    :Zombie(someX, someY, sw)
    {}
    virtual void doSomething();
    virtual void deathPoints();
};

class Human: public Agent
{
public:
    Human(int imageID, double someX, double someY, StudentWorld* sw)
    :Agent(imageID, someX, someY, sw)
    {
        infectionStatus = 0;
        isInfected = false;
    }
    virtual void doSomething() = 0;
    virtual bool canBeInfected() const {return true;}
    virtual void setInfect(bool b) {isInfected = b;}
    virtual bool getIfInf() const  {return isInfected;}
    virtual void setInfectStat(int num) {infectionStatus = num;}
    virtual int getInfStatus() {return infectionStatus;}
    virtual ~Human(){}
private:
    bool isInfected;
    int infectionStatus;
    
};

class Penelope: public Human
{
public:
    Penelope(double someX, double someY, StudentWorld* sw)
    :Human(IID_PLAYER, someX, someY, sw)
    {
        numLandmines = 0;
        numFlames = 0;
        numVaccines = 0;
    }
    virtual void doSomething();
    virtual ~Penelope() {}
    void incVaccines() {numVaccines++;}
    void incFlames() {numFlames += 5;}
    void incLandmines() {numLandmines += 2;}
    int getVaccines() const {return numVaccines;}
    int getFlames() const {return numFlames;}
    int getLandmines() const {return numLandmines;}
private:
    int numLandmines;
    int numFlames;
    int numVaccines;
};

class Citizen: public Human
{
public:
    Citizen(double someX, double someY, StudentWorld *sw)
    :Human(IID_CITIZEN, someX, someY, sw)
    {}
    virtual bool canBeInfected() const {return true;}
    virtual void doSomething();
};

class ActivatingObject: public Actor
{
public:
    ActivatingObject(int imageID, double someX, double someY, Direction dir, int depth, StudentWorld* sw)
    :Actor(imageID, someX, someY, dir, depth, sw)
    {}
    virtual void doSomething() = 0;
    virtual bool canBeDamaged() const {return false;}
    virtual ~ActivatingObject() {}
};

class Wall: public ActivatingObject
{
public:
    Wall(double someX, double someY, StudentWorld* sw)
    :ActivatingObject(IID_WALL, someX, someY, right, 0, sw)
    {}
    virtual void doSomething(){}
    virtual bool canBeDamaged() const {return false;}
    virtual bool canBlock() const {return true;}
    virtual bool canBlockFlames() const {return true;}
    virtual ~Wall() {}
};

class Exit: public ActivatingObject
{
public:
    Exit(double someX, double someY, StudentWorld* sw)
    :ActivatingObject(IID_EXIT, someX, someY, right, 1, sw)
    {}
    virtual void doSomething();
    virtual bool canBlockFlames() const {return true;}
    virtual ~Exit(){};
};

//REALLY DUMB REUSED BOOLS BC MY CODE DOESNT WORK WITHOUT THEM
class Pit: public ActivatingObject
{
public:
    Pit(double someX, double someY, StudentWorld* sw)
    :ActivatingObject(IID_PIT, someX, someY, right, 0, sw)
    {}
    virtual bool canBeDamaged() const {return false;}
    virtual bool canBePickedUp() const {return false;}
    virtual void doSomething();
    virtual ~Pit(){};
};

//REALLY DUMB REUSED BOOLS BC MY CODE DOESNT WORK WITHOUT THEM
class Flame: public ActivatingObject
{
public:
    Flame(double someX, double someY, Direction dir, StudentWorld* sw)
    :ActivatingObject(IID_FLAME, someX, someY, dir, 0, sw)
    {
        tickCheck = 0;
        cout << "creating flame" << endl;
    }
    virtual void doSomething();
    virtual bool canBeDamaged() const {return false;}
    virtual bool canBePickedUp() const {return false;}
    virtual ~Flame(){};
private:
    int tickCheck;
};

class Vomit: public ActivatingObject
{
public:
    Vomit(double someX, double someY, Direction dir, StudentWorld* sw)
    :ActivatingObject(IID_VOMIT, someX, someY, dir, 0, sw)
    {
        tickCheck = 0;
    }
    virtual void doSomething();
private:
    int tickCheck;
};

class Landmine: public ActivatingObject
{
public:
    Landmine(double someX, double someY, StudentWorld* sw)
    :ActivatingObject(IID_LANDMINE, someX, someY, right, 1, sw)
    {
        isActive = false;
        safetyTicks = 30;
    }
    virtual void doSomething();
    virtual void setActive() {isActive = true;}
    virtual bool canBeDamaged() const {return true;}
private:
    bool isActive;
    int safetyTicks;
};

class Goodie: public Actor
{
public:
    Goodie(int imageID, double someX, double someY, StudentWorld* sw)
    :Actor(imageID, someX, someY, right, 1, sw)
    {}
    virtual void doSomething() = 0;
    virtual bool canBePickedUp() const {return true;}
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(double someX, double someY, StudentWorld* sw)
    :Goodie(IID_VACCINE_GOODIE, someX, someY, sw)
    {}
    virtual void doSomething();
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(double someX, double someY, StudentWorld* sw)
    :Goodie(IID_GAS_CAN_GOODIE, someX, someY, sw)
    {}
    virtual void doSomething();
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(double someX, double someY, StudentWorld* sw)
    :Goodie(IID_LANDMINE_GOODIE, someX, someY, sw)
    {}
    virtual void doSomething();
};


#endif // ACTOR_H_
