#pragma once

#include "HVector.hpp"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

typedef HVector<GLdouble, 3>	TVector3GLd;

typedef HVector<GLint, 4>		TVector4GLi;

typedef tagTMatrix4<GLdouble>	TMatrix4GLd;

typedef struct  tagViewMatInfo {

	TVector4GLi viewport;//Viewport
	TMatrix4GLd mmv;//Model Matrix
	TMatrix4GLd mp;	//Proj Matrix

	tagViewMatInfo& operator=(const tagViewMatInfo& src)
	{
		viewport = src.viewport;
		mmv = src.mmv;
		mp = src.mp;
		return *this;
	}

} TViewMatInfo;


