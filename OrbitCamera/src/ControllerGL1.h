///////////////////////////////////////////////////////////////////////////////
// ControllerGL1.h
// ===============
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-18
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_GL_1_H
#define WIN_CONTROLLER_GL_1_H

#include "Controller.h"
#include "ViewGL.h"
#include "ModelGL.h"


namespace Win
{
    class ControllerGL1 : public Controller
    {
    public:
        ControllerGL1(ModelGL* model, ViewGL* view);
        ~ControllerGL1() {};

        int destroy();                              // for WM_DESTROY
        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
        int create();                               // create RC for OpenGL window and start new thread for rendering
        int paint();
        int lButtonDown(WPARAM state, int x, int y);
        int lButtonUp(WPARAM state, int x, int y);
        int rButtonDown(WPARAM state, int x, int y);
        int rButtonUp(WPARAM state, int x, int y);
        int mouseMove(WPARAM state, int x, int y);
        int mouseHover(int state, int x, int y);    // for WM_MOUSEHOVER:state, x, y
        int mouseLeave();                           // for WM_MOUSELEAVE
        int mouseWheel(int state, int delta, int x, int y); // for WM_MOUSEWHEEL:state, delta, x, y
        int size(int w, int h, WPARAM wParam);      // for WM_SIZE: width, height, type(SIZE_MAXIMIZED...)
        virtual int keyDown(int key, LPARAM lParam);
        HGLRC getRC()                               { return view->getRC(); }
        int   getPixelFormat()                      { return ::GetPixelFormat(view->getDC()); }

    private:
        ModelGL* model;                             // pointer to model component
        ViewGL* view;                               // pointer to view component
        bool mouseHovered;                          // mouse hover flag

    };
}

#endif
