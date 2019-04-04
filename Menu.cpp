#include "Main.hpp"
using namespace std;

Menu::Menu(menuType srcType) : type(srcType), xMin((SCREEN_WIDTH / 2) - 384), xMax((SCREEN_WIDTH / 2) + 384), yMin(32), yMax(SCREEN_HEIGHT - 32)
{
    GLfloat yBuff = yMin + 32;
    button.loadIMG("assets/button.png");
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
         items.push_back(new MenuItem(xMin + 128, yBuff, button, DIF));
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
void Menu::resize()
{
    xMin = (SCREEN_WIDTH / 2) - 384;
    xMax = (SCREEN_WIDTH / 2) + 384;
    yMin = 32;
    yMax = SCREEN_HEIGHT - 32;
    for(unsigned int i = 0; i < items.size(); i++)
    {
        items[i]->x = xMin + 128;
    }
}
