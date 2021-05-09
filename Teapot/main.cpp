///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// 3D drawing for quaternion
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2011-12-09
// UPDATED: 2016-04-05
///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
//#include <sstream>
//#include <string>
//#include <iomanip>
//#include <vector>
//#include "Vectors.h"
//#include "Matrices.h"
//#include "Quaternion.h"
//#include "Trackball.h"
//#include "animUtils.h"

#include <vld.h>
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    int* a = new int(4);

    return 0;
}
//
//
/////////////////////////////////////////////////////////////////////////////////
//// initialize GLUT for windowing
/////////////////////////////////////////////////////////////////////////////////
//int initGLUT(int argc, char **argv)
//{
//    // GLUT stuff for windowing
//    // initialization openGL window.
//    // It must be called before any other GLUT routine.
//    glutInit(&argc, argv);
//
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode
//
//    glutInitWindowSize(screenWidth, screenHeight);              // window size
//
//    glutInitWindowPosition(100, 100);                           // window location
//
//    // finally, create a window with openGL context
//    // Window will not displayed until glutMainLoop() is called
//    // It returns a unique ID.
//    int handle = glutCreateWindow(argv[0]);     // param is the title of window
//
//    // register GLUT callback functions
//    glutDisplayFunc(displayCB);
//    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
//    //glutIdleFunc(idleCB);                       // redraw whenever system is idle
//    glutReshapeFunc(reshapeCB);
//    glutKeyboardFunc(keyboardCB);
//    glutMouseFunc(mouseCB);
//    glutMotionFunc(mouseMotionCB);
//    glutPassiveMotionFunc(mousePassiveMotionCB);
//
//    return handle;
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// initialize OpenGL
//// disable unused features
/////////////////////////////////////////////////////////////////////////////////
//void initGL()
//{
//    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
//
//    // enable /disable features
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_BLEND);
//
//    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
//    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//    //glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
//    glEnable(GL_COLOR_MATERIAL);
//
//    glClearColor(0, 0, 0, 0);                   // background color
//    glClearStencil(0);                          // clear stencil buffer
//    glClearDepth(1.0f);                         // 0 is near, 1 is far
//    glDepthFunc(GL_LEQUAL);
//
//    initLights();
//
//    float white[] = {1,1,1,1};
//    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// write 2d text using GLUT
//// The projection matrix must be set to orthogonal before call this function.
/////////////////////////////////////////////////////////////////////////////////
//void drawString(const char *str, int x, int y, float color[4], void *font)
//{
//    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
//    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
//    glDisable(GL_TEXTURE_2D);
//    glDepthFunc(GL_ALWAYS);
//
//    glColor4fv(color);          // set text color
//    glRasterPos2i(x, y);        // place text position
//
//    // loop all characters in the string
//    while(*str)
//    {
//        glutBitmapCharacter(font, *str);
//        ++str;
//    }
//
//    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_LIGHTING);
//    glDepthFunc(GL_LEQUAL);
//    glPopAttrib();
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// draw a string in 3D space
/////////////////////////////////////////////////////////////////////////////////
//void drawString3D(const char *str, float pos[3], float color[4], void *font)
//{
//    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
//    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
//    glDisable(GL_TEXTURE_2D);
//
//    glColor4fv(color);          // set text color
//    glRasterPos3fv(pos);        // place text position
//
//    // loop all characters in the string
//    while(*str)
//    {
//        glutBitmapCharacter(font, *str);
//        ++str;
//    }
//
//    glDisable(GL_TEXTURE_2D);
//    glEnable(GL_LIGHTING);
//    glPopAttrib();
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// initialize global variables
/////////////////////////////////////////////////////////////////////////////////
//bool initSharedMem()
//{
//    screenWidth = SCREEN_WIDTH;
//    screenHeight = SCREEN_HEIGHT;
//
//    // set the trackball
//    if(screenWidth > screenHeight)
//        sphereRadius = screenHeight * RADIUS_SCALE;
//    else
//        sphereRadius = screenWidth * RADIUS_SCALE;
//
//    trackball.set(sphereRadius, screenWidth, screenHeight);
//    //trackball.setMode(Trackball::PROJECT);
//    //trackball.setMode(Trackball::ARC);
//    quat.set(1, 0, 0, 0);   // init rotation quaternion
//
//    cameraDistance = sphereRadius * 3.0f;
//
//    mouseLeftDown = mouseRightDown = false;
//    mouseX = mouseY = 0;
//
//    drawMode = 0;
//
//    // build vertices of circle
//    circlePoints = buildCircle(sphereRadius, 100);
//
//    return true;
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// clean up shared memory
/////////////////////////////////////////////////////////////////////////////////
//void clearSharedMem()
//{
//    gluDeleteQuadric(sphere);
//    gluDeleteQuadric(point);
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// initialize lights
/////////////////////////////////////////////////////////////////////////////////
//void initLights()
//{
//    // set up light colors (ambient, diffuse, specular)
//    GLfloat lightKa[] = {0.0f, 0.0f, 0.0f, 1.0f};  // ambient light
//    GLfloat lightKd[] = {1.0f, 1.0f, 1.0f, 1.0f};  // diffuse light
//    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
//    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
//
//    // position the light
//    float lightPos[4] = {0, sphereRadius*3, sphereRadius*2, 1};
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//
//    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// set camera position and lookat direction
/////////////////////////////////////////////////////////////////////////////////
//void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
//{
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// display info messages
/////////////////////////////////////////////////////////////////////////////////
//void showInfo()
//{
//    // backup current model-view matrix
//    glPushMatrix();                     // save current modelview matrix
//    glLoadIdentity();                   // reset modelview matrix
//
//    // set to 2D orthogonal projection
//    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
//    glPushMatrix();                     // save current projection matrix
//    glLoadIdentity();                   // reset projection matrix
//    gluOrtho2D(0, screenWidth, 0, screenHeight);  // set to orthogonal projection
//
//    float color[4] = {1, 1, 1, 1};
//    Vector3 v1 = sphereVector;                          // first vector on sphere
//    Vector3 v2 = trackball.getVector((int)mouseX, (int)mouseY);   // second vector on sphere
//    float angle = RAD2DEG * acosf(v1.dot(v2) / (v1.length() * v2.length()));
//
//    // for print infos
//    std::stringstream ss;
//    ss << "Mouse Coords: (" << mouseX << ", " << mouseY << ")";
//    drawString(ss.str().c_str(), 2, screenHeight-TEXT_HEIGHT, color, font);
//    ss.str("");
//
//    ss << std::fixed << std::setprecision(3);
//    ss << "Point on Sphere: " << v1;
//    if(mouseLeftDown)
//        ss << " - " << v2;
//    drawString(ss.str().c_str(), 2, screenHeight-(2*TEXT_HEIGHT), color, font);
//    ss.str("");
//
//    ss << "Normalized: " << v1.normalize();
//    if(mouseLeftDown)
//        ss << " - " << v2.normalize();
//    drawString(ss.str().c_str(), 2, screenHeight-(3*TEXT_HEIGHT), color, font);
//    ss.str("");
//
//    ss << "Angle between Points: " << angle << " deg";
//    drawString(ss.str().c_str(), 2, screenHeight-(4*TEXT_HEIGHT), color, font);
//    ss.str("");
//
//    Trackball::Mode mode = trackball.getMode();
//    ss << "Mode: " << (mode == Trackball::ARC ? "ARC" : "PROJECT");
//    drawString(ss.str().c_str(), 2, screenHeight-(5*TEXT_HEIGHT), color, font);
//    ss.str("");
//
//    ss << "Press SPACE to change trackball mode.";
//    drawString(ss.str().c_str(), 2, 2, color, font);
//    ss.str("");
//
//    // unset floating format
//    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
//
//    // restore projection matrix
//    glPopMatrix();                   // restore to previous projection matrix
//
//    // restore modelview matrix
//    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
//    glPopMatrix();                   // restore to previous modelview matrix
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// set projection matrix as orthogonal
/////////////////////////////////////////////////////////////////////////////////
//void toOrtho()
//{
//    // set viewport to be the entire window
//    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
//
//    // set orthographic viewing frustum
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
//
//    // switch to modelview matrix in order to set scene
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// set the projection matrix as perspective
/////////////////////////////////////////////////////////////////////////////////
//void toPerspective()
//{
//    // set viewport to be the entire window
//    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
//
//    // set perspective viewing frustum
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0f, (float)(screenWidth)/screenHeight, 1.0f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip
//
//    // switch to modelview matrix in order to set scene
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// make points of a circle
/////////////////////////////////////////////////////////////////////////////////
//std::vector<Vector3> buildCircle(float radius, int steps)
//{
//    std::vector<Vector3> points;
//    if(steps < 2) return points;
//
//    const float PI2 = 3.141592f * 2.0f;
//    float x, y, a;
//    for(int i = 0; i <= steps; ++i)
//    {
//        a = PI2 / steps * i;
//        x = radius * cosf(a);
//        y = radius * sinf(a);
//        points.push_back(Vector3(x, y, 0));
//    }
//    return points;
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// return rotation quaternion
/////////////////////////////////////////////////////////////////////////////////
//Quaternion getRotationQuaternion(const Vector3& v1, const Vector3& v2)
//{
//    // get rotation axis
//    Vector3 v = v1.cross(v2);
//
//    // compute angle
//    float angle = acosf(v1.dot(v2));
//
//    return Quaternion(v, angle*0.5f); // return half angle for quaternion
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// return rotation axis and angle
/////////////////////////////////////////////////////////////////////////////////
//void getRotationAxisAngle(const Vector3& v1, const Vector3& v2,
//                          Vector3& axis, float& angle)
//{
//    // get rotation axis
//    axis = v1.cross(v2);
//
//    // compute angle
//    angle = acosf(v1.dot(v2));
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// compute mouse path on a sphere between 2 mouse positions
/////////////////////////////////////////////////////////////////////////////////
//void generateMousePath()
//{
//    Vector3 v1 = sphereVector;
//    Vector3 v2 = trackball.getVector((int)mouseX, (int)mouseY);
//    float alpha;
//
//    pathPoints.clear();
//    for(int i = 0; i <= PATH_COUNT; ++i)
//    {
//        alpha = (float)i / PATH_COUNT;
//        Vector3 v = Gil::slerp(v1, v2, alpha);
//        v = v.normalize() * (sphereRadius + 0.3f);
//        pathPoints.push_back(v);
//    }
//
//    /*
//    Vector3 v1 = sphereVector;
//    Vector3 v2 = trackball.getVector(mouseX, mouseY);
//    Vector3 v;
//    float alpha;
//
//    pathPoints.clear();
//    for(int i = 0; i <= PATH_COUNT; ++i)
//    {
//        alpha = (float)i / PATH_COUNT;
//        v = v1 + (v2 - v1) * alpha;
//        v = v.normalize() * sphereRadius;
//        pathPoints.push_back(v);
//    }
//    */
//}
//
//
//
//
//
////=============================================================================
//// CALLBACKS
////=============================================================================
//
//void displayCB()
//{
//    // clear framebuffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//    glPushMatrix();
//
//    // tramsform camera
//    Matrix4 mat = quat.getMatrix(); // view matrix
//    mat.transpose();
//    mat.translate(0, 0, -cameraDistance);
//    glMultMatrixf(mat.get());
//
//    draw3D();   // draw 3D sphere, cursor vector and axis
//    draw2D();   // draw 2D trackball and mapped cursor point
//
//    showInfo(); // print text
//
//    glPopMatrix();
//    glutSwapBuffers();
//}
//
//
//void reshapeCB(int width, int height)
//{
//    screenWidth = width;
//    screenHeight = height;
//    toPerspective();
//
//    // set the trackball
//    if(screenWidth > screenHeight)
//        sphereRadius = screenHeight * RADIUS_SCALE;
//    else
//        sphereRadius = screenWidth * RADIUS_SCALE;
//    trackball.set(sphereRadius, screenWidth, screenHeight);
//    cameraDistance = sphereRadius * 3.0f;
//    circlePoints = buildCircle(sphereRadius, 100);
//}
//
//
//void timerCB(int millisec)
//{
//    glutTimerFunc(millisec, timerCB, millisec);
//    glutPostRedisplay();
//}
//
//
//void idleCB()
//{
//    glutPostRedisplay();
//}
//
//
//void keyboardCB(unsigned char key, int x, int y)
//{
//    switch(key)
//    {
//    case 27: // ESCAPE
//        exit(0);
//        break;
//
//    case 'd': // switch rendering modes (fill -> wire -> point)
//    case 'D':
//        drawMode = ++drawMode % 3;
//        if(drawMode == 0)        // fill mode
//        {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//            glEnable(GL_DEPTH_TEST);
//            glEnable(GL_CULL_FACE);
//        }
//        else if(drawMode == 1)  // wireframe mode
//        {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            glDisable(GL_DEPTH_TEST);
//            glDisable(GL_CULL_FACE);
//        }
//        else                    // point mode
//        {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//            glDisable(GL_DEPTH_TEST);
//            glDisable(GL_CULL_FACE);
//        }
//        break;
//
//    case 'r':
//    case 'R':
//        // reset rotation
//        quat.set(1, 0, 0, 0);
//        break;
//
//    case ' ':
//        if(trackball.getMode() == Trackball::ARC)
//            trackball.setMode(Trackball::PROJECT);
//        else
//            trackball.setMode(Trackball::ARC);
//        break;
//
//    default:
//        ;
//    }
//}
//
//
//void mouseCB(int button, int state, int x, int y)
//{
//    mouseX = (float)x;
//    mouseY = (float)y;
//
//    if(button == GLUT_LEFT_BUTTON)
//    {
//        if(state == GLUT_DOWN)
//        {
//            mouseLeftDown = true;
//
//            // remember mouse coords and quaternion before rotation
//            prevX = (float)x;
//            prevY = (float)y;
//            prevQuat = quat;
//        }
//        else if(state == GLUT_UP)
//        {
//            mouseLeftDown = false;
//            pathPoints.clear(); // clear mouse path
//        }
//    }
//
//    else if(button == GLUT_RIGHT_BUTTON)
//    {
//        if(state == GLUT_DOWN)
//            mouseRightDown = true;
//        else if(state == GLUT_UP)
//            mouseRightDown = false;
//    }
//}
//
//
//void mouseMotionCB(int x, int y)
//{
//    if(mouseLeftDown)
//    {
//        Vector3 v1 = trackball.getUnitVector((int)prevX, (int)prevY);
//        Vector3 v2 = trackball.getUnitVector(x, y);
//
//        /*@@ rotation from origin
//        float x1 = screenWidth * 0.5f;
//        float y1 = screenHeight * 0.5f;
//        float x2 = x1 + (x - prevX);
//        float y2 = y1 + (y - prevY);
//        Vector3 v1 = trackball.getUnitVector(x1, y1);
//        Vector3 v2 = trackball.getUnitVector(x2, y2);
//        */
//
//        Quaternion delta = Quaternion::getQuaternion(v1, v2);
//        quat = delta * prevQuat;
//
//        mouseX = (float)x;
//        mouseY = (float)y;
//
//        // compute mouse path
//        generateMousePath();
//    }
//    if(mouseRightDown)
//    {
//        cameraDistance -= (y - mouseY) * (sphereRadius * 0.01f);
//        mouseY = (float)y;
//    }
//}
//
//
//void mousePassiveMotionCB(int x, int y)
//{
//    mouseX = (float)x;
//    mouseY = (float)y;
//    sphereVector = trackball.getVector((int)mouseX, (int)mouseY);
//}
//
//
//
//void exitCB()
//{
//    clearSharedMem();
//}
