#include "Main.hpp"
using namespace std;

RendObj::RendObj(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : x(xIn), y(yIn), textures(srcTextures)
{
    textures.w = 32;
    textures.h = 32;
}
void RendObj::handle(SDL_Event) {}
void RendObj::update() {}
void RendObj::render() const
{
	glBindTexture(GL_TEXTURE_2D, textures.ID);
    glBegin(GL_QUADS);
     glTexCoord2f(textures.x / 128, textures.y / 128); glVertex2f(x, y);
     glTexCoord2f((textures.x + textures.w) / 128, textures.y / 128); glVertex2f(x + textures.w, y);
     glTexCoord2f((textures.x + textures.w) / 128, (textures.y + textures.h) / 128); glVertex2f(x + textures.w, y + textures.h);
     glTexCoord2f(textures.x / 128, (textures.y + textures.h) / 128); glVertex2f(x, y + textures.h);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}
RendObj::~RendObj() {}
