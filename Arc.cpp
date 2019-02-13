#include "Main.hpp"
using namespace std;

Arc::Arc(Space* space) : spaces{vector<Space*>{}}
{
    spaces.push_back(space);
}

void Arc::add(Space* space)
{
    vector<Space*> neighbours{vector<Space*>{}};
    if(spaces[0]->up != nullptr) {neighbours.push_back(spaces[0]->up);}
    if(spaces[0]->down != nullptr) {neighbours.push_back(spaces[0]->down);}
    if(spaces[0]->left != nullptr) {neighbours.push_back(spaces[0]->left);}
    if(spaces[0]->right != nullptr) {neighbours.push_back(spaces[0]->right);}
    if(std::find(neighbours.begin(), neighbours.end(), space) != neighbours.end())
    {
        spaces.insert(spaces.begin(), space);
    }
    else
    {
        spaces.push_back(space);
    }
}

void Arc::recomputeDistance(int startDist, Space* from)
{
    Space* next{spaces.back()};
    if(from == next)
    {
        next = spaces.front();
    }
    if(next->dist == -1 or (next->dist > (startDist + static_cast<int>(spaces.size()) - 1)))
    {
        next->dist = startDist + spaces.size() - 1;
        next->pred = this;
    }
}
