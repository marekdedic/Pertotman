#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "Pertotman.hpp"
using namespace std;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 1024;
int DELTA = 0;
int PAUSE = 0;
bool QUIT = false;
bool REPEAT_LEVEL = false;
bool NEXT_LEVEL = true;
Level* CURRENT = new Level;

GLint RoundTo(GLfloat input, GLint step)
{
    GLint counter = 0;
    while(input > step)
    {
        input -= step;
        counter++;
    }
    if(input < (step / 2)) {return step * counter;}
    else {return step * (counter + 1);}
}

GLuint GLTexture::usedIDs = 1;
GLTexture::GLTexture(GLfloat xIn = 0, GLfloat yIn = 0, GLfloat wIn = 0, GLfloat hIn = 0) : x(xIn), y(yIn), w(wIn), h(hIn), ID(usedIDs)
{
    usedIDs++;
}
void GLTexture::load(string filename)
{
	SDL_Surface* image = NULL;
    image = IMG_Load(filename.c_str());
    if(image == NULL) {cerr << "Failed to load image: " << filename; QUIT = true;}
    else
	{
		glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        GLenum mode = GL_RGB;
        if(image->format->BytesPerPixel == 4) {mode = GL_RGBA;}
        glTexImage2D(GL_TEXTURE_2D, 0, mode, image->w, image->h, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        w = image->w;
        h = image->h;
        SDL_FreeSurface(image);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {cerr << "Failed to convert image: " << gluErrorString(error); QUIT = true;}
	}
}

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

MenuItem::MenuItem(GLfloat xIn, GLfloat yIn, GLTexture srcTextures, menuItemType srcType) : RendObj(xIn, yIn, srcTextures), type(srcType)
{
    caption.load("caption.png");
    caption.w = 512;
    caption.h = 64;
    textures.w = 512;
    textures.h = 64;
    switch(type)
    {
        case NEW:
         break;
        case CLOSE:
         caption.y = 64;
         break;
        case SAVE:
         caption.y = 128;
         break;
        case LOAD:
         caption.y = 192;
         break;
        case SET:
         caption.y = 256;
         break;
        case BACK:
         caption.y = 320;
         break;
        case SHUT:
         caption.y = 384;
         break;
        default:
         break;
    }
}
bool MenuItem::handleM(SDL_Event inputEvent)
{
    bool state = true;
    if((inputEvent.type == SDL_MOUSEBUTTONDOWN) and (inputEvent.button.button == SDL_BUTTON_LEFT) and (inputEvent.button.x > x) and (inputEvent.button.x < (x + 512)) and (inputEvent.button.y > y) and (inputEvent.button.y < (y + 64)))
    {
        state = action();
    }
    return state;
}
bool MenuItem::action()
{
    bool state = true;
    switch(type)
    {
        case NEW:
        case CLOSE:
         state = false;
         break;
        case SAVE:
         break;
        case LOAD:
         break;
        case SET:
         break;
        case BACK:
         CURRENT->next = "NULL";
         NEXT_LEVEL = true;
         state = false;
         break;
        case SHUT:
         QUIT = true;
         break;
        default:
         break;
    }
    return state;
}
void MenuItem::render() const
{
    glBindTexture(GL_TEXTURE_2D, textures.ID);
    glBegin(GL_QUADS);
     glTexCoord2f(textures.x / 512, textures.y / 64); glVertex2f(x, y);
     glTexCoord2f((textures.x + textures.w) / 512, textures.y / 64); glVertex2f(x + textures.w, y);
     glTexCoord2f((textures.x + textures.w) / 512, (textures.y + textures.h) / 64); glVertex2f(x + textures.w, y + textures.h);
     glTexCoord2f(textures.x / 512, (textures.y + textures.h) / 64); glVertex2f(x, y + textures.h);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, caption.ID);
    glBegin(GL_QUADS);
     glTexCoord2f(caption.x / 512, caption.y / 444); glVertex2f(x, y);
     glTexCoord2f((caption.x + caption.w) / 512, caption.y / 444); glVertex2f(x + caption.w, y);
     glTexCoord2f((caption.x + caption.w) / 512, (caption.y + caption.h) / 444); glVertex2f(x + caption.w, y + caption.h);
     glTexCoord2f(caption.x / 512, (caption.y + caption.h) / 444); glVertex2f(x, y + caption.h);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

Menu::Menu(menuType type) : xMin((SCREEN_WIDTH / 2) - 384), xMax((SCREEN_WIDTH / 2) + 384), yMin(32), yMax (SCREEN_HEIGHT - 32)
{
    GLfloat yBuff = yMin + 32;
    button.load("button.png");
    switch(type)
    {
        case MAIN:
         items.push_back(new MenuItem(xMin + 128, yBuff, button, NEW));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, LOAD));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, SET));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, SHUT));
         yBuff += 96;
         break;
        case SETTINGS:
         break;
        case ESCAPE:
         items.push_back(new MenuItem(xMin + 128, yBuff, button, CLOSE));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, SAVE));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, LOAD));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, SET));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, BACK));
         yBuff += 96;
         break;
        default:
         break;
    }
}
bool Menu::handle(SDL_Event inputEvent)
{
    bool state = true;
    for (unsigned int i = 0; i < items.size(); i++)
    {
        if(!items[i]->handleM(inputEvent)) {state = false;}
    }
    return state;
}
void Menu::render() const
{
    glBegin(GL_QUADS);
     glColor4f(0.5, 0.5, 0.5, 0.95); glVertex2f(xMin, yMin);
     glColor4f(0.5, 0.5, 0.5, 0.95); glVertex2f(xMax, yMin);
     glColor4f(0.25, 0.25, 0.25, 0.95); glVertex2f(xMax, yMax);
     glColor4f(0.25, 0.25, 0.25, 0.95); glVertex2f(xMin, yMax);
    glEnd();
    glColor4f(1, 1, 1, 1);
    for (unsigned int i = 0; i < items.size(); i++)
    {
        items[i]->render();
    }
}

