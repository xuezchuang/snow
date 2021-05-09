//Rely on manager
#include "stdafx.h"
#include "Manager.h"

//#include "framebufferObject.h"
#include "renderbuffer.h"
//#define _TEST1
#ifdef _TEST1
static GLfloat view_rotx = 0.f, view_roty = 0.f, view_rotz = 0.f;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.f;
static GLint bLine = 1;
static GLint bLineSMOOTH = 0;
static GLint bDetpthMask = 0;
static GLint bDetpth = 0;
static GLint bReverse = 1;
static GLint bFbo = 0;
static GLuint m_fboId = 0;
static GLfloat scale = 1.0;
GLFWwindow* window = 0;
static FramebufferObject* m_pFBO = nullptr;
static Renderbuffer* m_pRB = nullptr;

static float* m_pBuffer = nullptr;

static void drawcube(GLfloat dr, GLfloat xpos, GLfloat ypos, GLfloat zpos, GLint btest)
{
	glPushMatrix();
	glTranslatef(xpos, ypos, zpos);
	glEnd();
	glBegin(GL_QUADS);    //顶面
	glNormal3f(0.0f, 1.0f, 0.0f);
	//glColor4f(1, 0.0, 0.0, 1.0);
	glVertex3f(-dr, dr, -dr);
	glVertex3f(-dr, dr, dr);
	glVertex3f(dr, dr, dr);
	glVertex3f(dr, dr, -dr);
	if (bReverse && btest)
	{
		glVertex3f(-dr, dr, -dr);
		glVertex3f(dr, dr, -dr);
		glVertex3f(dr, dr, dr);
		glVertex3f(-dr, dr, dr);

	}
	glEnd();
	glBegin(GL_QUADS);    //前面
	glNormal3f(0.0f, 0.0f, 1.0f);
	//glColor4f(0, 1.0, 0.0, 1.0);
	glVertex3f(-dr, -dr, dr);
	glVertex3f(dr, -dr, dr);
	glVertex3f(dr, dr, dr);
	glVertex3f(-dr, dr, dr);
	if (bReverse && btest)
	{
		glVertex3f(-dr, -dr, dr);
		glVertex3f(-dr, dr, dr);
		glVertex3f(dr, dr, dr);
		glVertex3f(dr, -dr, dr);
	}
	glEnd();
	glBegin(GL_QUADS);    //右面
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(dr, -dr, -dr);
	glVertex3f(dr, dr, -dr);
	glVertex3f(dr, dr, dr);
	glVertex3f(dr, -dr, dr);
	if (bReverse && btest)
	{
		glVertex3f(dr, -dr, -dr);
		glVertex3f(dr, -dr, dr);
		glVertex3f(dr, dr, dr);
		glVertex3f(dr, dr, -dr);
	}
	glEnd();
	glBegin(GL_QUADS);    //底面
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-dr, -dr, -dr);
	glVertex3f(dr, -dr, -dr);
	glVertex3f(dr, -dr, dr);
	glVertex3f(-dr, -dr, dr);
	if (bReverse && btest)
	{
		glVertex3f(-dr, -dr, -dr);
		glVertex3f(-dr, -dr, dr);
		glVertex3f(dr, -dr, dr);
		glVertex3f(dr, -dr, -dr);
	}
	glEnd();
	glBegin(GL_QUADS);    //左面
	glNormal3f(-1.0f, 0.0f, 0.0f);
	//glColor4f(0, 0.0, 1.0, 1.0);
	glVertex3f(-dr, -dr, -dr);
	glVertex3f(-dr, -dr, dr);
	glVertex3f(-dr, dr, dr);
	glVertex3f(-dr, dr, -dr);
	if (bReverse && btest)
	{
		glVertex3f(-dr, -dr, -dr);
		glVertex3f(-dr, dr, -dr);
		glVertex3f(-dr, dr, dr);
		glVertex3f(-dr, -dr, dr);
	}
	glEnd();
	glBegin(GL_QUADS);    //背面
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-dr, -dr, -dr);
	glVertex3f(-dr, dr, -dr);
	glVertex3f(dr, dr, -dr);
	glVertex3f(dr, -dr, -dr);
	if (bReverse && btest)
	{
		glVertex3f(-dr, -dr, -dr);
		glVertex3f(dr, -dr, -dr);
		glVertex3f(dr, dr, -dr);
		glVertex3f(-dr, dr, -dr);
	}
	glEnd();
	glPopMatrix();
}

