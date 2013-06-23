#include "Main.hpp"
using namespace std;

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
	if (full and(pacmans.size() != 1)) {cerr << "Error with loading level: Too many pacman instances." << endl; QUIT = true; return;}
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
    GLfloat xTemp, yTemp;
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
            file.read(reinterpret_cast<char*>(&xTemp), unit);
            file.read(reinterpret_cast<char*>(&yTemp), unit);
            pacmans.push_back(new Pacman(xTemp, yTemp, textures));
        }
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < sizeTemp; i++)
        {
            file.read(reinterpret_cast<char*>(&xTemp), unit);
            file.read(reinterpret_cast<char*>(&yTemp), unit);
            coffees.push_back(new Coffee(xTemp, yTemp, textures));
        }
    file.read(reinterpret_cast<char*>(&sizeTemp), sizeof(size_t));
    for (unsigned int i = 0; i < sizeTemp; i++)
        {
            file.read(reinterpret_cast<char*>(&xTemp), unit);
            file.read(reinterpret_cast<char*>(&yTemp), unit);
            enemies.push_back(new Enemy(xTemp, yTemp, textures));
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
