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

int TIME = 0;
int PAUSE = 0;
bool QUIT = false;
bool REPEAT_LEVEL = false;
bool NEXT_LEVEL = true;
Level current;
vector<GLCoord*> spaces;

GLint RoundTo(GLfloat input, GLint step)
{
// TODO (dedicovi#1#): změnit na zaokrouhlení
	GLint number = static_cast<GLint>(input);
	if((number % step) < (step/2)) {return (number - (number % step));}
	else {return (number + (step - (number % step)));}
}

GLCoord::GLCoord(GLfloat xIn, GLfloat yIn) : x(xIn), y(yIn), up(nullptr), down(nullptr), left(nullptr), right(nullptr), dist(-1) {}
void GLCoord::neighbours()
{
	for(unsigned int i = 0; i < spaces.size(); i++)
	{
		if((spaces[i]->x == x) and (spaces[i]->y == (y + 32))) {down = spaces[i];}
		if((spaces[i]->x == x) and (spaces[i]->y == (y - 32))) {up = spaces[i];}
		if((spaces[i]->y == y) and (spaces[i]->x == (x + 32))) {right = spaces[i];}
		if((spaces[i]->y == y) and (spaces[i]->x == (x - 32))) {left = spaces[i];}
	}
}
void GLCoord::compute(int ThisDist, char from = 'a')
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
     glTexCoord2f(textures.x/128, textures.y/128); glVertex2f(x, y);
     glTexCoord2f((textures.x + 32)/128, textures.y/128); glVertex2f(x + 32, y);
     glTexCoord2f((textures.x + 32)/128, (textures.y + 32)/128); glVertex2f(x + 32, y + 32);
     glTexCoord2f(textures.x/128, (textures.y + 32)/128); glVertex2f(x, y + 32);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}
RendObj::~RendObj() {}

Wall::Wall(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures) {}

