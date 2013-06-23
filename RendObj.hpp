/**
 * @file RendObj.hpp
 * File with RendObj
 * @author GenaBitu
 **/

#ifndef RENDOBJ_HPP
#define RENDOBJ_HPP
#include "Libs.hpp"
class GLTexture;

class RendObj
{
    public:
     GLfloat x, y;
     virtual void handle(SDL_Event);
     virtual void update();
     virtual void render() const;
    protected:
     GLTexture textures;
     RendObj(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     virtual ~RendObj();
};

#endif //RENDOBJ_HPP