Space::Space(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), up(nullptr), down(nullptr), left(nullptr), right(nullptr), dist(-1)
{
    textures.x = 0;
    textures.y = 0;
}
void Space::neighbours()
{
	for(unsigned int i = 0; i < CURRENT->spaces.size(); i++)
	{
		if((CURRENT->spaces[i]->x == x) and (CURRENT->spaces[i]->y == (y + 32))) {down = CURRENT->spaces[i];}
		if((CURRENT->spaces[i]->x == x) and (CURRENT->spaces[i]->y == (y - 32))) {up = CURRENT->spaces[i];}
		if((CURRENT->spaces[i]->y == y) and (CURRENT->spaces[i]->x == (x + 32))) {right = CURRENT->spaces[i];}
		if((CURRENT->spaces[i]->y == y) and (CURRENT->spaces[i]->x == (x - 32))) {left = CURRENT->spaces[i];}
	}
}
void Space::compute(int ThisDist, char from = 'a')
{
	if((dist == -1) or (ThisDist < dist))
    {
        dist = ThisDist;
        if((up != nullptr) and (from != 'u')) {up->compute(ThisDist + 1, 'd');}
        if((down != nullptr) and (from != 'd')) {down->compute(ThisDist + 1, 'u');}
        if((left != nullptr) and (from != 'l')) {left->compute(ThisDist + 1, 'r');}
        if((right != nullptr) and (from != 'r')) {right->compute(ThisDist + 1, 'l');}
    }
}

Wall::Wall(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures)
{
    textures.x = 32;
    textures.y = 0;
}

int Coffee::count = 0;
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

