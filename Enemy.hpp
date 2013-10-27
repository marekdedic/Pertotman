/**
 * @file Enemy.hpp
 * File with Enemy
 * @author GenaBitu
 **/

#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "Libs.hpp"
class GLTexture;
class RendObj;
class Space;

class Enemy: public RendObj
{
    public:
     static int velocity;
     Enemy(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     void update();
    private:
     Space* TransBuff[3];
     void path();
};

#endif //ENEMY_HPP