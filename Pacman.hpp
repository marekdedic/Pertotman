/**
 * @file Pacman.hpp
 * File with Pacman
 * @author GenaBitu
 **/

#ifndef PACMAN_HPP
#define PACMAN_HPP
#include "Libs.hpp"
class RendObj;

class Pacman: public RendObj
{
    public:
     Pacman(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     void handle(SDL_Event inputEvent);
     void update();
     void render();
     void path();
    private:
     GLfloat xPVel, xNVel, yPVel, yNVel, angle;
     static const int velocity;
};

#endif //PACMAN_HPP