const int Bad::velocity = 75;
Bad::Bad(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures)
{
    textures.x = 0;
	textures.y = 32;
}
void Bad::update()
{
	path3();
	GLfloat MoveBuff = velocity * (DELTA / 1000.f);
	if (MoveBuff > 0)
    {
        for(int i = 0; i < 3; i++)
        {
            if(TransBuff[i] == nullptr)
            {
                if(i == 2) {cerr << "Pathfinder error. Skipped." << endl;}
                continue;
            }
            if(TransBuff[i + 1] != nullptr)
            {
                if(((((TransBuff[i]->x - x) * (TransBuff[i + 1]->x - x )) + ((TransBuff[i]->y - y) * (TransBuff[i + 1]->y - y))) / (sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2)) * sqrt(pow(TransBuff[i + 1]->x - x, 2) + pow(TransBuff[i + 1]->y - y, 2)))) <= 0)
                {
                    continue;
                }
            }
            if((sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2)) < MoveBuff) or ((pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2)) < 0.001))
            {
                x = TransBuff[i]->x;
                y = TransBuff[i]->y;
                MoveBuff -= sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
                TransBuff[i] = nullptr;
            }
            else
            {
                x += (MoveBuff * (TransBuff[i]->x - x)) / sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
                y += (MoveBuff * (TransBuff[i]->y - y)) / sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
                MoveBuff = 0;
                break;
            }
        }
    }
    //cout << x << "x" << y << endl;                //ENABLE FOR DEBUG
    if (((x + 25) > CURRENT->pacmans[0]->x) and (x < (CURRENT->pacmans[0]->x + 25)) and ((y + 25) > CURRENT->pacmans[0]->y) and (y < (CURRENT->pacmans[0]->y + 25)))
	{
		PAUSE = 500;
		REPEAT_LEVEL = true;
	}
}
void Bad::path1()
{
    if((TransBuff[0] == nullptr) and (TransBuff[1] == nullptr))
    {
    if ((((RoundTo(x, 32) % RoundTo(x, 1)) < 2) or ((RoundTo(x, 32) % RoundTo(x, 1)) > 30)) and (((RoundTo(y, 32) % RoundTo(y, 1)) < 2) or ((RoundTo(y, 32) % RoundTo(y, 1)) > 30)))
        {
            Space* start = nullptr;
            for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
            {
            if((CURRENT->spaces[i]->y == RoundTo(y, 32)) and (CURRENT->spaces[i]->x == RoundTo(x, 32))) {start = CURRENT->spaces[i];}
        }
        start->compute(0);
        int j = 0;
        while(j < 3)
        {
            int number = rand() % 4;
            if ((number == 0) and (start->up != nullptr) and (start->up != TransBuff[j - 1]) and (start->up != TransBuff[j - 2]))
            {
                TransBuff[j] = start->up;
                start = start->up;
                j++;
                continue;
            }
            if ((number == 1) and (start->down != nullptr) and (start->down != TransBuff[j - 1]) and (start->down != TransBuff[j - 2]))
            {
                TransBuff[j] = start->down;
                start = start->down;
                j++;
                continue;
            }
            if ((number == 2) and (start->left != nullptr) and (start->left != TransBuff[j - 1]) and (start->left != TransBuff[j - 2]))
            {
                TransBuff[j] = start->left;
                start = start->left;
                j++;
                continue;
            }
            if ((number == 3) and (start->right != nullptr) and (start->right != TransBuff[j - 1]) and (start->right != TransBuff[j - 2]))
            {
                TransBuff[j] = start->right;
                start = start->right;
                j++;
                continue;
            }
        }
        for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
        {
            CURRENT->spaces[i]->dist = -1;
        }
    }
    }
}
void Bad::path2()
{/*
	bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    int safecount = 0;
    pathbase(up, down, left, right);
    if (up) {safecount++;}
    if (down) {safecount++;}
    if (left) {safecount++;}
    if (right) {safecount++;}
    if ((((xVel > 0) and right) or ((xVel < 0) and left) or ((yVel > 0) and down) or ((yVel < 0) and up)) and (safecount < 3)) {}
    else
    {
		GLfloat xRel = CURRENT->pacmans[0]->x - x;
		GLfloat yRel = CURRENT->pacmans[0]->y - y;
		if (xRel <= 0)
		{
			if (yRel <= 0)
			{
				if (xRel <= yRel)
				{
					if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else
					{
						xVel = 0;
						yVel = velocity;
					}
				}
				else
				{
					if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else if (right)
					{
						xVel = 0;
						yVel = velocity;
					}
					else
					{
						yVel = 0;
						xVel = velocity;
					}
				}
			}
			else
			{
				if (-xRel <= yRel)
				{
					if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else if (right)
					{
						xVel = 0;
						yVel = velocity;

					}
					else
					{
						xVel = 0;
						yVel = -velocity;
					}
				}
				else
				{
					if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else
					{
						xVel = 0;
						yVel = velocity;
					}
				}
			}
		}
		else
		{
			if (yRel <= 0)
			{
				if (xRel <= -yRel)
				{
					if (right)
					{
						xVel = 0;
						yVel = velocity;
					}
					else if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else
					{
						yVel = 0;
						xVel = -velocity;
					}
				}
				else
				{
					if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else if (right)
					{
						xVel = 0;
						yVel = velocity;
					}
					else if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else
					{
						yVel = 0;
						xVel = velocity;
					}
				}
			}
			else
			{
				if (xRel <= yRel)
				{
					if (right)
					{
						xVel = 0;
						yVel = velocity;

					}
					else if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else
					{
						xVel = 0;
						yVel = -velocity;
					}
				}
				else
				{
					if (down)
					{
						yVel = 0;
						xVel = velocity;
					}
					else if (left)
					{
						yVel = 0;
						xVel = -velocity;
					}
					else if (up)
					{
						xVel = 0;
						yVel = -velocity;
					}
					else
					{
						xVel = 0;
						yVel = velocity;
					}
				}
			}
		}
	}*/
}
void Bad::path3()
{
	Space* target = nullptr;
	Space* start = nullptr;
	for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
	{
		if((CURRENT->spaces[i]->y == RoundTo(y, 32)) and (CURRENT->spaces[i]->x == RoundTo(x, 32))) {start = CURRENT->spaces[i];}
		if((CURRENT->spaces[i]->x == RoundTo(CURRENT->pacmans[0]->x, 32)) and (CURRENT->spaces[i]->y == RoundTo(CURRENT->pacmans[0]->y, 32))) {target = CURRENT->spaces[i];}
	}
	if((start == nullptr) or (target == nullptr)) {cerr << "Pathfinder error. Deactivated Bad." << endl; return;}
	start->compute(0);
	while(target->dist > 0)
    {
        if(target->dist < 3) {TransBuff[target->dist] = target;}
        if((target->up != nullptr) and (target->up->dist == (target->dist - 1)))
        {
            target = target->up;
            continue;
        }
        if((target->left != nullptr) and (target->left->dist == (target->dist - 1)))
        {
            target = target->left;
            continue;
        }
        if((target->right != nullptr) and (target->right->dist == (target->dist - 1)))
        {
            target = target->right;;
            continue;
        }
        if((target->down != nullptr) and (target->down->dist == (target->dist - 1)))
        {
            target = target->down;
            continue;
        }
    }
    TransBuff[0] = target;
	for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
	{
		CURRENT->spaces[i]->dist = -1;
	}
 }

