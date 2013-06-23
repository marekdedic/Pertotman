#include "Pertotman.hpp"
using namespace std;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 1024;
int DELTA = 0;
int PAUSE = 0;
int DIFFICULTY = 3;
int FRAMERATE = 120;
bool QUIT = false;
bool REPEAT_LEVEL = false;
bool NEXT_LEVEL = false;
Level* CURRENT;

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
void GLTexture::process(SDL_Surface* image)
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
    if(error != GL_NO_ERROR) {cerr << "Failed to convert image: " << gluErrorString(error) << endl; QUIT = true;}
}
void GLTexture::loadIMG(string filename)
{
	SDL_Surface* image = NULL;
    image = IMG_Load(filename.c_str());
    if(image == NULL) {cerr << "Failed to load image: " << filename << endl; QUIT = true;}
    else{process(image);}
}
void GLTexture::loadText(string text, string filename, int size, SDL_Color color)
{
    TTF_Font* font = TTF_OpenFont(filename.c_str(), size);
    SDL_Surface* image = TTF_RenderText_Blended(font, text.c_str(), color);
    process(image);
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
    textures.w = 512;
    textures.h = 64;
    string cap;
    switch(type)
    {
        case NEW:
         cap = "New Game";
         break;
        case CLOSE:
         cap = "Back to Game";
         break;
        case SAVE:
         cap = "Save";
         break;
        case LOAD:
         cap = "Load";
         break;
        case SET:
         cap = "Settings";
         break;
        case BACK:
         cap = "Back to Main Menu";
         break;
        case SHUT:
         cap = "Quit";
         break;
        case FPS:
         switch (FRAMERATE)
         {
            case 30:
             cap = "Max. Framerate: 30";
             break;
            case 60:
             cap = "Max. Framerate: 60";
             break;
            case 120:
             cap = "Max. Framerate: 120";
             break;
            default:
             cap = "Max. Framerate: Inf";
             break;
         }
         break;
        case CLOSESET:
         cap = "Close Settings";
         break;
        case DIF:
         switch (DIFFICULTY)
         {
            case 1:
             cap = "Difficulty: Easy";
             break;
            case 2:
             cap = "Difficulty: Medium";
             break;
            default:
             cap = "Difficulty: Hard";
             break;
         }
         break;
        default:
         break;
    }
    caption.loadText(cap, "04B_30.ttf", 32, {0, 0, 0, 255});
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
    string cap;
    bool state = true;
    switch(type)
    {
        case NEW:
        case CLOSE:
        case CLOSESET:
         state = false;
         break;
        case SAVE:
         CURRENT->save();
         caption.loadText("Successful!", "04B_30.ttf", 32, {0, 0, 0, 255});
         break;
        case LOAD:
         CURRENT->destroy();
         CURRENT->load();
         break;
        case SET:
         CURRENT->menus.push_back(new Menu(SETTINGS));
         break;
        case BACK:
         CURRENT->levelCounter = 0;
         CURRENT->next = "NULL";
         NEXT_LEVEL = true;
         state = false;
         break;
        case SHUT:
         QUIT = true;
         break;
        case FPS:
         switch (FRAMERATE)
         {
            case 0:
             FRAMERATE = 30;
             cap = "Max. Framerate: 30";
             break;
            case 30:
             FRAMERATE = 60;
             cap = "Max. Framerate: 60";
             break;
            case 60:
             FRAMERATE = 120;
             cap = "Max. Framerate: 120";
             break;
            default:
             FRAMERATE = 0;
             cap = "Max. Framerate: Inf";
             break;
         }
         caption.loadText(cap, "04B_30.ttf", 32, {0, 0, 0, 255});
         break;
        case DIF:
         switch (DIFFICULTY)
         {
            case 1:
             DIFFICULTY = 2;
             cap = "Difficulty: Medium";
             break;
            case 2:
             DIFFICULTY = 3;
             cap = "Difficulty: Hard";
             break;
            default:
             DIFFICULTY = 1;
             cap = "Difficulty: Easy";
             break;
         }
         caption.loadText(cap, "04B_30.ttf", 32, {0, 0, 0, 255});
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
     glTexCoord2f(caption.x / caption.w, caption.y / caption.h); glVertex2f(x + ((textures.w - caption.w) / 2), y + ((textures.h - caption.h) / 2));
     glTexCoord2f((caption.x + caption.w) / caption.w, caption.y / caption.h); glVertex2f(x + ((textures.w + caption.w) / 2), y + ((textures.h - caption.h) / 2));
     glTexCoord2f((caption.x + caption.w) / caption.w, (caption.y + caption.h) / caption.h); glVertex2f(x + ((textures.w + caption.w) / 2), y + ((textures.h + caption.h) / 2));
     glTexCoord2f(caption.x / caption.w, (caption.y + caption.h) / caption.h); glVertex2f(x + ((textures.w - caption.w) / 2), y + ((textures.h + caption.h) / 2));
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

Menu::Menu(menuType srcType) : type(srcType), xMin((SCREEN_WIDTH / 2) - 384), xMax((SCREEN_WIDTH / 2) + 384), yMin(32), yMax(SCREEN_HEIGHT - 32)
{
    GLfloat yBuff = yMin + 32;
    button.loadIMG("button.png");
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
         items.push_back(new MenuItem(xMin + 128, yBuff, button, DIF));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, FPS));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, CLOSESET));
         yBuff += 96;
         break;
        case ESCAPE:
         items.push_back(new MenuItem(xMin + 128, yBuff, button, CLOSE));
         yBuff += 96;
         items.push_back(new MenuItem(xMin + 128, yBuff, button, SAVE));
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
void Space::neighbours(Level* thisLevel)
{
	for(unsigned int i = 0; i < thisLevel->spaces.size(); i++)
	{
		if((thisLevel->spaces[i]->x == x) and (thisLevel->spaces[i]->y == (y + 32))) {down = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->x == x) and (thisLevel->spaces[i]->y == (y - 32))) {up = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->y == y) and (thisLevel->spaces[i]->x == (x + 32))) {right = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->y == y) and (thisLevel->spaces[i]->x == (x - 32))) {left = thisLevel->spaces[i];}
	}
}
void Space::compute(GLint ThisDist, char from = 'a')
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

