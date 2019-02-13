/**
 * @file Arc.hpp
 * File with Arc
 * @author GenaBitu
 **/

#ifndef ARC_HPP
#define ARC_HPP
#include "Libs.hpp"
class Space;

class Arc
{
    public:
        Arc(Space* space);
        void add(Space* space);
        void recomputeDistance(int startDist, Space* from);
        std::vector<Space*> spaces;
};

#endif // ARC_HPP
