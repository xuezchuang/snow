///////////////////////////////////////////////////////////////////////////////
// ControllerGL2.h
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

#ifndef WIN_CONTROLLER_GL_2_H
#define WIN_CONTROLLER_GL_2_H

#include "Controller.h"
#include "ViewGL.h"
#include "ModelGL.h"
#include "ViewForm.h"
#include "ControllerGL1.h"
namespace Win
{
    class ControllerGL2 : public Controller
    {
    public:
        ControllerGL2(ModelGL* model, ViewGL* view);
        ~ControllerGL2() {};

        void setRC(HGLRC rc, int pixelFormat)       { hglrc = rc; this->pixelFormat = pixelFormat; }

        int destroy();                              // for WM_DESTROY
        int create();                               // create RC for OpenGL window and start new thread for rendering
        int paint();
        int size(int w, int h, WPARAM wParam);      // for WM_SIZE: width, height, type(SIZE_MAXIMIZED...)
		virtual int mouseHover(int state, int x, int y);    // for WM_MOUSEHOVER:state, x, y
		virtual int mouseLeave();                           // for WM_MOUSELEAVE
        virtual int mouseMove(WPARAM state, int x, int y);
		virtual int lButtonDown(WPARAM state, int x, int y);
        virtual int mButtonDown(WPARAM state, int x, int y);
		virtual int lButtonUp(WPARAM state, int x, int y);
        virtual int mouseWheel(int state, int delta, int x, int y); // for WM_MOUSEWHEEL:state, delta, x, y
        void setViewForm(ViewForm* viewForm) { ViewForm = viewForm; }
        void setControllerGL1(ControllerGL1* GL1) { m_ControllerGL1 = GL1; }
    private:
        ViewForm* ViewForm;
        ControllerGL1* m_ControllerGL1;
        ModelGL* model;                             // pointer to model component
        ViewGL* view;                               // pointer to view component
        HGLRC hglrc;
        int pixelFormat;

        bool mouseHovered;                          // mouse hover flag
    };
}

#endif
