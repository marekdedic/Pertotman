#include "Main.hpp"
using namespace std;

Arc::Arc(Space* space)
{
    add(space);
}

void Arc::add(Space* space)
{
    spaces.push_back(space);
}
