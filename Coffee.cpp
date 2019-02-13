#include "Main.hpp"
using namespace std;

unsigned int Coffee::count = 0;
Coffee::Coffee(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), armed(true)
{
	textures.x = 64;
	textures.y = 0;
    count++;
}
void Coffee::update()
{
	if(armed)
    {
		if (((x + 30) > CURRENT->pacmans[0]->x) and (x < (CURRENT->pacmans[0]->x + 30)) and ((y + 30) > CURRENT->pacmans[0]->y) and (y < (CURRENT->pacmans[0]->y + 30)))
        {
        	count--;
            if (count < 1)
			{
				PAUSE = 500;
				NEXT_LEVEL = true;
			}
			armed = false;
		}
	}
}
void Coffee::render() const
{
	if(armed)
    {
		RendObj::render();
	}
}
