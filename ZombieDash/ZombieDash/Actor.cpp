#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Penelope::doSomething()
{
    //...
    if (getIfDead())
        return;
    if (getIfInf())
    {
        setInfectStat(getInfStatus()+1);
        if (getInfStatus() == 500)
        {
            setDead();
            getWorld()->playSound(SOUND_PLAYER_DIE);
            getWorld()->decLives();
            return;
        }
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_SPACE:
                if (numFlames >= 1)
                {
                    numFlames--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    getWorld()->penFlames();
                }
                //... add flames in front of Penelope...;
                break;
            case KEY_PRESS_TAB:
                if (numLandmines >= 1)
                {
                    getWorld()->addALandmine(getX(), getY());
                    numLandmines--;
                }
                break;
            case KEY_PRESS_ENTER:
                if (numVaccines >= 1)
                {
                    setInfect(false);
                    setInfectStat(0);
                    numVaccines--;
                }
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld()->checkDir(left, this))
                {
                    moveTo(getX()-4, getY());
                }
                //... move Penelope to the left ...;
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->checkDir(right, this))
                {
                    moveTo(getX()+4, getY());
                }
                //... move Penelope to the right ...;
                break;
                // etc…
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld()->checkDir(up, this))
                {
                    moveTo(getX(), getY()+4);
                }
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getWorld()->checkDir(down, this))
                {
                    moveTo(getX(), getY()-4);
                }
                break;
        }
    }
    //...
}

bool Zombie::DS1()
{
    switchParalysis();
    if (getIfDead())
        return false;
    if (getIfParalyzed())
        return false;
    getWorld()->goVomit(getX(), getY(), getDirection(), this);
    return true;
}

void Zombie::DS2()
{
    int dest_x = getX();
    int dest_y = getY();
    switch(getDirection())
    {
        case up:
            dest_x = getX();
            dest_y = getY() + 1;
            break;
        case down:
            dest_x = getX();
            dest_y = getY() - 1;
            break;
        case left:
            dest_x = getX() - 1;
            dest_y = getY();
            break;
        case right:
            dest_x = getX() + 1;
            dest_y = getY();
            break;
    }
    if (getWorld()->checkDir(getDirection(), this))
    {
        moveTo(dest_x, dest_y);
        setMPD(getMPD()-1);
    }
    else setMPD(0);
}

void Citizen::doSomething()
{
    if (getIfDead())
        return;
    if (getIfInf())
        setInfectStat(getInfStatus()+1);
    if (getInfStatus() == 500)
    {
        setDead();
        getWorld()->infectedCitizenProtocol(getX(), getY());
        return;
    }
    switchParalysis();
    if (getIfParalyzed())
        return;
    getWorld()->moveCitizen(getX(), getY(), this);
}

void DumbZombie::doSomething()
{
    if (!DS1())
        return;
    if (getMPD() == 0)
    {
        setMPD(randInt(3, 10));
        int newDir = randInt(1, 4);
        switch(newDir)
        {
            case 1:
                setDirection(up);
                break;
            case 2:
                setDirection(down);
                break;
            case 3:
                setDirection(left);
                break;
            case 4:
                setDirection(right);
                break;
        }
    }
    DS2();
}

void DumbZombie::deathPoints()
{
    getWorld()->increaseScore(1000);
}

void SmartZombie::doSomething()
{
    if(!DS1())
        return;
    //SOMETHING BOUT DIRECTIONS
    getWorld()->smartZDir(getX(), getY(), this);
    DS2();
}

void SmartZombie::deathPoints()
{
    getWorld()->increaseScore(2000);
}

void Exit::doSomething()
{
    getWorld()->exitOverlap(getX(), getY());
    
}

void Pit::doSomething()
{
    getWorld()->pitFlameProtocol(getX(), getY(), this);
    
}

void Flame::doSomething()
{
    if (getIfDead())
    {
        return;
    }
    if (tickCheck == 2)
    {
        setDead();
        tickCheck = 0;
        return;
    }
    
    getWorld()->pitFlameProtocol(getX(), getY(), this);

    tickCheck++;
}

void Vomit::doSomething()
{

    if (getIfDead())
    {
        return;
    }
    if (tickCheck == 2)
    {
        setDead();
        tickCheck = 0;
        return;
    }
    getWorld()->infect(getX(), getY());
    tickCheck++;
    
}

void Landmine::doSomething()
{
    if (getIfDead())
    {
        return;
    }
    if (safetyTicks > 0)
        safetyTicks--;
    if (safetyTicks == 0)
    {
        isActive = true;
        getWorld()->explode(getX(), getY(), getDirection(), this);
        return;
    }
}

void VaccineGoodie::doSomething()
{
    if (getIfDead())
    {
        return;
    }
    if (getWorld()->getVacGoodie(getX(), getY()))
        setDead();
}

void GasCanGoodie::doSomething()
{
    if (getIfDead())
    {
        return;
    }
    if (getWorld()->getGCGoodie(getX(), getY()))
        setDead();
}

void LandmineGoodie::doSomething()
{
    if (getIfDead())
    {
        return;
    }
    if (getWorld()->getLMGoodie(getX(), getY()))
        setDead();
}