void BackGroundrender()
{
	m_pFBO->Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//render background and model first


	//OutputDebugString(_T("\nBackGround Render"));

	//draw gradient background
	GLubyte colBack1[4] = { 192,192,192,255 };
	GLubyte colBack2[4] = { 92,92,92,255 };
	//m_pView->OptData()->general().colorBackGround1.color4Ub(colBack1);
	//m_pView->OptData()->general().colorBackGround2.color4Ub(colBack2);


	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	{
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glShadeModel(GL_SMOOTH);

		glBegin(GL_QUADS);
		glColor4ubv(colBack2);
		glVertex3f(1.0f, 0.0f, 0.0f);	// v5
		glVertex3f(0.0f, 0.0f, 0.0f);	// v2

		glColor4ubv(colBack1);
		glVertex3f(0.0f, 1.0f, 0.0f); // v1
		glVertex3f(1.0f, 1.0f, 0.0f);	// v

		glEnd();

		glShadeModel(GL_FLAT);
	}
	glPopAttrib();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//m_pFBO->Disable();
}


//
static void drawFBO(void)
{
	GLuint id = 0;
	m_pFBO->Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//BackGroundrender();
	glPushMatrix();
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);
	glPushAttrib(GL_ENABLE_BIT);
	{
		glColor4f(0.0, 1.0, 0.0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawcube(30, 30, 30, 30, 1);
		glColor4f(0.0, 0.0, 0.0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawcube(30, 30, 30, 30, 0);
	}
	glPopAttrib();
	{
		int arViewport[4];
		glGetIntegerv(GL_VIEWPORT, arViewport);
		if (!m_pBuffer)
		{
			//delete[] m_pBuffer;
			//m_pBuffer = NULL;
			m_pBuffer = new GLfloat[arViewport[2] * arViewport[3]];
		}
		if (m_pFBO)
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		}
		else
		{
			glReadBuffer(GL_BACK);
		}
		glReadPixels(0, 0, arViewport[2], arViewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)m_pBuffer);
	}
	glPopMatrix();
	m_pFBO->Disable();
	
}