const GLint Enemy::velocity = 75;
Enemy::Enemy(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures)
{
    textures.x = 0;
	textures.y = 32;
}
void Enemy::update()
{
    /*switch(DIFFICULTY)
    {
        case 1:
         path1();
         break;
        case 2:
         path2();
         break;
        default:*/
         path3();
         /*break;
    }*/
	GLfloat MoveBuff = velocity * (DELTA / 1000.f);
	if (MoveBuff > 0)
    {
        for(unsigned int i = 0; i < 3; i++)
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
void Enemy::path1()
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
void Enemy::path2()
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
void Enemy::path3()
{
	Space* target = nullptr;
	Space* start = nullptr;
	for (unsigned int i = 0; i < CURRENT->spaces.size(); i++)
	{
		if((CURRENT->spaces[i]->y == RoundTo(y, 32)) and (CURRENT->spaces[i]->x == RoundTo(x, 32))) {start = CURRENT->spaces[i];}
        if((CURRENT->spaces[i]->x == RoundTo(CURRENT->pacmans[0]->x, 32)) and (CURRENT->spaces[i]->y == RoundTo(CURRENT->pacmans[0]->y, 32))) {target = CURRENT->spaces[i];}
	}
	if((start == nullptr) or (target == nullptr)) {cerr << "Pathfinder error. Deactivated Enemy." << endl; return;}
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

unsigned int Level::levelCounter = 0;
string Level::current = "NULL";
string Level::next = "NULL";
Level::Level(string name)
{
    init(name, true);
}
void Level::init(string name, bool full)
{
	if (name.empty() or (name == "NULL"))
    {
        current = "level1";
        levelCounter = 0;
        menus.push_back(new Menu(MAIN));
    }
    else {current = name;}
    stringstream ss;
    GLfloat x;
    GLfloat y = 0;
    fstream file(("levels/" + current + ".lvl").c_str(), fstream::in);
    string caption;
    if(!REPEAT_LEVEL) {levelCounter++;}
    if(!file.is_open()) {cerr << "Error with loading file: " << strerror(errno) << endl; QUIT = true; return;}
    textures.loadIMG("textures.png");
    getline(file, caption);
    next = caption;
    getline(file, caption);
    ss << "Pertotman, Level " << levelCounter << ": " << caption;
    caption = ss.str();
    SDL_WM_SetCaption(caption.c_str(), "Pertotman");
    for(unsigned int counter = 1; file.eof() == false; counter++)
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
                 if(full){coffees.push_back(new Coffee(x, y, textures));}
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 'b':
                case 'B':
                 if(full){enemies.push_back(new Enemy(x, y, textures));}
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 'p':
                case 'P':
                 if(full){pacmans.push_back(new Pacman(x, y, textures));}
                 spaces.push_back(new Space(x, y, textures));
                 break;
				case 's':
                case 'S':
                 spaces.push_back(new Space(x, y, textures));
                 break;
				default:
                 cerr << "Error with loading level: Invalid character." << endl; QUIT = true; return;
                 break;
			}
			x += 32;
		}
		y += 32;
	}
	for (unsigned int i = 0; i < spaces.size(); i++)
	{
		spaces[i]->neighbours(this);
    }
	if (full and(pacmans.size() != 1)) {cerr << "Error with loading level: No, or too many pacman found." << endl; QUIT = true; return;}
}
void Level::save() const
{
    size_t unit = sizeof(GLfloat);
    size_t sizeTemp;
    fstream file("savegame.sav", fstream::out | fstream::binary | fstream::trunc);
    if(!file.is_open()) {cerr << "Error with opening file: " << strerror(errno) << endl; QUIT = true; return;}
    file.write(reinterpret_cast<char*>(&unit), sizeof(size_t));
    sizeTemp = sizeof(int);
    file.write(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&levelCounter), sizeTemp);
    sizeTemp = current.size();
    file.write(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    file.write(current.c_str(), sizeTemp);
    sizeTemp = pacmans.size();
    file.write(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < pacmans.size(); i++)
        {
            file.write(reinterpret_cast<char*>(&pacmans[i]->x), unit);
            file.write(reinterpret_cast<char*>(&pacmans[i]->y), unit);
        }
    sizeTemp = coffees.size();
    for (unsigned int i = 0; i < coffees.size(); i++)
    {
        if(!coffees[i]->armed){sizeTemp--;}
    }
    file.write(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < coffees.size(); i++)
        {
            if(coffees[i]->armed)
            {
                file.write(reinterpret_cast<char*>(&coffees[i]->x), unit);
                file.write(reinterpret_cast<char*>(&coffees[i]->y), unit);
            }
        }
    sizeTemp = enemies.size();
    file.write(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < enemies.size(); i++)
        {
            file.write(reinterpret_cast<char*>(&enemies[i]->x), unit);
            file.write(reinterpret_cast<char*>(&enemies[i]->y), unit);
        }
}
void Level::load()
{
    size_t unit = sizeof(GLfloat);
    size_t sizeTemp;
    char lvlName[50];
    fstream file("savegame.sav", fstream::in | fstream::binary);
    if(!file.is_open())
    {
        levelCounter = 0;
        init("level1", true);
        return;
    }
    file.read(reinterpret_cast<char*>(&unit), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&levelCounter), sizeTemp);
    levelCounter--;
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    file.read(lvlName, sizeTemp);
    lvlName[sizeTemp] = 0;
    init(lvlName, false);
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < sizeTemp; i++)
        {
            pacmans.push_back(new Pacman(0, 0, textures));
            file.read(reinterpret_cast<char*>(&pacmans[i]->x), unit);
            file.read(reinterpret_cast<char*>(&pacmans[i]->y), unit);
        }
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < sizeTemp; i++)
        {
            coffees.push_back(new Coffee(0, 0, textures));
            file.read(reinterpret_cast<char*>(&coffees[i]->x), unit);
            file.read(reinterpret_cast<char*>(&coffees[i]->y), unit);
        }
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < sizeTemp; i++)
        {
            enemies.push_back(new Enemy(0, 0, textures));
            file.read(reinterpret_cast<char*>(&enemies[i]->x), unit);
            file.read(reinterpret_cast<char*>(&enemies[i]->y), unit);
        }
    menus.push_back(new Menu(ESCAPE));
}
void Level::handle(SDL_Event inputEvent)
{
    if((inputEvent.type == SDL_KEYDOWN) and (inputEvent.key.keysym.sym == SDLK_ESCAPE))
    {
        if(menus.empty()) {menus.push_back(new Menu(ESCAPE));}
        else {menus.pop_back();}
    }
    else
    {
        if(menus.size() > 0)
        {
            if(!menus[menus.size() - 1]->handle(inputEvent)) {menus.pop_back();}
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
        for (unsigned int i = 0; i < enemies.size(); i++)
        {
            enemies[i]->update();
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
	bool bgrender = true;
	if(menus.size() > 0)
    {
        if(menus[0]->type == MAIN) {bgrender = false;}
    }
	if (bgrender)
    {
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
        for (unsigned int i = 0; i < enemies.size(); i++)
        {
            enemies[i]->render();
        }
        for (unsigned int i = 0; i < pacmans.size(); i++)
        {
            pacmans[i]->render();
        }
    }
	if(menus.size() > 0) {menus[menus.size() - 1]->render();}
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
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
	    enemies.erase(enemies.begin() + i);
	}
	for (unsigned int i = 0; i < pacmans.size(); i++)
	{
	    pacmans.erase(pacmans.begin() + i);
	}
	menus.clear();
	spaces.clear();
	walls.clear();
	coffees.clear();
    enemies.clear();
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
    if(TTF_Init() < 0) {cerr << "Failed to initialize SDL_ttf." << endl; return 0;}
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
    if(gerror != GL_NO_ERROR) {cerr << "Failed to initialize OpenGL: " << gluErrorString(gerror) << endl; return 0;}
    SDL_GL_SwapBuffers();
    time = SDL_GetTicks();
    srand(SDL_GetTicks());
    CURRENT = new Level("NULL");
    while(QUIT == false)
	{
		if (REPEAT_LEVEL and (PAUSE == 0))
		{
			CURRENT = new Level(CURRENT->current);
			REPEAT_LEVEL = false;
		}
		if (NEXT_LEVEL and (PAUSE == 0))
		{
			CURRENT = new Level(CURRENT->next);
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
		if(FRAMERATE > 0)
        {
            int sleep = (1000 / FRAMERATE) - SDL_GetTicks() + time;
            if(sleep > 0){SDL_Delay(sleep);}
        }
        DELTA = (SDL_GetTicks() - time);
        time = SDL_GetTicks();
		CURRENT->render();
		SDL_GL_SwapBuffers();
		if ((REPEAT_LEVEL or NEXT_LEVEL) and (PAUSE == 0))
		{
		    CURRENT->destroy();
			delete CURRENT;
		}
//        cout << 1000.f / DELTA << endl;           //FPS
	}
	CURRENT->destroy();
	delete CURRENT;
	return 1;
}
