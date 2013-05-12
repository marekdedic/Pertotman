/**
 * @file Coffee.hpp
 * File with Coffee
 * @author GenaBitu
 **/

#ifndef COFFEE_HPP
#define COFFEE_HPP
#include "Libs.hpp"
class GLTexture;
class RendObj;

class Coffee: public RendObj
{
    public:
     bool armed;
     static unsigned int count;
     Coffee(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     void update();
     void render() const;
};

#endif //Coffee_HPP