/* OpenGL draw function & timing */
static void draw(void)
{
	
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//GLint ntest;
	//glGetIntegerv(GL_TEXTURE_RECTANGLE_ARB, &ntest);
	//if (bDetpth)
	//{
	//	glDrawBuffer(GL_BACK);
	//	int arViewport[4];
	//	glGetIntegerv(GL_VIEWPORT, arViewport);
	//	glPushMatrix();
	//	glMatrixMode(GL_PROJECTION);
	//	glPushMatrix();
	//	glLoadIdentity();
	//	gluOrtho2D(0.0f, arViewport[2], 0.0f, arViewport[3]);
	//	glMatrixMode(GL_MODELVIEW);
	//	glPushMatrix();
	//	glLoadIdentity();

	//	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//	glEnable(GL_DEPTH_TEST);

	//	int nWidth = arViewport[2];
	//	int nHeight = arViewport[3];

	//	glRasterPos2i(0, 0);
	//	glDrawPixels(nWidth, nHeight, GL_DEPTH_COMPONENT, GL_FLOAT, m_pBuffer);

	//	glPopAttrib();

	//	glPopMatrix();
	//	glMatrixMode(GL_PROJECTION);
	//	glPopMatrix();
	//	glMatrixMode(GL_MODELVIEW);
	//	glPopMatrix();
	//}

	//if (bLine)
	//{
	//	glEnable(GL_FRAMEBUFFER_EXT);
	//	glDrawBuffer(GL_BACK);
	//	UINT texID = m_pFBO->GetAttachedId(GL_COLOR_ATTACHMENT0_EXT);
	//	if (glIsTexture(texID))
	//	{
	//		glPushMatrix();
	//		glMatrixMode(GL_PROJECTION);
	//		glPushMatrix();
	//		glLoadIdentity();
	//		int width, height;
	//		glfwGetFramebufferSize(window, &width, &height);
	//		gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
	//		glMatrixMode(GL_MODELVIEW);
	//		glPushMatrix();
	//		//glutSolidTeapot(1.0);
	//		glPushAttrib(GL_ENABLE_BIT);
	//		{
	//			glDisable(GL_DEPTH_TEST);
	//			glDisable(GL_LIGHTING);
	//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//			glDisable(GL_CULL_FACE);

	//			//Texture Background
	//			glEnable(GL_TEXTURE_RECTANGLE_ARB);
	//			glGetIntegerv(GL_TEXTURE_RECTANGLE_ARB, &ntest);
	//			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texID);
	//			glColor3f(1.f, 1.f, 1.f);

	//			glBegin(GL_QUADS);
	//			glTexCoord2i(0, height); glVertex3f(0.0f, 1.0f, 0.0f);
	//			glTexCoord2i(width, height); glVertex3f(1.0f, 1.0f, 0.0f);
	//			glTexCoord2i(width, 0); glVertex3f(1.0f, 0.0f, 0.0f);
	//			glTexCoord2i(0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
	//			glEnd();
	//			//if(bDetpth)
	//				//glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	//		}
	//		glPopAttrib();

	//		glPopMatrix();
	//		glMatrixMode(GL_PROJECTION);
	//		glPopMatrix();
	//		glMatrixMode(GL_MODELVIEW);
	//		glPopMatrix();
	//	}


	//	//glDisable(GL_FRAMEBUFFER_EXT);
	//}


	
	glPushMatrix();
	GLdouble _mat[16],_mp[16];
	//GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, _mat);
	glGetDoublev(GL_PROJECTION_MATRIX, _mp);
	//glGetIntegerv(GL_VIEWPORT, viewport);
	
	//glGetIntegerv(GL_TEXTURE_RECTANGLE_ARB, &ntest);
	//glGetDoublev(GL_PROJECTION_MATRIX, _mp);
	glEnable(GL_DEPTH_TEST);
	
	//glGetIntegerv(GL_DEPTH_TEST, &ntest);
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);
	{
		//glPolygonOffset(-1.0, -1.0);
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		glLineWidth(3);
		glColor4f(1, 0.0, 0.0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawcube(30, 0, 0, 0, 1);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawcube(30, 0, 0, 0, 0);
		//if (bDetpthMask)
		//{
		//	glDepthMask(1);
		//}
		glPopAttrib();
		{
			glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glColor4f(0.0, 0.0, 1.0, 1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			drawcube(30, -30, -30, -30, 1);
			glColor4f(0.0, 0.0, 1.0, 1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			drawcube(30, -30, -30, -30, 0);

			glPopAttrib();
		}
	}

	glPopMatrix();
	


	
}



/* update animation parameters */
static void animate(void)
{
	angle = 10 * (float)glfwGetTime();
}

/* new window size */
void reshape(GLFWwindow* window, int width, int height)
{
	GLfloat h = (GLfloat)height / (GLfloat)width;
	GLfloat xmax, znear, zfar;

	znear = -10.0f;
	zfar = 30.0f;
	xmax = znear * 0.5f;

	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
	xmax = 100.0;
	znear = -xmax * 10;
	zfar = xmax * 10;
	glOrtho(-xmax * scale, xmax * scale, -xmax * scale * h, xmax * scale * h, znear * scale, zfar * scale);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef( 0.0, 0.0, -20.0 );
}

/* change view angle, exit upon ESC */
void key(GLFWwindow* window, int k, int s, int action, int mods)
{
	if (action != GLFW_PRESS) return;

	switch (k) {
	case GLFW_KEY_T:
		 bFbo = 1 - bFbo;
		break;
	case GLFW_KEY_Z:
		if (mods & GLFW_MOD_SHIFT)
			view_rotz -= 5.0;
		else
			view_rotz += 5.0;
		break;
	case GLFW_KEY_X:
		if (mods & GLFW_MOD_SHIFT)
		{
			scale -= 0.05f;
			//glScalef(0.5, 0.5, 0.5);
		}
		else
		{ 
			scale += 0.05f;
			//glScalef(1.5, 1.5, 1.5);
		}
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		reshape(window, width, height);
		break;
	case GLFW_KEY_N:
		bDetpth = 1 - bDetpth;
		break;
	case GLFW_KEY_V:
		bLine = 1 - bLine;
		break;
	case GLFW_KEY_S:
		bLineSMOOTH = 1 - bLineSMOOTH;
		break;
	case GLFW_KEY_R:
		bReverse = 1 - bReverse;
		break;
	case GLFW_KEY_M:
		bDetpthMask = 1 - bDetpthMask;
		break;
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_UP:
		view_rotx += 5.0;
		//drawFBO();
		break;
	case GLFW_KEY_DOWN:
		view_rotx -= 5.0;
		//drawFBO();
		break;
	case GLFW_KEY_LEFT:
		view_roty += 5.0;
		////drawFBO();
		break;
	case GLFW_KEY_RIGHT:
		view_roty -= 5.0;
		//drawFBO();
		break;
	default:
		return;
	} 
}

void mymouse(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		glScalef(0.5, 0.5, 0.5);
		break;
	//case GLFW_MOUSE_BUTTON_MIDDLE:
	//	strcpy(msg, "Mosue middle button clicked!");
	//	break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		glScalef(1.5, 1.5, 1.5);
		break;
	default:
		return;
	}
	//glutPostRedisplay();//重新调用绘制函数
	return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scale -= 0.05f*(GLfloat)yoffset;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	reshape(window, width, height);
}



static void createFBO()
{
	//glEnable(GL_FRAMEBUFFER_EXT);
	if (glewGetExtension("GL_EXT_framebuffer_object") != GL_TRUE)
	{
		//TRACE("Driver does not support Framebuffer Objects (GL_EXT_framebuffer_object)\n");
		return;
	}
	GLuint texID; GLint ntest;
	glGenTextures(1, &texID); // create (reference to) a new texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texID);
	//// (set texture parameters here)
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//GL_TEXTURE_RECTANGLE_ARB
	////create the texture
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// init size ( no meaning )
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// init size ( no meaning )

	m_pFBO = new FramebufferObject;
	m_pRB = new Renderbuffer;
	m_pFBO->Bind();

	// Attach texture to framebuffer color buffer
	m_pFBO->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, texID, GL_COLOR_ATTACHMENT0_EXT);

	// Optional: initialize depth & stencil renderbuffer
	m_pRB->Set(GL_DEPTH_STENCIL_EXT, width, height);
	m_pFBO->AttachRenderBuffer(m_pRB->GetId(), GL_DEPTH_ATTACHMENT_EXT);
	//m_pFBO->AttachRenderBuffer(m_pRB->GetId(), GL_STENCIL_ATTACHMENT_EXT);

	// Validate the FBO after attaching textures and render buffers
	if (!m_pFBO->IsValid())
	{
		//TRACE0("FBO Error!");
		if (m_pFBO)
		{
			UINT texID;
			texID = m_pFBO->GetAttachedId(GL_COLOR_ATTACHMENT0_EXT);
			if (glIsTexture(texID))
			{
				glDeleteTextures(1, &texID);
			}
			_SAFE_DELETE(m_pFBO);
		}
		return;
	}
	// Disable FBO rendering for now...
	FramebufferObject::Disable();
	glGetIntegerv(GL_TEXTURE_RECTANGLE_ARB, &ntest);
}


