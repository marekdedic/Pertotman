#include "Main.hpp"
using namespace std;

GLuint GLTexture::usedIDs = 1;
GLTexture::GLTexture(GLfloat xIn, GLfloat yIn, GLfloat wIn, GLfloat hIn) : x(xIn), y(yIn), w(wIn), h(hIn), ID(usedIDs)
{
    usedIDs++;
}
void GLTexture::process(SDL_Surface* image)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    GLenum mode = GL_RGB;
    if(image->format->BytesPerPixel == 4) {mode = GL_RGBA;}
    glTexImage2D(GL_TEXTURE_2D, 0, mode, image->w, image->h, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    w = image->w;
    h = image->h;
    SDL_FreeSurface(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {cerr << "Failed to convert image: " << gluErrorString(error) << endl; QUIT = true;}
}
void GLTexture::loadIMG(string filename)
{

	SDL_Surface* image = NULL;
    image = IMG_Load(filename.c_str());
    if(image == NULL) {cerr << "Failed to load image: " << filename << endl; QUIT = true;}
    else{process(image);}
}
void GLTexture::loadText(string text, string filename, int size, SDL_Color color)
{
    TTF_Font* font = TTF_OpenFont(filename.c_str(), size);
    SDL_Surface* image = TTF_RenderText_Blended(font, text.c_str(), color);
    process(image);
}
