#ifndef PERTOTMAN_HPP
#define PERTOTMAN_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

GLint RoundTo(GLfloat, GLint);

class GLTexture
{
    public:
     GLfloat x;
     GLfloat y;
     GLfloat w;
     GLfloat h;
     GLuint ID;
     GLTexture(GLfloat, GLfloat, GLfloat, GLfloat);
     void loadIMG(std::string);
     void loadText(std::string, std::string, int, SDL_Color);
    private:
     static GLuint usedIDs;
     void process(SDL_Surface*);
};

class RendObj
{
    public:
     GLfloat x, y;
     virtual void handle(SDL_Event);
     virtual void update();
     virtual void render() const;
    protected:
     RendObj(GLfloat, GLfloat, GLTexture);
     GLTexture textures;
     virtual ~RendObj();
};

enum menuItemType {NEW, CLOSE, SAVE, LOAD, SET, BACK, SHUT, FPS, CLOSESET};

class MenuItem: public RendObj
{
    public:
     MenuItem(GLfloat, GLfloat, GLTexture, menuItemType);
     bool handleM(SDL_Event);
     void render() const;
    private:
     menuItemType type;
     GLTexture caption;
     bool action();
};

enum menuType {MAIN, SETTINGS, ESCAPE};

class Menu
{
    public:
     Menu(menuType);
     bool handle(SDL_Event);
     void render() const;
    private:
     GLTexture button;
     GLfloat xMin;
     GLfloat xMax;
     GLfloat yMin;
     GLfloat yMax;
     std::vector<MenuItem*> items;
};

class Level;
class Space: public RendObj
{
    public:
     Space* up;
     Space* down;
     Space* left;
     Space* right;
     int dist;
     Space(GLfloat, GLfloat, GLTexture);
     void neighbours(Level*);
     void compute(int, char);
};

class Wall: public RendObj
{
    public:
     Wall(GLfloat, GLfloat, GLTexture);
};

class Coffee: public RendObj
{
    public:
     static int count;
     Coffee(GLfloat, GLfloat, GLTexture);
     void update();
     void render() const;
    private:
     bool armed;
};

class Bad: public RendObj
{
    public:
     Bad(GLfloat, GLfloat, GLTexture);
     void update();
    private:
     Space* TransBuff[3];
     static const int velocity;
     void path1();
     void path2();
     void path3();
};

class Pacman: public RendObj
{
    public:
     Pacman(GLfloat, GLfloat, GLTexture);
     void handle(SDL_Event);
     void update();
     void render();
    private:
     GLfloat xPVel, xNVel, yPVel, yNVel, angle;
     static const int velocity;
};

class Level
{
    public:
     static unsigned int levelCounter;
     static std::string current;
     static std::string next;
     std::vector<Menu*> menus;
     std::vector<Space*> spaces;
     std::vector<Wall*> walls;
     std::vector<Coffee*> coffees;
     std::vector<Bad*> bads;
     std::vector<Pacman*> pacmans;
     Level(std::string);
     ~Level();
     void handle(SDL_Event);
     void update();
     void render() const;
    private:
     GLTexture textures;
};

#endif