const int Pacman::velocity = 150;
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
//    cout << 1000.f / DELTA << endl;           //FPS
	x += (xPVel - xNVel) * (DELTA / 1000.f);
    y += (yPVel - yNVel) * (DELTA / 1000.f);
    bool xcontrol = true;
    bool ycontrol = true;
    for (unsigned int i = 0; i < CURRENT->walls.size(); i++)
	{
		if (((y + 29) > CURRENT->walls[i]->y) and (y < (CURRENT->walls[i]->y + 29)) and ((((x + 32) > CURRENT->walls[i]->x) and (x < CURRENT->walls[i]->x) and ((xPVel - xNVel) > 0)) or ((x < (CURRENT->walls[i]->x + 32)) and (x > CURRENT->walls[i]->x) and ((xPVel - xNVel) < 0))) and xcontrol)
		{
			x -= (xPVel - xNVel) * (DELTA / 1000.f);
			xcontrol = false;
		}
		if (((x + 29) > CURRENT->walls[i]->x) and (x < (CURRENT->walls[i]->x + 29)) and ((((y + 32) > CURRENT->walls[i]->y) and (y < CURRENT->walls[i]->y) and ((yPVel - yNVel) > 0)) or ((y < (CURRENT->walls[i]->y + 32)) and (y > CURRENT->walls[i]->y) and ((yPVel - yNVel) < 0))) and ycontrol)
		{
			y -= (yPVel - yNVel) * (DELTA / 1000.f);
			ycontrol = false;
		}
	}
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

