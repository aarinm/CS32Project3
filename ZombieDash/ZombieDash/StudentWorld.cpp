#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_penelope = nullptr;
    canExit = false;
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
    {
        cerr << "Cannot find " << oss.str() << " data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    if (getLevel() >= 99)
    {
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                Level::MazeEntry ge = lev.getContentsOf(i,j);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        gameObjects.push_back(new SmartZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::dumb_zombie:
                        gameObjects.push_back(new DumbZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::player:
                        m_penelope = new Penelope(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this);
                        break;
                    case Level::exit:
                        gameObjects.push_back(new Exit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::wall:
                        gameObjects.push_back(new Wall(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::pit:
                        gameObjects.push_back(new Pit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::citizen:
                        gameObjects.push_back(new Citizen(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::vaccine_goodie:
                        gameObjects.push_back(new VaccineGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::gas_can_goodie:
                        gameObjects.push_back(new GasCanGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::landmine_goodie:
                        gameObjects.push_back(new LandmineGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        (*it)->doSomething();
        it++;
    }
    m_penelope->doSomething();
    if ((m_penelope->getIfDead()))
    {
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    if (canExit)
    {
        canExit = false;
        playSound(SOUND_LEVEL_FINISHED);
        while (getLives() < 3)
        {
            incLives();
        }
        return GWSTATUS_FINISHED_LEVEL;
    }
    it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if ((*it)->getIfDead())
        {
            delete (*it);
            it = gameObjects.erase(it);
        }
        it++;
    }
    
    ostringstream level;
    level << getLevel();
    ostringstream lives;
    lives << getLives();
    ostringstream vaccines;
    vaccines << m_penelope->getVaccines();
    string statVaccines = vaccines.str();
    ostringstream flames;
    flames << m_penelope->getFlames();
    string statFlames = flames.str();
    ostringstream landmines;
    landmines << m_penelope->getLandmines();
    ostringstream infection;
    infection << m_penelope->getInfStatus();
    string statInfection = infection.str();
    string statLandmines = landmines.str();
    string statLives = lives.str();
    string statLevel = level.str();
    ostringstream score;
    score << setw(6);
    score.fill('0');
    string status;
    if (getScore() < 0)
    {
        score << -getScore();
        string statscore = score.str();
        status = "Score: -" + statscore + "  Level: " + statLevel + "  Lives: " + statLives + "  Vaccines: " + statVaccines + "  Flames: " + statFlames + "  Landmines: " + statLandmines + "  Infected: " + statInfection + "\n";
    }
    else
    {
        score << getScore();
        string statscore = score.str();
        status = "Score: " + statscore + "  Level: " + statLevel + "  Lives: " + statLives + "  Vaccines: " + statVaccines + "  Flames: " + statFlames + "  Landmines: " + statLandmines + "  Infected: " + statInfection + "\n";
    }
    setGameStatText(status);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_penelope != nullptr)
    {
        delete m_penelope;
        m_penelope = nullptr;
    }
    list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        delete (*it);
        it = gameObjects.erase(it);
    }
}

bool StudentWorld::checkDir(Direction dir, Actor* act)
{
    std::list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
     {
         if (act != *it)
         {
             if ((*it)->canBlock())
             {
                 switch(dir)
                 {
                     case 180:
                         if (((*it)->getX()+SPRITE_WIDTH == act->getX()) && ((*it)->getY() <= act->getY() + SPRITE_HEIGHT-1) && (act->getY() <= ((*it)->getY()+SPRITE_HEIGHT-1)))
                             return false;
                         break;
                     case 0:
                         if (((*it)->getX() == act->getX()+SPRITE_WIDTH) && ((*it)->getY() <= act->getY()+SPRITE_HEIGHT-1) && (act->getY() <= ((*it)->getY()+SPRITE_HEIGHT-1)))
                             return false;
                         break;
                     case 90:
                         if (((*it)->getY() == act->getY()+SPRITE_HEIGHT) && ((*it)->getX() <= act->getX()+SPRITE_WIDTH-1) && (act->getX() <= ((*it)->getX()+SPRITE_WIDTH-1)))
                             return false;
                         break;
                     case 270:
                         if (((*it)->getY()+SPRITE_HEIGHT == act->getY()) && (((*it)->getX() <= act->getX() + SPRITE_WIDTH-1) && (act->getX() <= ((*it)->getX()+SPRITE_WIDTH-1))))
                             return false;
                         break;
                 }
             }
         }
         it++;
     }
    return true;
}

bool StudentWorld::overlap(int x1, int x2, int y1, int y2)
{
    int euDistX = (x2-x1)*(x2-x1);
    int euDistY = (y2-y1)*(y2-y1);
    if ((euDistX + euDistY) <= 100)
        return true;
    return false;
}

void StudentWorld::exitOverlap(int x, int y)
{
    std::list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if ((*it)->canBeInfected())
        {
            if (overlap(x, (*it)->getX(), y, (*it)->getY()))
            {
                increaseScore(500);
                (*it)->setDead();
                playSound(SOUND_CITIZEN_SAVED);
            }
        }
        it++;
    }
    if (checkAllCDead())
    {
        if (overlap(x, m_penelope->getX(), y, m_penelope->getY()))
        {
            canExit = true;
        }
    }
}


void StudentWorld::smartZDir(int x, int y, SmartZombie* sz)
{
    if (sz->getMPD() == 0)
    {
        randInt(3, 10);
    }
    Actor* currClose = nullptr;
    int smallDist = 100000;
    for (list<Actor*>::iterator it = gameObjects.begin(); it != gameObjects.end(); it++)
    {
        if ((*it)->canBeInfected())
        {
            int distSqX = (x - ((*it)->getX()));
            int distSqY = (y - ((*it)->getY()));
            int distSq = (distSqX)*(distSqX) + (distSqY)*(distSqY);
            if (distSq < smallDist)
            {
                smallDist = distSq;
                currClose = (*it);
            }
        }
    }
    int distSqX = (x - (m_penelope->getX()));
    int distSqY = (y - (m_penelope->getY()));
    int distSq = (distSqX)*(distSqX) + (distSqY)*(distSqY);
    if (distSq < smallDist)
    {
        smallDist = distSq;
        currClose = m_penelope;
    }
    if (distSq > (80*80))
    {
        int num = randInt(1, 4);
        switch(num)
        {
            case 1:
                sz->setDirection(0);
                break;
            case 2:
                sz->setDirection(90);
                break;
            case 3:
                sz->setDirection(180);
                break;
            case 4:
                sz->setDirection(270);
                break;
        }
    }
    else
    {
        if (x == currClose->getX())
        {
            if (y > currClose->getY())
                sz->setDirection(270);
            else sz->setDirection(90);
        }
        else if (y == currClose->getY())
        {
            if (x > currClose->getX())
                sz->setDirection(180);
            else sz->setDirection(0);
        }
        else
        {
            int randClose = randInt(1, 2);
            if (randClose == 1)
            {
                if (y > currClose->getY())
                    sz->setDirection(270);
                else sz->setDirection(90);
            }
            else
            {
                if (x > currClose->getX())
                    sz->setDirection(180);
                else sz->setDirection(0);
            }
        }
    }
    
}



void StudentWorld::pitFlameProtocol(int x, int y, Actor *act)
{
    
    std::list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if (act != (*it))
        {
            if (overlap(x, (*it)->getX(), y, (*it)->getY()))
            {
                if ((*it)->canBeInfected()) // if it's a citizen
                {
                    deadCitizenProtocol((*it));
                }
                else if ((*it)->canBeDamaged() && ((*it)->canBlock())) // if it's a zombie
                {
                    deadZombieProtocol((*it));
                    int drop = randInt(1, 10);
                    int direct = randInt(1, 4);
                    if (drop == 10)
                    {
                        bool toDrop = true;
                        switch (direct)
                        {
                            case 1:
                                for (list<Actor*>::iterator it2 = gameObjects.begin(); it2 != gameObjects.end(); it2++)
                                {
                                    if (overlap((*it2)->getX(), (*it)->getX() + SPRITE_WIDTH, (*it2)->getY(), (*it)->getY()))
                                        toDrop = false;
                                }
                                if (toDrop)
                                {
                                    gameObjects.push_back(new VaccineGoodie((*it)->getX() + SPRITE_WIDTH,(*it)->getY(), this));
                                }
                                break;
                            case 2:
                                for (list<Actor*>::iterator it2 = gameObjects.begin(); it2 != gameObjects.end(); it2++)
                                {
                                    if (overlap((*it2)->getX(), (*it)->getX() - SPRITE_WIDTH, (*it2)->getY(), (*it)->getY()))
                                        toDrop = false;
                                }
                                if (toDrop)
                                {
                                    gameObjects.push_back(new VaccineGoodie((*it)->getX() - SPRITE_WIDTH,(*it)->getY(), this));
                                }
                                break;
                            case 3:
                                for (list<Actor*>::iterator it2 = gameObjects.begin(); it2 != gameObjects.end(); it2++)
                                {
                                    if (overlap((*it2)->getX(), (*it)->getX(), (*it2)->getY(), (*it)->getY()+SPRITE_HEIGHT))
                                        toDrop = false;
                                }
                                if (toDrop)
                                {
                                    gameObjects.push_back(new VaccineGoodie((*it)->getX() - SPRITE_WIDTH,(*it)->getY()+SPRITE_HEIGHT, this));
                                }
                                break;
                            case 4:
                                for (list<Actor*>::iterator it2 = gameObjects.begin(); it2 != gameObjects.end(); it2++)
                                {
                                    if (overlap((*it2)->getX(), (*it)->getX(), (*it2)->getY(), (*it)->getY()-SPRITE_HEIGHT))
                                        toDrop = false;
                                }
                                if (toDrop)
                                {
                                    gameObjects.push_back(new VaccineGoodie((*it)->getX() - SPRITE_WIDTH,(*it)->getY()-SPRITE_HEIGHT, this));
                                }
                                break;
                        }
                    }
                }
                else if ((*it)->canBeDamaged() && (((*it)->canBePickedUp())==false)) //if is a landmine
                {
                    if (!(*it)->getIfDead())
                    {
                        landmineFlameProtocol((*it)->getX(), (*it)->getY(), (*it)->getDirection());
                    }
                }
                else if ((*it)->canBeDamaged() && ((*it)->canBePickedUp())) //if is a goodie
                {
                    (*it)->setDead();
                }
            }
        }
        it++;
    }
    
    if (overlap(x, m_penelope->getX(), y, m_penelope->getY()))
    {
        deadPenelopeProtocol();
    }
    
}

void StudentWorld::infectedCitizenProtocol(int x, int y)
{
    playSound(SOUND_ZOMBIE_BORN);
    increaseScore(-1000);
    int makeZombie = randInt(1, 100);
    if (makeZombie <= 30)
    {
        gameObjects.push_back(new SmartZombie(x, y, this));
    }
    else
    {
        gameObjects.push_back(new DumbZombie(x, y, this));
    }
}

Actor* StudentWorld::closeZombie(int x, int y, int &dist_z, Citizen *c, Actor* currClose, bool &areThereZombies)
{
    dist_z = 100000;
    for (list<Actor*>::iterator it = gameObjects.begin(); it != gameObjects.end(); it++)
    {
        if ((*it) != c)
        {
            if ((*it)->canBlock() && (*it)->canBeDamaged() && !(*it)->canBeInfected())
            {
                areThereZombies = true;
                int distSqX = (x - ((*it)->getX()));
                int distSqY = (y - ((*it)->getY()));
                int distSq = (distSqX)*(distSqX) + (distSqY)*(distSqY);
                if (distSq < dist_z)
                {
                    dist_z = distSq;
                    currClose = (*it);
                }
            }
        }
    }
    return currClose;
}

void StudentWorld::moveCitizen(int x, int y, Citizen *c)
{
    int distXSq = (x-m_penelope->getX())*(x-m_penelope->getX());
    int distYSq = (y-m_penelope->getY())*(y-m_penelope->getY());
    int dist_p = distXSq + distYSq;
    Actor* currClose = nullptr;
    int dist_z = 100000;
    bool areThereZombies = false;
    currClose = closeZombie(x, y, dist_z, c, currClose, areThereZombies);
    if (((dist_p < dist_z) || !areThereZombies) && dist_p <= (80*80))
    {
        if (x == m_penelope->getX())
        {
            if (y > m_penelope->getY())
            {
                if (checkDir(270, c))
                {
                    c->setDirection(270);
                    c->moveTo(x, y-2);
                    return;
                }
            }
            else
            {
                if (checkDir(90, c))
                {
                    c->setDirection(90);
                    c->moveTo(x, y+2);
                    return;
                }
            }
        }
        else if (y == m_penelope->getY())
        {
            if (x > m_penelope->getX())
            {
                if (checkDir(180, c))
                {
                    c->setDirection(180);
                    c->moveTo(x-2, y);
                    return;
                }
            }
            else
            {
                if (checkDir(0, c))
                {
                    c->setDirection(0);
                    c->moveTo(x+2, y);
                    return;
                }
            }
        }
        else
        {
            int randClose = randInt(1, 2);
            if (randClose == 1)
            {
                if (y > m_penelope->getY())
                {
                    if (checkDir(270, c))
                    {
                        c->setDirection(270);
                        c->moveTo(x, y-2);
                        return;
                    }
                    else if (x >= m_penelope->getX())
                    {
                        if (checkDir(180, c))
                        {
                            c->setDirection(180);
                            c->moveTo(x-2, y);
                            return;
                        }
                    }
                    else if (x < m_penelope->getX())
                    {
                        if (checkDir(0, c))
                        {
                            c->setDirection(0);
                            c->moveTo(x+2, y);
                            return;
                        }
                    }
                }
                else
                {
                    if (checkDir(90, c))
                    {
                        c->setDirection(90);
                        c->moveTo(x, y+2);
                        return;
                    }
                    else if (x >= m_penelope->getX())
                    {
                        if (checkDir(180, c))
                        {
                            c->setDirection(180);
                            c->moveTo(x-2, y);
                            return;
                        }
                    }
                    else if (x < m_penelope->getX())
                    {
                        if (checkDir(0, c))
                        {
                            c->setDirection(0);
                            c->moveTo(x+2, y);
                            return;
                        }
                    }
                }
            }
            else
            {
                if (x > m_penelope->getX())
                {
                    if (checkDir(180, c))
                    {
                        c->setDirection(180);
                        c->moveTo(x-2, y);
                        return;
                    }
                    else if (y >= m_penelope->getX())
                    {
                        if (checkDir(270, c))
                        {
                            c->setDirection(270);
                            c->moveTo(x, y-2);
                            return;
                        }
                    }
                    else if (y < m_penelope->getX())
                    {
                        if (checkDir(90, c))
                        {
                            c->setDirection(90);
                            c->moveTo(x, y+2);
                            return;
                        }
                    }
                }
                else
                {
                    if (checkDir(0, c))
                    {
                        c->setDirection(0);
                        c->moveTo(x+2, y);
                        return;
                    }
                    else if (y >= m_penelope->getX())
                    {
                        if (checkDir(270, c))
                        {
                            c->setDirection(270);
                            c->moveTo(x, y-2);
                            return;
                        }
                    }
                    else if (y < m_penelope->getX())
                    {
                        if (checkDir(90, c))
                        {
                            c->setDirection(90);
                            c->moveTo(x, y+2);
                            return;
                        }
                    }
                }
            }
        }
    }
    if (dist_z <= (80*80) && currClose != nullptr)
    {
        int newDist_z = 100000;
        Actor* newCurrClose = nullptr;
        if (checkDir(0, c))
        {
            newCurrClose = closeZombie(x+2, y, newDist_z, c, newCurrClose, areThereZombies);
        }
        if (checkDir(90, c))
        {
            newCurrClose = closeZombie(x, y+2, newDist_z, c, newCurrClose, areThereZombies);
        }
        if (checkDir(180, c))
        {
            newCurrClose = closeZombie(x-2, y, newDist_z, c, newCurrClose, areThereZombies);
        }
        if (checkDir(270, c))
        {
            newCurrClose = closeZombie(x, y-2, newDist_z, c, newCurrClose, areThereZombies);
        }
        if (newDist_z >= dist_z)
        {
            return;
        }
        int goX = (x - newCurrClose->getX());
        if (goX < 0)
            goX = -goX;
        int goY = (y - newCurrClose->getY());
        if (goY < 0)
            goY = -goY;
        if (goX >= goY) //moving horizontal
        {
            if (x <= newCurrClose->getX())
            {
                if (checkDir(180, c))
                {
                    c->setDirection(180);
                    c->moveTo(x-2, y);
                }
                else if (y <= newCurrClose->getY())
                {
                    if (checkDir(270, c))
                    {
                        c->setDirection(270);
                        c->moveTo(x, y-2);
                    }
                }
                else if (y > newCurrClose->getY())
                {
                    if (checkDir(90, c))
                    {
                        c->setDirection(90);
                        c->moveTo(x,y+2);
                    }
                }
            }
            else
            {
                if (checkDir(0, c))
                {
                    c->setDirection(0);
                    c->moveTo(x+2,y);
                }
                else if (y <= newCurrClose->getY())
                {
                    if (checkDir(270, c))
                    {
                        c->setDirection(270);
                        c->moveTo(x, y-2);
                    }
                }
                else if (y > newCurrClose->getY())
                {
                    if (checkDir(90, c))
                    {
                        c->setDirection(90);
                        c->moveTo(x,y+2);
                    }
                }
            }
        }
        else
        {
            if (y <= newCurrClose->getY())
            {
                if (checkDir(270, c))
                {
                    c->setDirection(270);
                    c->moveTo(x, y-2);
                }
                else if (x <= newCurrClose->getX())
                {
                    if (checkDir(0, c))
                    {
                        c->setDirection(0);
                        c->moveTo(x+2,y);
                    }
                }
                else if (x > newCurrClose->getX())
                {
                    if (checkDir(90, c))
                    {
                        c->setDirection(90);
                        c->moveTo(x,y+2);
                    }
                }
            }
            else
            {
                if (checkDir(90, c))
                {
                    c->setDirection(90);
                    c->moveTo(x,y+2);
                }
                else if (x <= newCurrClose->getX())
                {
                    if (checkDir(0, c))
                    {
                        c->setDirection(0);
                        c->moveTo(x+2,y);
                    }
                }
                else if (x > newCurrClose->getX())
                {
                    if (checkDir(90, c))
                    {
                        c->setDirection(90);
                        c->moveTo(x,y+2);
                    }
                }
            }
        }
    }
}

void StudentWorld::infect(int x, int y)
{
    if (overlap(x, m_penelope->getX(), y, m_penelope->getY()))
    {
        m_penelope->setInfect(true);
    }
    std::list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if ((*it)->canBeInfected())
        {
            if (overlap(x, (*it)->getX(), y, (*it)->getY()))
            {
                (*it)->setInfect(true);
            }
        }
        it++;
    }
            
}


void StudentWorld::goVomit(int x, int y, Direction dir, Zombie* z)
{
    int vomX = 0;
    int vomY = 0;
    switch(dir)
    {
        case 0:
            vomX = x+SPRITE_WIDTH;
            vomY = y;
            break;
        case 90:
            vomX = x;
            vomY = y-SPRITE_HEIGHT;
            break;
        case 180:
            vomX = x-SPRITE_WIDTH;
            vomY = y;
            break;
        case 270:
            vomX = x;
            vomY = y+SPRITE_HEIGHT;
            break;
    }
    list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if ((*it)->canBeInfected())
        {
            if (overlap(vomX, (*it)->getX(), vomY, (*it)->getY()))
            {
                int toVom = randInt(1, 3);
                if (toVom == 3)
                {
                    gameObjects.push_back(new Vomit(vomX, vomY, dir, this));
                    playSound(SOUND_ZOMBIE_VOMIT);
                    return;
                }
            }
        }
        it++;
    }
    if (overlap(vomX, m_penelope->getX(), vomY, m_penelope->getY()))
    {
        int toVom = randInt(1, 3);
        if (toVom == 3)
        {
            gameObjects.push_back(new Vomit(vomX, vomY, dir, this));
            playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
}

bool StudentWorld::checkAllCDead()
{
    bool allDead = true;
    std::list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if ((*it)->canBeInfected())
        {
            if (!((*it)->getIfDead()))
                allDead = false;
        }
        it++;
    }
    return allDead;
}

bool StudentWorld::goodieOverlap(int x, int y)
{
    if (overlap(x, m_penelope->getX(), y, m_penelope->getY()))
    {
        increaseScore(50);
        playSound(SOUND_GOT_GOODIE);
        return true;
    }
    return false;
}

bool StudentWorld::getVacGoodie(int x, int y)
{
    if (goodieOverlap(x, y))
    {
        m_penelope->incVaccines();
        return true;
    }
    return false;
}

bool StudentWorld::getGCGoodie(int x, int y)
{
    if (goodieOverlap(x, y))
    {
        m_penelope->incFlames();
        return true;
    }
    return false;
}

bool StudentWorld::getLMGoodie(int x, int y)
{
    if (goodieOverlap(x, y))
    {
        m_penelope->incLandmines();
        return true;
    }
    return false;
}

void StudentWorld::landmineFlameProtocol(int x, int y, Direction dir)
{
    playSound(SOUND_LANDMINE_EXPLODE);
    Flame* flameoHotman = new Flame(x, y, 90, this);
    gameObjects.push_back(flameoHotman);
    if (checkDir(0, flameoHotman))
        gameObjects.push_back(new Flame(SPRITE_WIDTH+x, y, 90, this));
    if (checkDir(180, flameoHotman))
        gameObjects.push_back(new Flame(x-SPRITE_WIDTH, y, 90, this));
    if (checkDir(90, flameoHotman))
    {
        gameObjects.push_back(new Flame(x, SPRITE_HEIGHT+y, 90, this));
        if (checkDir(0, flameoHotman))
            gameObjects.push_back(new Flame(SPRITE_WIDTH+x, SPRITE_HEIGHT+y, 90, this));
        if (checkDir(180, flameoHotman))
            gameObjects.push_back(new Flame(x-SPRITE_WIDTH, SPRITE_HEIGHT+y, 90, this));
    }
    if (checkDir(270, flameoHotman))
    {
        gameObjects.push_back(new Flame(x, y-SPRITE_HEIGHT, dir, this));
        if (checkDir(0, flameoHotman))
            gameObjects.push_back(new Flame(SPRITE_WIDTH+x, y-SPRITE_HEIGHT, 90, this));
        if (checkDir(180, flameoHotman))
            gameObjects.push_back(new Flame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT, 90, this));
    }
    gameObjects.push_back(new Pit(x, y, this));
}

void StudentWorld::explode(int x, int y, Direction dir, Landmine* act)
{
    if (overlap(x, m_penelope->getX(), y, m_penelope->getY()))
    {
        act->setDead();
        deadPenelopeProtocol();
        landmineFlameProtocol(x, y, dir);
    }
    list<Actor*>::iterator it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        if (overlap(x, (*it)->getX(), y, (*it)->getY()))
        {
            if ((*it)->canBeInfected())
            {
                act->setDead();
                deadCitizenProtocol((*it));
                landmineFlameProtocol(x, y, dir);
            }
            else if ((*it)->canBeDamaged() && (*it)->canBlock())
            {
                act->setDead();
                deadZombieProtocol((*it));
                landmineFlameProtocol(x, y, dir);
            }
        }
        it++;
    }
}

void StudentWorld::deadCitizenProtocol(Actor *act)
{
    act->setDead();
    increaseScore(-1000);
    playSound(SOUND_CITIZEN_DIE);
}

void StudentWorld::deadPenelopeProtocol()
{
    m_penelope->setDead();
    decLives();
}

void StudentWorld::deadZombieProtocol(Actor *act)
{
    act->setDead();
    playSound(SOUND_ZOMBIE_DIE);
    act->deathPoints();
}

void StudentWorld::penFlames()
{
    int posX = 0;
    int posY = 0;
    for (int i = 1; i < 4; i++)
    {
        switch(m_penelope->getDirection())
        {
            case(90):
                posX = m_penelope->getX();
                posY = (m_penelope->getY()) + (i*SPRITE_HEIGHT);
                break;
            case(270):
                posX = m_penelope->getX();
                posY = (m_penelope->getY()) - (i*SPRITE_HEIGHT);
                break;
            case(180):
                posX = (m_penelope->getX()) - (i*SPRITE_WIDTH);
                posY = m_penelope->getY();
                break;
            case(0):
                posX = (m_penelope->getX()) + (i*SPRITE_WIDTH);
                posY = m_penelope->getY();
                break;
        }
        list<Actor*>::iterator it = gameObjects.begin();
        while (it != gameObjects.end())
        {
            if ((*it)->canBlockFlames())
            {
                if (overlap(posX, (*it)->getX(), posY, ((*it)->getY())))
                {
                    return;
                }
            }
            it++;
        }
        gameObjects.push_back(new Flame(posX, posY, m_penelope->getDirection(), this));
    }
}

void StudentWorld::addALandmine(int x, int y)
{
    gameObjects.push_back(new Landmine(x, y, this));
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}
