///////////////////////////////////////////////////////////////////////////////
// ControllerGL2.cpp
// =================
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

#include <string>
#include <sstream>
#include "ControllerGL2.h"
#include "wcharUtil.h"
#include "Log.h"
using namespace Win;



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerGL2::ControllerGL2(ModelGL* model, ViewGL* view)
    : model(model), view(view), hglrc(0), pixelFormat(0), mouseHovered(false)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_DESTROY
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::destroy()
{
    ::wglMakeCurrent(0, 0);
    ::ReleaseDC(handle, view->getDC());
    Win::log("OpenGL window is destroyed for screen 2.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::create()
{
    // set DC only, and use RC from view 1
	bool result = view->setContext(handle, hglrc, pixelFormat);
    if(result)
        Win::log("Created OpenGL window for screen 2.");
    else
        Win::log("[ERROR] Failed to create OpenGL window for screen 2.");

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_PAINT
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::paint()
{
    // redraw
    view->activateContext(); // make it active first
    model->draw(2);
    view->swapBuffers();
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::size(int w, int h, WPARAM wParam)
{
    model->setWindowSize(w, h);
    paint();    // repaint glWindow
    Win::log(L"Changed OpenGL rendering window size: %dx%d for screen 2.", w, h);
    return 0;
}


int ControllerGL2::mouseMove(WPARAM state, int x, int y)
{
	// for tracking mouse hover/leave tracking
	if (!mouseHovered)
	{
		// mouse hover/leave tracking
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.hwndTrack = handle;
		tme.dwHoverTime = HOVER_DEFAULT;
		::TrackMouseEvent(&tme);
		mouseHovered = true;
	}

	if (state == MK_LBUTTON)
	{
		model->rotateCamera2(x, y);
		paint();
		m_ControllerGL1->paint();
		ViewForm->updateTrackbars();
	}
	//if (state == MK_RBUTTON)
	//{
	//	//model->zoomCamera(y);
	//	model->setOff(x, y);
	//	paint();
	//}
	if (state == MK_MBUTTON)
	{
		model->DragScreen(x, y);
		paint();
		m_ControllerGL1->paint();
		ViewForm->updateTrackbars();
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEHOVER
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::mouseHover(int state, int x, int y)
{
	mouseHovered = true;
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSELEAVE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::mouseLeave()
{
	mouseHovered = false;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle Left mouse down
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::lButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	model->setMousePosition2(x, y);

	if (state == MK_LBUTTON)
	{
		model->setMouseLeft2(true);
	}

	// set focus to receive wm_mousewheel event
	::SetFocus(handle);
	return 0;
}

int ControllerGL2::mButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	model->setMousePosition2(x, y);

	if (state == MK_MBUTTON)
	{
		model->setMouseLeft2(true);
	}

	// set focus to receive wm_mousewheel event
	::SetFocus(handle);
	return 0;
}

int ControllerGL2::keyDown(int key, LPARAM lParam)
{
	if(key == 'b')
		model->setNewShader(true);
	else
		model->setNewShader(false);
	paint();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::lButtonUp(WPARAM state, int x, int y)
{
	// update mouse position
	model->setMousePosition2(x, y);
	model->setMouseLeft2(false);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEWHEEL
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::mouseWheel(int state, int delta, int x, int y)
{
	if (mouseHovered)
	{
		model->zoomCameraDelta2(delta / 120.0f);
		paint();
		m_ControllerGL1->paint();
		ViewForm->updateTrackbars();
	}
	return 0;
}