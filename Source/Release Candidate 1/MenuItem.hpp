/**
 * @file MenuItem.hpp
 * File with MenuItem
 * @author GenaBitu
 **/

#ifndef MENUITEM_HPP
#define MENUITEM_HPP
#include "Libs.hpp"
class RendObj;

enum menuItemType {NEW, CLOSE, SAVE, LOAD, SET, BACK, SHUT, FPS, CLOSESET, DIF};
class MenuItem: public RendObj
{
    public:
     MenuItem(GLfloat xIn, GLfloat yIn, GLTexture srcTextures, menuItemType srcType);
     bool handleM(SDL_Event inputEvent);
     void render() const;
    private:
     menuItemType type;
     GLTexture caption;
};

#endif //MENUITEM_HPP
