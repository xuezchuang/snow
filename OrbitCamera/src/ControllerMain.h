///////////////////////////////////////////////////////////////////////////////
// ControllerMain.h
// ================
// Derived Controller class for main window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-28
// UPDATED: 2016-05-28
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_MAIN_H
#define WIN_CONTROLLER_MAIN_H

#include "Controller.h"

namespace Win
{
    class ControllerMain :public Controller
    {
    public:
        ControllerMain();
        ~ControllerMain() {};

        void setGLHandles(HWND h1, HWND h2)         { glHandle1 = h1; glHandle2 = h2; }
        void setFormHandle(HWND handle)             { formHandle = handle; }

        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
        int close();                                // for WM_CLOSE
        int create();                               // for WM_CRERATE
        int destroy();                              // for WM_DESTROY
        int size(int w, int h, WPARAM wParam);      // for WM_SIZE: width, height, type(SIZE_MAXIMIZED...)
        virtual int keyDown(int key, LPARAM lParam);
    private:
        HWND glHandle1;                             // handle of gl window #1
        HWND glHandle2;                             // handle of gl window #2
        HWND formHandle;                            // handle of form dialog

    };
}

#endif
