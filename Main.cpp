#include "Main.hpp"
using namespace std;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 1024;
int DELTA = 0;
int PAUSE = 0;
int FRAMERATE = 120;
bool QUIT = false;
bool REPEAT_LEVEL = false;
bool NEXT_LEVEL = false;
Level* CURRENT;
GLint Enemy::velocity = 100;

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

int main(int, char**)
{
	SDL_Event event;
	int time;
	bool first = true;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {cerr << "Failed to initialize graphics: " << SDL_GetError() << endl; return -1;}
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {cerr << "Failed to initialize double buffering: " << SDL_GetError() << endl;}
    if(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1) < 0) {cerr << "Failed to initialize hardware acceleration: " << SDL_GetError() << endl;}
    if(SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_RESIZABLE | SDL_OPENGL) == NULL) {cerr << "Failed to initialize screen: " << SDL_GetError() << endl; return -1;}
    if(TTF_Init() < 0) {cerr << "Failed to initialize SDL_ttf." << endl; return -1;}
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
    if(gerror != GL_NO_ERROR) {cerr << "Failed to initialize OpenGL: " << gluErrorString(gerror) << endl; return -1;}
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
			else if(event.type == SDL_VIDEORESIZE)
			{
                SCREEN_WIDTH = event.resize.w;
                SCREEN_HEIGHT = event.resize.h;
				glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				for(unsigned int i = 0; i < CURRENT->menus.size(); i++)
                {
                    CURRENT->menus[i]->resize();
                }
				if(!first and CURRENT->menus.empty())
                {
                    CURRENT->menus.push_back(new Menu(ESCAPE));
                }
				first = false;
			}
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
        //cout << 1000.f / DELTA << endl;           //FPS
	}
	CURRENT->destroy();
	delete CURRENT;
	return 0;
}
