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
     Arc* pred;
     bool isDissolved;
     Space(GLfloat xIn, GLfloat yIn, GLTexture srcTextures);
     void neighbours(Level* thisLevel);
     void continueArc(Arc* arc);
     int nodeEvaluation(Space* target);
     void added_to_permanent();
     std::vector<Arc*> arcs;
};

#endif //SPACE_HPP
