/**
 * @file GLTexture.hpp
 * File with GLTexture
 * @author GenaBitu
 **/

#ifndef GLTEXTURE_HPP
#define GLTEXTURE_HPP
#include "Libs.hpp"

class GLTexture
{
    public:
     GLfloat x;
     GLfloat y;
     GLfloat w;
     GLfloat h;
     GLuint ID;
     GLTexture(GLfloat xIn = 0, GLfloat yIn = 0, GLfloat wIn = 0, GLfloat hIn = 0);
     void loadIMG(std::string filename);
     void loadText(std::string text, std::string filename, int size, SDL_Color color);
    private:
     static GLuint usedIDs;
     void process(SDL_Surface* image);
};

#endif //GLTEXTURE_HPP
