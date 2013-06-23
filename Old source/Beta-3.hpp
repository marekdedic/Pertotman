#ifndef PERTOTMAN_HPP
#define PERTOTMAN_HPP
#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

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
     void load(std::string);
    private:
     static GLuint usedIDs;
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

enum menuItemType {NEW, CLOSE, SAVE, LOAD, SET, BACK, SHUT};

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

class Space: public RendObj
{
    public:
     Space* up;
     Space* down;
     Space* left;
     Space* right;
     int dist;
     Space(GLfloat, GLfloat, GLTexture);
     void neighbours();
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
     std::string current;
     std::string next;
     std::vector<Menu*> menus;
     std::vector<Space*> spaces;
     std::vector<Wall*> walls;
     std::vector<Coffee*> coffees;
     std::vector<Bad*> bads;
     std::vector<Pacman*> pacmans;
     Level();
     void initialize(std::string);
     void handle(SDL_Event);
     void update();
     void render() const;
     void destroy();
    private:
     static unsigned int levelCounter;
     GLTexture textures;
};

#endif
