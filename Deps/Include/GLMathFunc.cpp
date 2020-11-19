#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif
#include <math.h>
#include "GLMathFunc.h"
#include "AriesMathf.h"



BOOL GLProject(const TViewMatInfo* pViewMatInfo, const GLdouble obj[3], GLdouble win[3])
{
	return gluProject(obj[0], obj[1], obj[2], pViewMatInfo->mmv._mat,pViewMatInfo->mp._mat, pViewMatInfo->viewport.vec, &win[0], &win[1], &win[2]);
}

BOOL GLProject(const TViewMatInfo* pViewMatInfo, const GLdouble obj[3], int& x, int& y)
{
	GLdouble win[3];
	if(GLProject(pViewMatInfo, obj, win))
	{
			x = int(win[0]);	y = int(win[1]);
			return TRUE;
	}
	return FALSE;
}

BOOL GLUnProject(const TViewMatInfo* pViewMatInfo, const GLdouble win[3], GLdouble obj[3])
{
	return gluUnProject(win[0], win[1], win[2], pViewMatInfo->mmv._mat,pViewMatInfo->mp._mat, pViewMatInfo->viewport.vec, &obj[0], &obj[1], &obj[2]);
}

BOOL GLUnProject(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble obj[3])
{
	const GLdouble win[3] = { (GLdouble)x, (GLdouble)y, 0.f };
	return GLUnProject(pViewMatInfo, win, obj);	
}

void GLPullToScreen(const TViewMatInfo* pViewMatInfo, double dPullLength, GLdouble obj[3])
{
	register GLdouble win[3];	
	GLProject(pViewMatInfo, obj, win);
	win[2] -= dPullLength;
	GLUnProject(pViewMatInfo, win, obj);		
}

void GLGetScreenNorm(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble norm[3])
{	
	TVector3GLd win(x, y, 0.f);
	GLdouble oPnt1[3], oPnt2[3];
	GLUnProject( pViewMatInfo, win.vec, oPnt1 );
	win.vec[2] = 4.0;
	GLUnProject( pViewMatInfo, win.vec, oPnt2 );
	Aries_TmplMath::getSub(oPnt2, oPnt1, norm);
	Aries_TmplMath::getNormalize(norm);
}

void GLGetScreenX(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble xDir[3])
{
    GLdouble norm[3] = {0};
    GLGetScreenNorm(pViewMatInfo, x, y, norm);

    GLdouble yDir[3] = {0};
    GLGetScreenY(pViewMatInfo, x, y, yDir);

    //x dir
	Aries_TmplMath::getCross(yDir, norm, xDir);
	Aries_TmplMath::getNormalize(xDir);

}

void GLGetScreenY(const TViewMatInfo* pViewMatInfo, int x, int y, GLdouble yDir[3])
{
    //normal
    GLdouble norm[3] = {0};
    GLGetScreenNorm(pViewMatInfo, x, y, norm);

    //imm precise x dir
    GLdouble xDir[3] = {0};
    TVector3GLd win(x, y, 0.f);
    GLdouble oPnt1[3], oPnt2[3];
    GLUnProject( pViewMatInfo, win.vec, oPnt1 );
    win.vec[0] = x + 123.f;
    GLUnProject( pViewMatInfo, win.vec, oPnt2 );
	Aries_TmplMath::getSub(oPnt2, oPnt1, xDir);
	Aries_TmplMath::getNormalize(xDir);

    //y dir
	Aries_TmplMath::getCross(norm, xDir, yDir);
	Aries_TmplMath::getNormalize(yDir);
}