/* program & OpenGL initialization */
static void init(void)
{
	int* abc = new int(4);
	static GLfloat pos[4] = { 5.f, 5.f, 10.f, 0.f };

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glDisable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LINE_SMOOTH);
}


/* program entry */
int main(int argc, char* argv[])
{
	int width, height;

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(800, 800, "Gears", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Set callback functions
	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, key);
	//glfwSetMouseButtonCallback(window, mymouse);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	glfwGetFramebufferSize(window, &width, &height);
	reshape(window, width, height);

	//Init GLEW
	//glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();

	// Parse command-line options
	init();
	//createFBO();
	//drawFBO();
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Draw gears
		draw();
		//::Sleep(100);
		// Update animation
		//animate();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Terminate GLFW
	glfwTerminate();
	//VLDRefreshModules();
	// Exit program
	exit(EXIT_SUCCESS);
}

#else
#pragma region Main
/*Enter program*/


#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>


#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

int main(int argc, char** argv)
{
	//Create manager object and call sequential functions
	_CrtSetBreakAlloc(340);
	_CrtSetBreakAlloc(368);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//Manager manager;
	//manager.init();

	////Enter main loop
	//while (manager.state != programState::Closing)
	//{
	//	//Call frame functions chronologically
	//	manager.early();
	//	manager.input();
	//	manager.logic();
	//	manager.draw();
	//	manager.late();
	//}

	////Done
	//manager.quit();
	int* abc = new int(31);
	//_CrtDumpMemoryLeaks();
	return 0;
}
#pragma endregion
#endif