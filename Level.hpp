/**
 * @file Level.hpp
 * File with Level
 * @author GenaBitu
 **/

#ifndef LEVEL_HPP
#define LEVEL_HPP
#include "Libs.hpp"
class Coffee;
class Enemy;
class GLTexture;
class Menu;
class Pacman;
class Space;
class Wall;

class Level
{
    public:
     static unsigned int levelCounter;
     static std::string current;
     static std::string next;
     std::vector<Menu*> menus;
     std::vector<Space*> nodes;
     std::vector<Space*> spaces;
     std::vector<Wall*> walls;
     std::vector<Coffee*> coffees;
     std::vector<Enemy*> enemies;
     std::vector<Pacman*> pacmans;
     std::vector<Space*> permanent;
     Level(std::string name);
     void init(std::string name, bool full);
     void save() const;
     void load();
     void handle(SDL_Event inputEvent);
     void update();
     void render() const;
     void computeGraph(std::vector<Space*> endpoints, Space* start);
     void pathfind(Space* target);
     void destroy();
    private:
     GLTexture textures;
};

#endif //LEVEL_HPP
