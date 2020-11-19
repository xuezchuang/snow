#pragma once

#include "GLTypeDef.h"


 BOOL GLProject(const TViewMatInfo* pViewMatInfo, const GLdouble obj[3], GLdouble win[3]);
 BOOL GLProject(const TViewMatInfo* pViewMatInfo, const GLdouble obj[3], int& x, int& y);
 BOOL GLUnProject(const TViewMatInfo* pViewMatInfo, const GLdouble win[3], GLdouble obj[3]);
 BOOL GLUnProject(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble obj[3]);

 void GLPullToScreen(const TViewMatInfo* pViewMatInfo, double dPullLength, GLdouble obj[3]);
//due to perspective projection, it needs exact window coord. and projection mat. norm direction is -z(for convenient)
 void GLGetScreenNorm(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble norm[3]);
 void GLGetScreenX(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble xDir[3]);
 void GLGetScreenY(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble yDir[3]);


template<typename T, typename T2, typename T3> 
void GLMultMatrix(const T Mat[16], const T2 pos[3], T3 res_pos[3])
{
	res_pos[0] = Mat[0] * pos[0] + Mat[4] * pos[1] + Mat[8] * pos[2] + Mat[12];
	res_pos[1] = Mat[1] * pos[0] + Mat[5] * pos[1] + Mat[9] * pos[2] + Mat[13];
	res_pos[2] = Mat[2] * pos[0] + Mat[6] * pos[1] + Mat[10]* pos[2] + Mat[14];
};