unsigned int Level::levelCounter = 0;
Level::Level() {}
void Level::initialize(string name)
{
	if (name.empty() or (name == "NULL"))
    {
        current = "level1";
        menus.push_back(new Menu(MAIN));
    }
    else {current = name;}
    stringstream ss;
    GLfloat x;
    GLfloat y = 0;
    fstream file(("levels/" + current + ".lvl").c_str(), ifstream::in);
    string caption;
    if(!REPEAT_LEVEL) {levelCounter++;}
    if(!file.is_open()) {cerr << "Error with loading file: " << strerror(errno); QUIT = true; return;}
    textures.load("textures.png");
    getline(file, caption);
    next = caption;
    getline(file, caption);
    ss << "Pertotman, Level " << levelCounter << ": " << caption;
    caption = ss.str();
    SDL_WM_SetCaption(caption.c_str(), "Pertotman");
    for(int counter = 1; file.eof() == false; counter++)
    {
		x = 0;
		string line;
		file >> line;
		for(unsigned int i = 0; i < line.size(); i++)
		{
			switch (line[i])
			{
				case 'w':
                case 'W':
                 walls.push_back(new Wall(x, y, textures));
                 break;
				case 'c':
                case 'C':
                 coffees.push_back(new Coffee(x, y, textures));
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 'b':
                case 'B':
                 bads.push_back(new Bad(x, y, textures));
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 'p':
                case 'P':
                 pacmans.push_back(new Pacman(x, y, textures));
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 's':
                case 'S':
                 spaces.push_back(new Space(x, y, textures));
                 break;
				default:
                 cerr << "Error with loading level: Invalid character."; QUIT = true; return;
                 break;
			}
			x += 32;
		}
		y += 32;
	}
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		spaces[i]->neighbours();
    }
	if (pacmans.size() != 1) {cerr << "Error with loading level: No, or too many pacman found."; QUIT = true; return;}
}
void Level::handle(SDL_Event inputEvent)
{
    if((inputEvent.type == SDL_KEYDOWN) and (inputEvent.key.keysym.sym == SDLK_ESCAPE))
    {
        if(menus.empty()) {menus.push_back(new Menu(ESCAPE));}
        else {menus.erase(menus.end() - 1);}
    }
    else
    {
        for (unsigned int i = 0; i < menus.size(); i++)
        {
            if(!menus[i]->handle(inputEvent))
            {
                menus.erase(menus.begin() + i);
                break;
            }
        }
        for (unsigned int i = 0; i < pacmans.size(); i++)
        {
                pacmans[i]->handle(inputEvent);
        }
    }
}
void Level::update()
{
    if(menus.empty())
    {
        for (unsigned int i = 0; i < spaces.size(); i++)
        {
            spaces[i]->update();
        }
        for (unsigned int i = 0; i < walls.size(); i++)
        {
            walls[i]->update();
        }
        for (unsigned int i = 0; i < coffees.size(); i++)
        {
            coffees[i]->update();
        }
        for (unsigned int i = 0; i < bads.size(); i++)
        {
            bads[i]->update();
        }
        for (unsigned int i = 0; i < pacmans.size(); i++)
        {
            pacmans[i]->update();
        }
    }
}
void Level::render() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		spaces[i]->render();
	}
	for (unsigned int i = 0; i < walls.size(); i++)
	{
		walls[i]->render();
	}
	for (unsigned int i = 0; i < coffees.size(); i++)
	{
		coffees[i]->render();
	}
	for (unsigned int i = 0; i < bads.size(); i++)
	{
		bads[i]->render();
	}
	for (unsigned int i = 0; i < pacmans.size(); i++)
	{
		pacmans[i]->render();
	}
	for (unsigned int i = 0; i < menus.size(); i++)
	{
		menus[i]->render();
	}
}
void Level::destroy()
{
    for (unsigned int i = 0; i < menus.size(); i++)
	{
		menus.erase(menus.begin() + i);
	}
    for (unsigned int i = 0; i < spaces.size(); i++)
	{
	    spaces.erase(spaces.begin() + i);
	}
	for (unsigned int i = 0; i < walls.size(); i++)
	{
	    walls.erase(walls.begin() + i);
	}
	for (unsigned int i = 0; i < coffees.size(); i++)
	{
	    coffees.erase(coffees.begin() + i);
	}
	for (unsigned int i = 0; i < bads.size(); i++)
	{
	    bads.erase(bads.begin() + i);
	}
	for (unsigned int i = 0; i < pacmans.size(); i++)
	{
	    pacmans.erase(pacmans.begin() + i);
	}
	menus.clear();
	spaces.clear();
	walls.clear();
	coffees.clear();
    bads.clear();
    pacmans.clear();
    Coffee::count = 0;
}

