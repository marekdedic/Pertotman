#ifndef PERTOTMAN_HPP
#define PERTOTMAN_HPP
#include <string>
#include <vector>

GLint RoundTo(GLfloat, GLint);

class GLCoord
{
    public:
     const GLfloat x;
     const GLfloat y;
     GLCoord* up;
     GLCoord* down;
     GLCoord* left;
     GLCoord* right;
     int dist;
     GLCoord(GLfloat, GLfloat);
     void neighbours();
     void compute(int, char);
};

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
     GLCoord* TransBuff[3];
     GLfloat xVel, yVel;
     static const int velocity;
     void pathbase(bool&, bool&, bool&, bool&);
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
     std::vector<Wall*> walls;
     std::vector<Coffee*> targets;
     std::vector<Bad*> bads;
     std::vector<Pacman*> characters;
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