int Coffee::count = 0;
Coffee::Coffee(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), armed(true)
{
	textures.x = 32;
    count++;
}
void Coffee::update()
{
	if(armed)
    {
		if (((x + 30) > current.characters[0]->x) and (x < (current.characters[0]->x + 30)) and ((y + 30) > current.characters[0]->y) and (y < (current.characters[0]->y + 30)))
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
Bad::Bad(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), xVel(0), yVel(0)
{
	textures.y = 32;
}
void Bad::update()
{
	path3();
	GLfloat MoveBuff = velocity * ((SDL_GetTicks() - TIME) / 1000.f);
	for(int i = 0; i < 3; i++)
    {
        if(((((TransBuff[i]->x - x) * (TransBuff[i + 1]->x - x )) + ((TransBuff[i]->y - y) * (TransBuff[i + 1]->y - y))) / (sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2)) * sqrt(pow(TransBuff[i + 1]->x - x, 2) + pow(TransBuff[i + 1]->y - y, 2)))) <= 0)
        {
            continue;
        }
        if(sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2)) <= MoveBuff)
        {
            x = TransBuff[i]->x;
            y = TransBuff[i]->y;
            MoveBuff -= sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
        }
        else
        {
            x += (MoveBuff * (TransBuff[i]->x - x)) / sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
            y += (MoveBuff * (TransBuff[i]->y - y)) / sqrt(pow(TransBuff[i]->x - x, 2) + pow(TransBuff[i]->y - y, 2));
            MoveBuff = 0;
            break;
        }
    }
    if (((x + 30) > current.characters[0]->x) and (x < (current.characters[0]->x + 30)) and ((y + 30) > current.characters[0]->y) and (y < (current.characters[0]->y + 30)))
	{
		PAUSE = 500;
		REPEAT_LEVEL = true;
	}
}
void Bad::pathbase(bool &up, bool &down, bool &left, bool &right)
{
	if(((static_cast<GLint>(x) % 32) < 1) or ((static_cast<GLint>(x) % 32) > 31))
	{
		for (unsigned int i = 0; i < spaces.size(); i++)
		{
			if((spaces[i]->x == RoundTo(x, 32)) and (spaces[i]->y == (RoundTo(y + 22, 32) - 32))) {up = true;}
			if((spaces[i]->x == RoundTo(x, 32)) and (spaces[i]->y == (RoundTo(y, 32) + 32))) {down = true;}
		}
	}
	else
	{
		left = true;
		right = true;
	}
	if(((static_cast<GLint>(y) % 32) < 2) or ((static_cast<GLint>(y) % 32) > 30))
	{
		for (unsigned int i = 0; i < spaces.size(); i++)
		{
			if((spaces[i]->y == RoundTo(y, 32)) and (spaces[i]->x == (RoundTo(x + 22, 32) - 32))) {left = true;}
			if((spaces[i]->y == RoundTo(y, 32)) and (spaces[i]->x == (RoundTo(x, 32) + 32))) {right = true;}
		}
	}
	else
	{
		up = true;
		down = true;
	}
}
void Bad::path1()
{
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
		safecount = 20;
		while(true)
		{
			int count = rand() % 4;
			if ((count == 0) and up)
			{
				xVel = 0;
				yVel = -velocity;
				break;
			}
			if ((count == 1) and down)
			{
				xVel = 0;
				yVel = velocity;
				break;
			}
			if ((count == 2) and left)
			{
				yVel = 0;
				xVel = -velocity;
				break;
			}
			if ((count == 3) and right)
			{
				yVel = 0;
				xVel = velocity;
				break;
			}
			safecount--;
			if (safecount < 1 ) {break;}
		}
	}
}
void Bad::path2()
{
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
		GLfloat xRel = current.characters[0]->x - x;
		GLfloat yRel = current.characters[0]->y - y;
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
	}
}
void Bad::path3()
{
	GLCoord* target = nullptr;
	GLCoord* start = nullptr;
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		if((spaces[i]->y == RoundTo(y, 32)) and (spaces[i]->x == RoundTo(x, 32))) {start = spaces[i];}
		if((spaces[i]->x == RoundTo(current.characters[0]->x, 32)) and (spaces[i]->y == RoundTo(current.characters[0]->y, 32))) {target = spaces[i];}
	}
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
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		spaces[i]->dist = -1;
	}
 }

