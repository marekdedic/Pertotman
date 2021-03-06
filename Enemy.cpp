#include "Main.hpp"
using namespace std;

Enemy::Enemy(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures)
{
    textures.x = 0;
	textures.y = 32;
	for (unsigned int i = 0; i < 3; i++)
    {
        TransBuff[i] = nullptr;
    }
}
void Enemy::update()
{
    GLfloat MoveBuff = velocity * (DELTA / 1000.f);
    for (unsigned int i = 0; i < 3; i++)
    {
        if (TransBuff[i] == nullptr)
        {
            if (i < 2) {continue;}
            CURRENT->pacmans[0]->path();
            i = 0;
            if (TransBuff[i] == nullptr) {cerr << "Pathfinder error. Skipped." << endl; return;}
        }
        GLfloat vec[2] = {TransBuff[i]->x - x, TransBuff[i]->y - y};
        GLfloat mag = sqrt(pow(vec[0], 2) + pow(vec[1], 2));
        if ((i < 2) and (TransBuff[i + 1] != nullptr))
        {
            GLfloat vec2[2] = {TransBuff[i + 1]->x - x, TransBuff[i + 1]->y - y};
            GLfloat dotProduct = vec[0] * vec2[0] + vec[1] * vec2[1];
            GLfloat mag2 = sqrt(pow(vec2[0], 2) + pow(vec2[1], 2));
            GLfloat cosine = dotProduct / (mag * mag2);
            if(cosine < 0)
            {
                continue;
            }
        }
        if (MoveBuff < mag)
        {
            x += (vec[0] * MoveBuff) / sqrt(pow(vec[0], 2) + pow(vec[1], 2));
            y += (vec[1] * MoveBuff) / sqrt(pow(vec[0], 2) + pow(vec[1], 2));
            break;
        }
        MoveBuff -= mag;
        x = TransBuff[i]->x;
        y = TransBuff[i]->y;
        TransBuff[i] = nullptr;
    }
    if (((x + 25) > CURRENT->pacmans[0]->x) and (x < (CURRENT->pacmans[0]->x + 25)) and ((y + 25) > CURRENT->pacmans[0]->y) and (y < (CURRENT->pacmans[0]->y + 25)))
	{
		PAUSE = 500;
		REPEAT_LEVEL = true;
	}
}
void Enemy::setPath(Space* pacmanPos)
{
    Space* target = nullptr;
	for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
	{
		if((CURRENT->spaces[i]->y == RoundTo(y, 32)) and (CURRENT->spaces[i]->x == RoundTo(x, 32)))
		{
            target = CURRENT->spaces[i];
            break;
        }
	}
	TransBuff[0] = nullptr;
	TransBuff[1] = nullptr;
	TransBuff[2] = nullptr;

    TransBuff[0] = target;
    //cout << endl << "PATH:" << endl << target->x / 32 << " " << target->y / 32 << endl;
	unsigned int i{1};
	while(target != pacmanPos)
	{
        Arc* arc = target->pred;
        if(target == arc->spaces.front())
        {
            for(auto it{next(arc->spaces.begin())}; it != arc->spaces.end(); ++it)
            {
                if(i < 2)
                {
                    TransBuff[i] = *it;
                    ++i;
                }
                //cout << (*it)->x / 32 << " " << (*it)->y / 32 << endl;
            }
            target = arc->spaces.back();
        }
        else
        {
            for(auto it{next(arc->spaces.rbegin())}; it != arc->spaces.rend(); ++it)
            {
                if(i < 2)
                {
                    TransBuff[i] = *it;
                    ++i;
                }
                //cout << (*it)->x / 32 << " " << (*it)->y / 32 << endl;
            }
            target = arc->spaces.front();
        }

    }
}
