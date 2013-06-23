/**
 * @file Menu.hpp
 * File with Menu
 * @author GenaBitu
 **/

#ifndef MENU_HPP
#define MENU_HPP
#include "Libs.hpp"
class GLTexture;
class MenuItem;

enum menuType {MAIN, SETTINGS, ESCAPE};
class Menu
{
    public:
     menuType type;
     Menu(menuType srcType);
     bool handle(SDL_Event inputEvent);
     void render() const;
     void resize();
    private:
     GLTexture button;
     GLfloat xMin;
     GLfloat xMax;
     GLfloat yMin;
     GLfloat yMax;
     std::vector<MenuItem*> items;
};

#endif //MENU_HPP