const int Pacman::velocity = 150;
Pacman::Pacman(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), xPVel(0), xNVel(0), yPVel(0), yNVel(0), angle(0)
{
    textures.x = 64;
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
//    cout << (1 / ((SDL_GetTicks() - TIME) / 1000.f)) << endl; //FPS
	x += (xPVel - xNVel) * ((SDL_GetTicks() - TIME) / 1000.f);
    y += (yPVel - yNVel) * ((SDL_GetTicks() - TIME) / 1000.f);
    bool xcontrol = true;
    bool ycontrol = true;
    for (unsigned int i = 0; i < current.walls.size(); i++)
	{
		if (((y + 29) > current.walls[i]->y) and (y < (current.walls[i]->y + 29)) and ((((x + 32) > current.walls[i]->x) and (x < current.walls[i]->x) and ((xPVel - xNVel) > 0)) or ((x < (current.walls[i]->x + 32)) and (x > current.walls[i]->x) and ((xPVel - xNVel) < 0))) and xcontrol)
		{
			x -= (xPVel - xNVel) * ((SDL_GetTicks() - TIME) / 1000.f);
			xcontrol = false;
		}
		if (((x + 29) > current.walls[i]->x) and (x < (current.walls[i]->x + 29)) and ((((y + 32) > current.walls[i]->y) and (y < current.walls[i]->y) and ((yPVel - yNVel) > 0)) or ((y < (current.walls[i]->y + 32)) and (y > current.walls[i]->y) and ((yPVel - yNVel) < 0))) and ycontrol)
		{
			y -= (yPVel - yNVel) * ((SDL_GetTicks() - TIME) / 1000.f);
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
	if (name.empty()) {current = "level1";}
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
                 walls.push_back(new Wall(x, y, textures));
                 break;
				case 'c':
                 targets.push_back(new Coffee(x, y, textures));
                 spaces.push_back(new GLCoord(x, y));
                 break;
				case 'b':
                 bads.push_back(new Bad(x, y, textures));
                 spaces.push_back(new GLCoord(x, y));
                 break;
				case 'p':
                 characters.push_back(new Pacman(x, y, textures));
                 spaces.push_back(new GLCoord(x, y));
                 break;
				case 's':
                 spaces.push_back(new GLCoord(x, y));
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
	if (characters.size() != 1) {cerr << "Error with loading level: No, or too many pacman found."; QUIT = true; return;}
}
void Level::handle(SDL_Event inputEvent)
{
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		characters[i]->handle(inputEvent);
	}
}
void Level::update()
{
	for (unsigned int i = 0; i < walls.size(); i++)
	{
		walls[i]->update();
	}
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		targets[i]->update();
	}
	for (unsigned int i = 0; i < bads.size(); i++)
	{
		bads[i]->update();
	}
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		characters[i]->update();
	}
}
void Level::render() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i = 0; i < walls.size(); i++)
	{
		walls[i]->render();
	}
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		targets[i]->render();
	}
	for (unsigned int i = 0; i < bads.size(); i++)
	{
		bads[i]->render();
	}
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		characters[i]->render();
	}
}
void Level::destroy()
{
	for (unsigned int i = 0; i < walls.size(); i++)
	{
		delete walls[i];
	}
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		delete targets[i];
	}
	for (unsigned int i = 0; i < bads.size(); i++)
	{
		delete bads[i];
	}
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		delete characters[i];
	}
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		delete spaces[i];
	}
	walls.clear();
	targets.clear();
    bads.clear();
    characters.clear();
    spaces.clear();
    Coffee::count = 0;
}

int main(int, char**)
{
	SDL_Event event;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {cerr << "Failed to initialize graphics: " << SDL_GetError() << endl; return 0;}
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {cerr << "Failed to initialize double buffering: " << SDL_GetError() << endl; return 0;}
    if(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1) < 0) {cerr << "Failed to initialize hardware acceleration: " << SDL_GetError() << endl; return 0;}
    if(SDL_SetVideoMode(1024, 1024, 32, SDL_RESIZABLE | SDL_OPENGL) == NULL) {cerr << "Failed to initialize screen: " << SDL_GetError() << endl; return 0;}
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1024, 1024, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {cerr << "Failed to initialize OpenGL: " << gluErrorString(error); return 0;}
    SDL_GL_SwapBuffers();
    TIME = SDL_GetTicks();
    while(QUIT == false)
	{
		if (REPEAT_LEVEL and (PAUSE == 0))
		{
			current.initialize(current.current);
			REPEAT_LEVEL = false;
		}
		if (NEXT_LEVEL and (PAUSE == 0))
		{
			current.initialize(current.next);
			NEXT_LEVEL = false;
		}
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT) {QUIT = true;}
			/*else if(event.type == SDL_VIDEORESIZE)
			{
				if(SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_RESIZABLE | SDL_OPENGL) == NULL) {cerr << "Failed to re-initialize screen: " << SDL_GetError() << endl; return 0;}
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0.0, event.resize.w, event.resize.h, 0.0, 1.0, -1.0);
				glMatrixMode(GL_MODELVIEW);
			}*/
			else {current.handle(event);}
		}
		if (PAUSE == 0) {current.update();}
		if (PAUSE > 0)
		{
			if((static_cast<int>(SDL_GetTicks()) - TIME) < PAUSE) {PAUSE = PAUSE - SDL_GetTicks() + TIME;}
			else {PAUSE = 0;}
		}
		TIME = SDL_GetTicks();
		current.render();
		SDL_GL_SwapBuffers();
		if (((REPEAT_LEVEL or NEXT_LEVEL) and (PAUSE == 0)) or QUIT)
		{
			current.destroy();
		}
	}
	return 1;
}
