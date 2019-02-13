#include "Main.hpp"
using namespace std;

const GLint Pacman::velocity = 150;
Pacman::Pacman(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), xPVel(0), xNVel(0), yPVel(0), yNVel(0), angle(0)
{
    textures.x = 96;
    textures.y = 0;
}
void Pacman::handle(SDL_Event inputEvent)
{
	if(inputEvent.type == SDL_KEYDOWN)
	{
     switch(inputEvent.key.keysym.sym)
		{
			case SDLK_PAUSE:
			 if (PAUSE == 0) {PAUSE = -1;}
             else {PAUSE = 0;}
             break;
			case SDLK_UP:
             yNVel = velocity;
             if (PAUSE == 0) {angle = 270;}
             break;
			case SDLK_DOWN:
             yPVel = velocity;
             if (PAUSE == 0) {angle = 90;}
             break;
			case SDLK_LEFT:
             xNVel = velocity;
             if (PAUSE == 0) {angle = 180;}
             break;
			case SDLK_RIGHT:
             xPVel = velocity;
             if (PAUSE == 0) {angle = 0;}
             break;
			default:
             break;
		}
	}
	else if(inputEvent.type == SDL_KEYUP)
	{
		switch(inputEvent.key.keysym.sym)
		{
			case SDLK_UP:
             yNVel = 0;
             break;
            case SDLK_DOWN:
             yPVel = 0;
             break;
			case SDLK_LEFT:
             xNVel = 0;
             break;
			case SDLK_RIGHT:
             xPVel = 0;
             break;
			default:
             break;
		}
	}
}
void Pacman::update()
{
    GLfloat xMove = (xPVel - xNVel) * (DELTA / 1000.f);
    GLfloat yMove = (yPVel - yNVel) * (DELTA / 1000.f);
    for(unsigned int i = 0; i < CURRENT->walls.size(); i++)
    {
        if (((y + yMove + 25) > CURRENT->walls[i]->y) and (y + yMove < (CURRENT->walls[i]->y + 25)) and ((x + xMove + 32) > CURRENT->walls[i]->x) and ((x + xMove) < CURRENT->walls[i]->x) and (xMove > 0))
        {
            xMove = CURRENT->walls[i]->x - (x + 30);
        }
        if (((y + yMove + 25) > CURRENT->walls[i]->y) and (y + yMove < (CURRENT->walls[i]->y + 25)) and ((x + xMove) < (CURRENT->walls[i]->x + 32)) and ((x + xMove) > CURRENT->walls[i]->x) and (xMove < 0))
        {
            xMove = (CURRENT->walls[i]->x + 30) - x;
        }
        if (((x + xMove + 25) > CURRENT->walls[i]->x) and (x + xMove < (CURRENT->walls[i]->x + 25)) and ((y + yMove + 32) > CURRENT->walls[i]->y) and ((y + yMove) < CURRENT->walls[i]->y) and (yMove > 0))
        {
            yMove = CURRENT->walls[i]->y - (y + 30);
        }
        if (((x + xMove + 25) > CURRENT->walls[i]->x) and (x + xMove < (CURRENT->walls[i]->x + 25)) and ((y + yMove) < (CURRENT->walls[i]->y + 32)) and ((y + yMove) > CURRENT->walls[i]->y) and (yMove < 0))
        {
            yMove = (CURRENT->walls[i]->y + 30) - y;
        }
    }
    x += xMove;
    y += yMove;
}
void Pacman::render()
{
	glTranslatef(x + 16,y + 16, 0);
    glRotatef(angle, 0, 0, 1);
    if (angle >= 180) {glScalef(1, -1, 0);}
    glBindTexture(GL_TEXTURE_2D, textures.ID);
    glBegin(GL_QUADS);
     glTexCoord2f(textures.x/128, textures.y/128); glVertex2f(-16, -16);
     glTexCoord2f((textures.x + 32)/128, textures.y/128); glVertex2f(16, -16);
     glTexCoord2f((textures.x + 32)/128, (textures.y + 32)/128); glVertex2f(16, 16);
     glTexCoord2f(textures.x/128, (textures.y + 32)/128); glVertex2f(-16, 16);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glLoadIdentity();
}
void Pacman::path()
{
	Space* start{nullptr};
	for(auto space: CURRENT->spaces)
	{
		if((space->y == RoundTo(y, 32)) and (space->x == RoundTo(x, 32))) {start = space;}
    }
    if(start == nullptr)
    {
        cerr << "Pathfinder error." << endl;
        return;
    }
    vector<Space*> targets{vector<Space*>{}};
    for(auto enemy: CURRENT->enemies)
    {
        for(auto space: CURRENT->spaces)
        {
            if((space->x == RoundTo(enemy->x, 32)) and (space->y == RoundTo(enemy->y, 32))) {targets.push_back(space);}
        }
    }
	if(targets.size() < CURRENT->enemies.size())
	{
        cerr << "Pathfinder error." << endl;
        return;
    }
    vector<Space*> endpoints{targets};
    endpoints.push_back(start);
	CURRENT->computeGraph(endpoints, start);
    for(unsigned int i{0}; i < targets.size(); ++i)
    {
        CURRENT->pathfind(targets[i]);
        CURRENT->enemies[i]->setPath(start);
    }
 }
