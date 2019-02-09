/**
 * @file Space.hpp
 * File with Space
 * @author GenaBitu
 **/

#ifndef SPACE_HPP
#define SPACE_HPP
#include "Libs.hpp"
class Level;
class RendObj;

class Space: public RendObj
{
    public:
     Space* up;
     Space* down;
     Space* left;
     Space* right;
     int dist;
     Space(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     void neighbours(Level* thisLevel);
     void computeGraph(std::vector<Space*> endpoints, Arc* currentArc);
     void compute(GLint ThisDist, char from = 'a');
    private:
     std::vector<Arc*> arcs;
};

#endif //SPACE_HPP