int SDL_main(int, char**)
{
	SDL_Event event;
	int time;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {cerr << "Failed to initialize graphics: " << SDL_GetError() << endl; return 0;}
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {cerr << "Failed to initialize double buffering: " << SDL_GetError() << endl; return 0;}
    if(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1) < 0) {cerr << "Failed to initialize hardware acceleration: " << SDL_GetError() << endl; return 0;}
    if(SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_RESIZABLE | SDL_OPENGL) == NULL) {cerr << "Failed to initialize screen: " << SDL_GetError() << endl; return 0;}
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    GLenum gerror = glGetError();
    if(gerror != GL_NO_ERROR) {cerr << "Failed to initialize OpenGL: " << gluErrorString(gerror); return 0;}
    SDL_GL_SwapBuffers();
    time = SDL_GetTicks();
    srand(SDL_GetTicks());
    while(QUIT == false)
	{
		if (REPEAT_LEVEL and (PAUSE == 0))
		{
			CURRENT->initialize(CURRENT->current);
			REPEAT_LEVEL = false;
		}
		if (NEXT_LEVEL and (PAUSE == 0))
		{
			CURRENT->initialize(CURRENT->next);
			NEXT_LEVEL = false;
		}
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT) {QUIT = true;}
			/*else if(event.type == SDL_VIDEORESIZE)
			{
                SCREEN_WIDTH = event.resize.w;
                SCREEN_HEIGHT = event.resize.h
				if(SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_RESIZABLE | SDL_OPENGL) == NULL) {cerr << "Failed to re-initialize screen: " << SDL_GetError() << endl; return 0;}
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
				glMatrixMode(GL_MODELVIEW);
			}*/
			else {CURRENT->handle(event);}
		}
		if (PAUSE == 0) {CURRENT->update();}
		if (PAUSE > 0)
		{
			if(DELTA < PAUSE) {PAUSE -= DELTA;}
			else {PAUSE = 0;}
		}
		DELTA = SDL_GetTicks() - time;
		time = SDL_GetTicks();
		CURRENT->render();
		SDL_GL_SwapBuffers();
		if (((REPEAT_LEVEL or NEXT_LEVEL) and (PAUSE == 0)) or QUIT)
		{
			CURRENT->destroy();
		}
	}
	delete CURRENT;
	return 1;
}
