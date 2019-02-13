#include "Main.hpp"
using namespace std;

Space::Space(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), up(nullptr), down(nullptr), left(nullptr), right(nullptr), dist(-1), isDissolved{true}, arcs{vector<Arc*>{}}
{
    textures.x = 0;
    textures.y = 0;
}
void Space::neighbours(Level* thisLevel)
{
	for(unsigned int i = 0; i < thisLevel->spaces.size(); i++)
	{
		if((thisLevel->spaces[i]->x == x) and (thisLevel->spaces[i]->y == (y + 32))) {down = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->x == x) and (thisLevel->spaces[i]->y == (y - 32))) {up = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->y == y) and (thisLevel->spaces[i]->x == (x + 32))) {right = thisLevel->spaces[i];}
		if((thisLevel->spaces[i]->y == y) and (thisLevel->spaces[i]->x == (x - 32))) {left = thisLevel->spaces[i];}
	}
}
void Space::continueArc(Arc* arc)
{
    vector<Space*> neighbours{vector<Space*>{}};
    if(up != nullptr) {neighbours.push_back(up);}
    if(down != nullptr) {neighbours.push_back(down);}
    if(left != nullptr) {neighbours.push_back(left);}
    if(right != nullptr) {neighbours.push_back(right);}
    arc->add(this);
    arcs.push_back(arc);
    if(std::find(CURRENT->nodes.begin(), CURRENT->nodes.end(), this) == CURRENT->nodes.end())
    {
        isDissolved = true;
        if(neighbours.size() > 0 and !neighbours[0]->isDissolved)
        {
            neighbours[0]->continueArc(arc);
        }
        if(neighbours.size() > 1 and !neighbours[1]->isDissolved)
        {
            neighbours[1]->continueArc(arc);
        }
    }
}
int Space::nodeEvaluation(Space* target)
{
    return dist + abs(x - target->x) + abs(y - target->y);
}
void Space::added_to_permanent()
{
    for(auto it: arcs)
    {
        it->recomputeDistance(dist, this);
    }
}
