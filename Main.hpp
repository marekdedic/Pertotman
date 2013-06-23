/**
 * @file Main.hpp
 * File with Main
 * @author GenaBitu
 **/

#ifndef MAIN_HPP
#define MAIN_HPP

#include "Libs.hpp"
#include "GLTexture.hpp"
#include "RendObj.hpp"
#include "Coffee.hpp"
#include "Enemy.hpp"
#include "Level.hpp"
#include "Menu.hpp"
#include "MenuItem.hpp"
#include "Pacman.hpp"
#include "Space.hpp"
#include "Wall.hpp"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int DELTA;
extern int PAUSE;
extern int FRAMERATE;
extern bool QUIT;
extern bool REPEAT_LEVEL;
extern bool NEXT_LEVEL;
extern Level* CURRENT;

GLint RoundTo(GLfloat, GLint);

#endif //MAIN_HPP
