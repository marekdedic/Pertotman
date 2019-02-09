#include "Main.hpp"
using namespace std;

Space::Space(GLfloat xIn, GLfloat yIn, GLTexture srcTextures) : RendObj(xIn, yIn, srcTextures), up(nullptr), down(nullptr), left(nullptr), right(nullptr), dist(-1)
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
void Space::computeGraph(std::vector<Space*> endpoints, Arc* currentArc)
{
    if(arcs.size() > 0)
    {
        return;
    }
    unsigned int neighbours{0};
    if(up != nullptr) {++neighbours;}
    if(down != nullptr) {++neighbours;}
    if(left != nullptr) {++neighbours;}
    if(right != nullptr) {++neighbours;}
    if(neighbours > 2 or (std::find(endpoints.begin(), endpoints.end(), this) != endpoints.end()))
    {
        arcs.push_back(currentArc);
        currentArc->add(this);
        currentArc = new Arc{this};
    }
    arcs.push_back(new Arc{this});
    currentArc->add(this);
    if(up != nullptr) {up->computeGraph(endpoints, currentArc);}
    if(down != nullptr) {down->computeGraph(endpoints, currentArc);}
    if(left != nullptr) {left->computeGraph(endpoints, currentArc);}
    if(right != nullptr) {right->computeGraph(endpoints, currentArc);}
}
void Space::compute(GLint ThisDist, char from)
{
	if((dist == -1) or (ThisDist < dist))
    {
        dist = ThisDist;
        if((up != nullptr) and (from != 'u')) {up->compute(ThisDist + 1, 'd');}
        if((down != nullptr) and (from != 'd')) {down->compute(ThisDist + 1, 'u');}
        if((left != nullptr) and (from != 'l')) {left->compute(ThisDist + 1, 'r');}
        if((right != nullptr) and (from != 'r')) {right->compute(ThisDist + 1, 'l');}
    }
}
