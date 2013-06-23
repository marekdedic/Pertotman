#include "Main.hpp"
using namespace std;

Wall::Wall(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures)
{
    textures.x = 32;
    textures.y = 0;
}
