/**
 * @file Wall.hpp
 * File with Wall
 * @author GenaBitu
 **/

#ifndef WALL_HPP
#define WALL_HPP
#include "Libs.hpp"
class RendObj;

class Wall: public RendObj
{
    public:
     Wall(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
};

#endif //WALL_HPP
