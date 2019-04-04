#include "Main.hpp"
using namespace std;

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
         switch (Enemy::velocity)
         {
            case 50:
             cap = "Difficulty: Easy";
             break;
            case 75:
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
    // TODO: Don't segfault when font loading fails
    caption.loadText(cap, "assets/04B_30.ttf", 32, {0, 0, 0, 255});
}
bool MenuItem::handleM(SDL_Event inputEvent)
{
    bool state = true;
    if((inputEvent.type == SDL_MOUSEBUTTONDOWN) and (inputEvent.button.button == SDL_BUTTON_LEFT) and (inputEvent.button.x > x) and (inputEvent.button.x < (x + 512)) and (inputEvent.button.y > y) and (inputEvent.button.y < (y + 64)))
    {
        string cap;
        switch(type)
        {
            case NEW:
            case CLOSE:
            case CLOSESET:
             state = false;
             break;
            case SAVE:
             CURRENT->save();
             // TODO: Don't segfault when font loading fails
             caption.loadText("Successful!", "assets/04B_30.ttf", 32, {0, 0, 0, 255});
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
             // TODO: Don't segfault when font loading fails
             caption.loadText(cap, "assets/04B_30.ttf", 32, {0, 0, 0, 255});
             break;
            case DIF:
             switch (Enemy::velocity)
             {
                case 50:
                 Enemy::velocity = 75;
                 cap = "Difficulty: Medium";
                 break;
                case 75:
                 Enemy::velocity = 100;
                 cap = "Difficulty: Hard";
                 break;
                default:
                 Enemy::velocity = 50;
                 cap = "Difficulty: Easy";
                 break;
             }
             // TODO: Don't segfault when font loading fails
             caption.loadText(cap, "assets/04B_30.ttf", 32, {0, 0, 0, 255});
             break;
            default:
             break;
        }
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
