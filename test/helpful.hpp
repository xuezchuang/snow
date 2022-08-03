#ifndef HELPFUL_H
#define HELPFUL_H

#include <sstream>
#include <string>
#include <GL/glew.h>

std::string intToStr(int a);

std::string smallIntToStr(int a);
char* textFileRead(const char *fileName);

void printGLString(const char *name, GLenum s);
void checkGlError(const char* op);


#endif
