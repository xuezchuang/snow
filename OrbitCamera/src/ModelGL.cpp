///////////////////////////////////////////////////////////////////////////////
// ModelGL.cpp
// ===========
// Model component of OpenGL
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-29
// UPDATED: 2017-12-13
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include "gl/glew.h"
//#include <GL/gl.h>
//
//#include "GL/glut.h"


#include "ModelGL.h"

//#include "gl/glew.h"
#endif
//#include <GL/glut.h>
#include <cmath>
#include <sstream>


// constants
const float GRID_SIZE = 10.0f;
const float GRID_STEP = 1.0f;
const float CAM_DIST = 5.0f;
const float DEG2RAD = 3.141593f / 180;
const float FOV_Y = 50.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;
const int   MAX_LOG_LENGTH = 4096;
const float OBJ_SCALE = 0.01f;
const std::string OBJ_MODEL = "../bin/data/debugger_small_5k.obj";
const std::string OBJ_CAM = "../bin/data/camera.obj";
const std::string FONT_FILE = "../bin/data/walkway32_bold.fnt";

#pragma region Shader
// flat shading ===========================================
const char* vsSource1 = R"(
void main()
{
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
)";
const char* fsSource1 = R"(
void main()
{
    gl_FragColor = gl_Color;
}
)";


// blinn shading ==========================================
const char* vsSource2 = R"(
varying vec3 esVertex, esNormal;
void main()
{
    esVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    //esNormal = transpose(inverse(gl_ModelViewMatrix)) * gl_Normal;
    esNormal = gl_NormalMatrix * gl_Normal;
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
)";
const char* fsSource2 = R"(
varying vec3 esVertex, esNormal;
void main()
{
    vec3 normal = normalize(esNormal);
    vec3 light;
    if(gl_LightSource[0].position.w == 0.0)
    {
        light = normalize(gl_LightSource[0].position.xyz);
    }
    else
    {
        light = normalize(gl_LightSource[0].position.xyz - esVertex);
    }
    vec3 view = normalize(-esVertex);
    vec3 halfv = normalize(light + view);
    vec4 color =  gl_FrontMaterial.ambient * gl_FrontLightProduct[0].ambient;
    float dotNL = max(dot(normal, light), 0.0);
    color += gl_FrontMaterial.diffuse * gl_FrontLightProduct[0].diffuse * dotNL;
    float dotNH = max(dot(normal, halfv), 0.0);
    /*vec4 specular = (vec4(1.0) - color) * gl_FrontMaterial.specular * gl_FrontLightProduct[0].specular * pow(dotNH, gl_FrontMaterial.shininess);
    color += specular;*/
    color += gl_FrontMaterial.specular * gl_FrontLightProduct[0].specular * pow(dotNH, gl_FrontMaterial.shininess);
    gl_FragColor = color;
}
)";


//
const char* vsSource3 = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4 (aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;
}
)";
const char* fsSource3 = R"(
//片元着色器代码
#version 330 core
in vec3 Normal;
in vec3 FragPos;
in mat4 model_matrix;
out vec4 FragColor;
out mat4 test_matrix;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //漫反射光
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //镜面高光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
   // vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);
    test_matrix = model_matrix;
}
)";

#pragma endregion

///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
ModelGL::ModelGL() : windowWidth(0), windowHeight(0), mouseLeftDown(false), mouseLeftDown2(false),
                     mouseRightDown(false), windowSizeChanged(false),
                     nearPlane(NEAR_PLANE), farPlane(FAR_PLANE), fov(FOV_Y),
                     gridEnabled(true), gridSize(GRID_SIZE), gridStep(GRID_STEP),
                     vboSupported(false), vboReady(false), vboModel(0), vboCam(0),
                     glslSupported(false), glslReady(false), progId1(0), progId2(0),
                     objLoaded(false), fovEnabled(true)
{
    bgColor.set(0, 0, 0, 0);

    // init cameras
    Vector3 camPosition = Vector3(CAM_DIST*2, CAM_DIST*1.5f, CAM_DIST*2);
    Vector3 camTarget = Vector3(0, 0, 0);
    cam1.lookAt(camPosition, camTarget);
    cam2.lookAt(Vector3(0, 0, CAM_DIST), Vector3(0, 0, 0));
    cameraAngle = cam2.getAngle();
    cameraPosition = cam2.getPosition();
    cameraTarget = cam2.getTarget();
    cameraTempTarget = cameraTarget;
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();

    // init default material
    defaultAmbient[0] = 0.8f; defaultAmbient[1] = 0.6f; defaultAmbient[2] = 0.2f; defaultAmbient[3] = 1.0f;
    defaultDiffuse[0] = 1.0f; defaultDiffuse[1] = 0.9f; defaultDiffuse[2] = 0.2f; defaultDiffuse[3] = 1.0f;
    defaultSpecular[0] = defaultSpecular[1] = defaultSpecular[2] = 1.0f; defaultSpecular[3] = 1.0f;
    defaultShininess = 128.0f;

    // init camera material
    camAmbient[0] = camAmbient[1] = camAmbient[2] = 0.0f; camAmbient[3] = 1.0f;
    camDiffuse[0] = camDiffuse[1] = camDiffuse[2] = 0.9f; camDiffuse[3] = 1.0f;
    camSpecular[0] = camSpecular[1] = camSpecular[2] = 1.0f; camSpecular[3] = 1.0f;
    camShininess = 256.0f;

    dxOff = 0.0;
    dyOff = 0.0;
    // init fov vertices
    computeFovVertices(fov);
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
ModelGL::~ModelGL()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void ModelGL::init()
{
    glShadeModel(GL_SMOOTH);                        // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);          // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);   // background color
    glClearStencil(0);                              // clear stencil buffer
    glClearDepth(1.0f);                             // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    initLights();
    initFont();

    
	//if (!gladLoadGL(glfwGetProcAddress)) {
	//	std::cout << "Failed to initialize GLAD" << std::endl;
	//	return -1;
	//}
    GLenum glewStatus = glewInit();
    lampShader = new Shader("Shader.vs", "LampShader.fs");
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void ModelGL::initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};      // ambient light
    GLfloat lightKd[] = {.8f, .8f, .8f, 1.0f};      // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};               // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light in eye space
    float lightPos[4] = {0, 0, 1, 0};               // directional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                            // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// initialize bitmap font
///////////////////////////////////////////////////////////////////////////////
void ModelGL::initFont()
{
    font.loadFont(FONT_FILE.c_str());
    font.setColor(1, 1, 1, 1);
    font.setScale(0.5, 0.5);
}



///////////////////////////////////////////////////////////////////////////////
// initialize GLSL programs
// NOTE:shader programs can be shared among multiple contexts, create only once
///////////////////////////////////////////////////////////////////////////////
bool ModelGL::initShaders()
{
    if(!glslReady)
    {
		//check extensions
		glExtension& extension = glExtension::getInstance();
		glslSupported = extension.isSupported("GL_ARB_shader_objects");
		if (glslSupported)
            glslReady = createShaderPrograms();
    }
    return glslReady;
}



///////////////////////////////////////////////////////////////////////////////
// clean up OpenGL objects
///////////////////////////////////////////////////////////////////////////////
void ModelGL::quit()
{
}



///////////////////////////////////////////////////////////////////////////////
// set rendering window size
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setWindowSize(int width, int height)
{
    // assign the width/height of viewport
    windowWidth = width;
    windowHeight = height;
    windowSizeChanged = true;
}



///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setViewport(int x, int y, int w, int h)
{
    // set viewport to be the entire window
    glViewport((GLsizei)x, (GLsizei)y, (GLsizei)w, (GLsizei)h);

    // set perspective viewing frustum
    setFrustum(fov, (float)(w)/h, nearPlane, farPlane); // FOV, AspectRatio, NearClip, FarClip
}

///////////////////////////////////////////////////////////////////////////////
// draw 2D/3D scene
///////////////////////////////////////////////////////////////////////////////
void ModelGL::draw(int screenId)
{
    preFrame();
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glEnable(GL_BLEND);

    if(screenId == 1)
    {
        // set projection matrix to OpenGL
        setFrustum(FOV_Y, (float)windowWidth/windowHeight, nearPlane, farPlane);
		//glMatrixMode(GL_PROJECTION);
		//glLoadMatrixf(matrixProjection.get());
		//glMatrixMode(GL_MODELVIEW);
        //glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
        lampShader->setMat4("projection", matrixProjection.get());
        

		float mat[16], _mat[16];// get the modelview matrix
        //float normat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		glGetFloatv(GL_PROJECTION_MATRIX, _mat);
        //glGetFloatv(GL_MATRIX_MODE, normat);.
        // from 3rd person camera
        Matrix4 matView = cam1.getMatrix();
        glLoadMatrixf(matView.get());

        lampShader->setMat4("view", matView.get());
        
        // draw grid
        if(gridEnabled)
            drawGridXZ(gridSize, gridStep);

        // draw line from camera to focal
        drawFocalLine();
        drawFocalPoint();

		Matrix4 matModel2 = cam2.getMatrix();
        Matrix4 matModel3 = cam2.getMatrix();
        Matrix4 matModel4 = cam2.getMatrix();
        Matrix4 matModel5 = cam2.getMatrix();
		matModel2.invertEuclidean();
		matModel3.invertAffine();
        matModel4.invertProjective();
        matModel5.invertGeneral();
        // matrix for camera model
        Matrix4 matModel;
        matModel.translate(cameraPosition);
        matModel.lookAt(cameraTarget, cam2.getUpAxis());
        Matrix4 matModelView = matView * matModel;


		
        
        //// draw obj models
        //if(objLoaded)
        //{
        //    if(vboReady)
        //    {
        //        drawObjWithVbo();
        //        /*glLoadMatrixf(matModelView.get());*/
        //        lampShader->setMat4("mode", matModelView.get());
        //        drawCameraWithVbo();
        //    }
        //    else 
        //    {
        //        drawObj();
        //        //glLoadMatrixf(matModelView.get());
        //        drawCamera();
        //    }
        //    if(fovEnabled)
        //        drawFov();
        //}
        //showInfo(screenId);
    }
    else if(screenId == 2)
    {

        float mat[16], _mat[16];// get the modelview matrix
        glGetFloatv(GL_PROJECTION_MATRIX, _mat);
        // set projection matrix to OpenGL
        setFrustum2(fov, (float)windowWidth/windowHeight, nearPlane, farPlane);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(matrixProjection.get());
        glMatrixMode(GL_MODELVIEW);

        // from camera object
        glLoadMatrixf(cameraMatrix.get());
        ///////////////////////////////////////////////////////////////////////////////
		//Verify gluLookAt & OrbitCamera::lookAt 
		
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
        glGetFloatv(GL_PROJECTION_MATRIX, _mat);
        float mat_n[16];
        glGetFloatv(GL_NORMALIZE, mat_n);
  //      glGetIntegerv(GL_VIEWPORT, viewport);
		//gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraTarget.x, cameraTarget.y, cameraTarget.z, cam2.getMatrix()[1], cam2.getMatrix()[5], cam2.getMatrix()[9]);
		//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
        ///////////////////////////////////////////////////////////////////////////////
        // draw grid
        if(gridEnabled)
            drawGridXZ(gridSize, gridStep);
        // draw focal point
        drawFocalPoint();

        // draw OBJ model
        if(objLoaded)
        {
            if(vboReady)
                drawObjWithVbo();
            else
                drawObj();
        }
        Matrix4 matModelView2 = mat;
		//matView.identity();
		Vector3 abc = { 1.0,2.0,3.0 };
        abc.normalize();
        Vector3 abc2 = abc * matModelView2;
		matModelView2.invertGeneral().transpose();
		Matrix3 Temp = { matModelView2[0],matModelView2[1],matModelView2[2],matModelView2[4],matModelView2[5],matModelView2[6],matModelView2[8],matModelView2[9],matModelView2[10] };
        
		//Temp.identity();
		Matrix3 TempV = Temp;
		Matrix3 TempT = Temp;

		Matrix3 tt = TempV.invert() * TempT.transpose();
		//Temp.invert().transpose();
		//Temp.identity();
		Vector3 abc1 = abc * Temp;
        int a = 4;
    }

     //draw 2D
    draw2D(screenId);

    postFrame();
}


///////////////////////////////////////////////////////////////////////////////
// draw 2D
///////////////////////////////////////////////////////////////////////////////
void ModelGL::draw2D(int screenId)
{
    // set orthogonal projection
    setOrthoFrustum(0, (float)windowWidth, 0, (float)windowHeight, -1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrixProjection.get());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    if(screenId == 1)
    {
        //gl_NormalMatrix验证glsl的法向量矩阵为modematrix的invert.transpose 逆转换矩阵，得不到当前的gl_NormalMatrix,可在glsl中导出gl_NormalMatrix查看
        //当前程序的glsl版本太旧，在opengl 140版本后已放弃gl_NormalMatrix全局变量；用户自己输入，公式 Normal = mat3(transpose(inverse(model)) * aNormal;
        std::stringstream ss;
        Matrix4 matView = cam1.getMatrix();
        Vector4 v1 = { matView[0],matView[1],matView[2],matView[3] };
        Vector4 v2 = { matView[4],matView[5],matView[6],matView[7] };
        Vector4 v3 = { matView[8],matView[9],matView[10],matView[11] };
        Vector4 v4 = { matView[12],matView[13],matView[14],matView[16] };
        ss << "Normalized: " << v1.normalize();
        font.drawText(5, (float)windowHeight - font.getHeight(), ss.str().c_str());
        ss.str("");
        ss << "Normalized: " << v2.normalize();
        font.drawText(5, (float)windowHeight - 2*font.getHeight(), ss.str().c_str());
        ss.str("");
        ss << "Normalized: " << v3.normalize();
        font.drawText(5, (float)windowHeight - 3 * font.getHeight(), ss.str().c_str());
        ss.str("");
        ss << "Normalized: " << v4.normalize();
        font.drawText(5, (float)windowHeight - 4 * font.getHeight(), ss.str().c_str());
        //
        matView.invertGeneral().transpose();
        //

		v1 = { matView[0],matView[1],matView[2],matView[3] };
		v2 = { matView[4],matView[5],matView[6],matView[7] };
		v3 = { matView[8],matView[9],matView[10],matView[11] };
		v4 = { matView[12],matView[13],matView[14],matView[16] };
        ss.str("");
		ss << "Normalized: " << v1.normalize();
		font.drawText(5, (float)windowHeight - 5 * font.getHeight(), ss.str().c_str());
		ss.str("");
		ss << "Normalized: " << v2.normalize();
		font.drawText(5, (float)windowHeight - 6 * font.getHeight(), ss.str().c_str());
		ss.str("");
		ss << "Normalized: " << v3.normalize();
		font.drawText(5, (float)windowHeight - 7 * font.getHeight(), ss.str().c_str());
		ss.str("");
		ss << "Normalized: " << v4.normalize();
		font.drawText(5, (float)windowHeight - 8 * font.getHeight(), ss.str().c_str());

    }
    else if(screenId == 2)
    {
        font.drawText(5, (float)windowHeight-font.getHeight(), "Point of View");
    }

    glDisable(GL_TEXTURE_2D);

    // restore prev setting
    glEnable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
}



///////////////////////////////////////////////////////////////////////////////
// pre-frame
///////////////////////////////////////////////////////////////////////////////
void ModelGL::preFrame()
{
    if(windowSizeChanged)
    {
        setViewport(0, 0, windowWidth, windowHeight);
        windowSizeChanged = false;
    }
}


///////////////////////////////////////////////////////////////////////////////
// post-frame
///////////////////////////////////////////////////////////////////////////////
void ModelGL::postFrame()
{
}



///////////////////////////////////////////////////////////////////////////////
// rotate the 3rd person camera
///////////////////////////////////////////////////////////////////////////////
void ModelGL::rotateCamera(int x, int y)
{
    const float ANGLE_SCALE = 0.2f;
    Vector3 angle = cam1.getAngle();
    angle.y -= (x - mouseX) * ANGLE_SCALE;
    angle.x += (y - mouseY) * ANGLE_SCALE;
    mouseX = x;
    mouseY = y;

    // constrain x angle -89 < x < 89
    if(angle.x < -89.0f)
        angle.x = -89.0f;
    else if(angle.x > 89.0f)
        angle.x = 89.0f;

    cam1.rotateTo(angle);
}

void ModelGL::rotateCamera2(int x, int y)
{
	const float ANGLE_SCALE = 0.2f;
	Vector3 angle = cam2.getAngle();
	angle.y -= (x - mouseX2) * ANGLE_SCALE;
	angle.x += (y - mouseY2) * ANGLE_SCALE;
	mouseX2= x;
	mouseY2 = y;

	//// constrain x angle -89 < x < 89
	//if (angle.x < -89.0f)
	//	angle.x = -89.0f;
	//else if (angle.x > 89.0f)
	//	angle.x = 89.0f;

	cam2.rotateTo(angle);
}



///////////////////////////////////////////////////////////////////////////////
// move forward the 3rd person camera
///////////////////////////////////////////////////////////////////////////////
void ModelGL::zoomCamera(int y)
{
    float delta = (float)y - mouseY;
    zoomCameraDelta(delta);
}
void ModelGL::zoomCameraDelta(float delta)
{
    const float ZOOM_SCALE = 0.5f;
    const float MIN_DIST   = 1.0f;
    const float MAX_DIST   = 30.0f;
    float distance = cam1.getDistance();
    distance -= (delta * ZOOM_SCALE);

    // constrain min and max
    if(distance < MIN_DIST)
        distance = MIN_DIST;
    else if(distance > MAX_DIST)
        distance = MAX_DIST;

    cam1.setDistance(distance);
}
void ModelGL::zoomCameraDelta2(float delta)
{
	const float ZOOM_SCALE = 0.5f;
	const float MIN_DIST = 1.0f;
	const float MAX_DIST = 30.0f;
	float distance = cam2.getDistance();
	distance -= (delta * ZOOM_SCALE);

	// constrain min and max
	if (distance < MIN_DIST)
		distance = MIN_DIST;
	else if (distance > MAX_DIST)
		distance = MAX_DIST;

	cam2.setDistance(distance);
}


///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setFrustum(float l, float r, float b, float t, float n, float f)
{
    matrixProjection.identity();
    matrixProjection[0]  =  2 * n / (r - l);
    matrixProjection[5]  =  2 * n / (t - b);
    matrixProjection[8]  =  (r + l) / (r - l);
    matrixProjection[9]  =  (t + b) / (t - b);
    matrixProjection[10] = -(f + n) / (f - n);
    matrixProjection[11] = -1;
    matrixProjection[14] = -(2 * f * n) / (f - n);
    matrixProjection[15] =  0;
}



void ModelGL::setFrustum(float fovY, float aspectRatio, float front, float back)
{
	float tangent = tanf(fovY / 2 * DEG2RAD);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane

	// params: left, right, bottom, top, near, far
	setFrustum(-width, width, -height,height , front, back);
}


///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setFrustum2(float fovY, float aspectRatio, float front, float back)
{
    float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane

    // params: left, right, bottom, top, near, far
    setFrustum(-width + dxOff, width + dxOff, -height + dyOff, height + dyOff, front, back);
}



///////////////////////////////////////////////////////////////////////////////
// set a orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
    matrixProjection.identity();
    matrixProjection[0]  =  2 / (r - l);
    matrixProjection[5]  =  2 / (t - b);
    matrixProjection[10] = -2 / (f - n);
    matrixProjection[12]  =  -(r + l) / (r - l);
    matrixProjection[13]  =  -(t + b) / (t - b);
    matrixProjection[14] = -(f + n) / (f - n);
}



///////////////////////////////////////////////////////////////////////////////
// set vertical FOV
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setFov(float fov)
{
    this->fov = fov;
    computeFovVertices(fov);
    setFrustum(fov, (float)(windowWidth)/windowHeight, nearPlane, farPlane); // FOV, AspectRatio, NearClip, FarClip
}



///////////////////////////////////////////////////////////////////////////////
// draw a line from camera to focal point
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawFocalLine()
{
    // disable lighting
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
    glLineWidth(1.0f);

    glColor4f(1.0f, 1.0f, 0.2f, 0.7f);
    glBegin(GL_LINES);
    glVertex3fv(&cameraPosition[0]);
    glVertex3fv(&cameraTarget[0]);
    glEnd();

    // enable lighting back
    glLineWidth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
}



///////////////////////////////////////////////////////////////////////////////
// draw a line from camera to focal point
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawFocalPoint()
{
    // disable lighting
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
    glPointSize(5.0f);

    glColor4f(1.0f, 1.0f, 0.2f, 0.7f);
    glBegin(GL_POINTS);
    glVertex3fv(&cameraTarget[0]);
    glEnd();

    // enable lighting back
    glPointSize(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
}



///////////////////////////////////////////////////////////////////////////////
// draw a grid on XZ-plane
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawGridXZ(float size, float step)
{
    // disable lighting
    glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);
    glLineWidth(0.5f);

    glBegin(GL_LINES);

    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size, 0,  i);   // lines parallel to X-axis
        glVertex3f( size, 0,  i);
        glVertex3f(-size, 0, -i);   // lines parallel to X-axis
        glVertex3f( size, 0, -i);

        glVertex3f( i, 0, -size);   // lines parallel to Z-axis
        glVertex3f( i, 0,  size);
        glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
        glVertex3f(-i, 0,  size);
    }

    // x-axis
    glColor4f(1, 0, 0, 1.0f);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f( size, 0, 0);

	// y-axis
	glColor4f(0, 1, 0, 1.0f);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, size, 0);

    // z-axis
    glColor4f(0, 0, 1, 1.0f);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0,  size);

    glEnd();

    // enable lighting back
    glLineWidth(1.0f);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



///////////////////////////////////////////////////////////////////////////////
// draw a grid on the xy plane
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawGridXY(float size, float step)
{
    glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);
    glLineWidth(0.5f);

    glBegin(GL_LINES);

    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size,  i, 0);   // lines parallel to X-axis
        glVertex3f( size,  i, 0);
        glVertex3f(-size, -i, 0);   // lines parallel to X-axis
        glVertex3f( size, -i, 0);

        glVertex3f( i, -size, 0);   // lines parallel to Y-axis
        glVertex3f( i,  size, 0);
        glVertex3f(-i, -size, 0);   // lines parallel to Y-axis
        glVertex3f(-i,  size, 0);
    }

    // x-axis
    glColor4f(1.0f, 0, 0, 0.5f);
    glVertex3f(-size, 0, 0);
    glVertex3f( size, 0, 0);

    // y-axis
    glColor4f(0, 0, 1.0f, 0.5f);
    glVertex3f(0, -size, 0);
    glVertex3f(0,  size, 0);

    glEnd();

    glLineWidth(1.0f);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



///////////////////////////////////////////////////////////////////////////////
// reset both cameras
///////////////////////////////////////////////////////////////////////////////
void ModelGL::resetCamera()
{
    // 3rd person camera
    cam1.lookAt(Vector3(CAM_DIST*2, CAM_DIST*1.5f, CAM_DIST*2), Vector3(0, 0, 0));

    // camera object
    cam2.lookAt(Vector3(0, 0, CAM_DIST), Vector3(0, 0, 0));
    cameraAngle = cam2.getAngle();
    cameraPosition = cam2.getPosition();
    cameraTarget = cam2.getTarget();
    cameraTempTarget = cameraTarget;
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();

    // fov
    fov = FOV_Y;
    computeFovVertices(fov);
    setFrustum(fov, (float)(windowWidth)/windowHeight, nearPlane, farPlane); // FOV, AspectRatio, NearClip, FarClip
}



///////////////////////////////////////////////////////////////////////////////
// compute grid size and step
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setGridSize(float size)
{
    gridSize = size;
    gridStep = 1;
}



///////////////////////////////////////////////////////////////////////////////
// create glsl programs
///////////////////////////////////////////////////////////////////////////////
bool ModelGL::createShaderPrograms()
{
    // create 1st shader and program
    GLhandleARB vsId1 = glCreateShaderObjectARB(GL_VERTEX_SHADER);
    GLhandleARB fsId1 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
    progId1 = glCreateProgramObjectARB();

    // load shader sources: flat shader
    glShaderSourceARB(vsId1, 1, &vsSource1, NULL);
    glShaderSourceARB(fsId1, 1, &fsSource1, NULL);

    // compile shader sources
    glCompileShaderARB(vsId1);
    glCompileShaderARB(fsId1);

    // attach shaders to the program
    glAttachObjectARB(progId1, vsId1);
    glAttachObjectARB(progId1, fsId1);

    // link program
    glLinkProgramARB(progId1);

    // create 2nd shader and program
    GLhandleARB vsId2 = glCreateShaderObjectARB(GL_VERTEX_SHADER);
    GLhandleARB fsId2 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
    progId2 = glCreateProgramObjectARB();

    // load shader sources:
    glShaderSourceARB(vsId2, 1, &vsSource2, NULL);
    glShaderSourceARB(fsId2, 1, &fsSource2, NULL);

    // compile shader sources
    glCompileShaderARB(vsId2);
    glCompileShaderARB(fsId2);

    // attach shaders to the program
    glAttachObjectARB(progId2, vsId2);
    glAttachObjectARB(progId2, fsId2);

    // link program
    glLinkProgramARB(progId2);

    glUseProgramObjectARB(progId2);

    // check status
    int linkStatus1, linkStatus2;
    glGetObjectParameterivARB(progId1, GL_OBJECT_LINK_STATUS_ARB, &linkStatus1);
    glGetObjectParameterivARB(progId2, GL_OBJECT_LINK_STATUS_ARB, &linkStatus2);
    
    if(linkStatus1 == GL_TRUE && linkStatus2 == GL_TRUE)
    {
        return true;
    }
    else
    {
        return false;
    }

    //
    
}



///////////////////////////////////////////////////////////////////////////////
// get/set camera parameters
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setCamera()
{
	cameraPosition = cam2.getPosition();
	cameraQuaternion = cam2.getQuaternion();
	cameraMatrix = cam2.getMatrix();
    cameraAngle = cam2.getAngle();
    //cam2.setRotation(cameraAngle);
}
void ModelGL::setCameraAngleX(float x)
{
    cameraAngle.x = x;
    cam2.setRotation(cameraAngle);
    cameraPosition = cam2.getPosition();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraAngleY(float y)
{
    cameraAngle.y = y;
    cam2.setRotation(cameraAngle);
    cameraPosition = cam2.getPosition();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraAngleZ(float z)
{
    cameraAngle.z = z;
    cam2.setRotation(cameraAngle);
    cameraPosition = cam2.getPosition();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraPositionX(float x)
{
    cameraPosition.x = x;
    cam2.setPosition(cameraPosition);
    cameraAngle = cam2.getAngle();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraPositionY(float y)
{
    cameraPosition.y = y;
    cam2.setPosition(cameraPosition);
    cameraAngle = cam2.getAngle();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraPositionZ(float z)
{
    cameraPosition.z = z;
    cam2.setPosition(cameraPosition);
    cameraAngle = cam2.getAngle();
    cameraQuaternion = cam2.getQuaternion();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraTargetX(float x)
{
    cameraTempTarget.x = x;
	Matrix4 temp = cam2.getMatrix();
    cameraTarget = temp.transpose() * cameraTempTarget;
    cam2.setTarget(cameraTarget);
    cameraPosition = cam2.getPosition();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraTargetY(float y)
{
	cameraTempTarget.y = y;
	Matrix4 temp = cam2.getMatrix();
    cameraTarget = temp.transpose() * cameraTempTarget;
	cam2.setTarget(cameraTarget);
    cameraPosition = cam2.getPosition();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::setCameraTargetZ(float z)
{
	cameraTempTarget.z = z;
	Matrix4 temp = cam2.getMatrix();
    cameraTarget = temp.transpose() * cameraTempTarget;
	cam2.setTarget(cameraTarget);
    cameraPosition = cam2.getPosition();
    cameraMatrix = cam2.getMatrix();
}
void ModelGL::DragScreen(int x,int y)
{
    //SetViewMatrixInfo();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	float tangent = 0.1f * tanf(50 / 2 * DEG2RAD);
    double dradio = (tangent * 2) / viewport[3];
    
    dxOff += (float)dradio * (mouseX2 - x);
    dyOff -= (float)dradio * (mouseY2 - y);
	mouseX2 = x;
    mouseY2 = y;

	// params: left, right, bottom, top, near, far
	//setFrustum(-width, width, -height, height, front, back);

	//double wx, wy, wz, wx2, wy2, wz2;
	//

 //   float fx = mouseX2 - x;
 //   float fy = mouseY2 - y;
	//mouseX2 = x;
	//mouseY2 = y;
	////Vector3 v1 = { fx,fy,0 };

 //   screen2wcs(fx, fy, wx, wy, wz);

 //   cameraTarget.x += wx;
	//cameraTarget.y += wy;
 //   cameraTarget.z += wz;
	//Matrix4 temp = cam2.getMatrix();
 //   cameraTempTarget = temp * cameraTarget;

	//cam2.setTarget(cameraTarget);
	//cameraPosition = cam2.getPosition();
	//cameraMatrix = cam2.getMatrix();


	//glGetDoublev(GL_MODELVIEW_MATRIX, mmv);


    //matrixProjection cameraMatrix

	//glGetIntegerv(GL_VIEWPORT, viewport);

	//GLdouble win[3], win2[3];
	//gluUnProject(x, viewport[3] - y, 0.0, mmv, mp, viewport, &win[0], &win[1], &win[2]);
	//gluUnProject(mouseX2, viewport[3] - mouseY2, 0.0, mmv, mp, viewport, &win2[0], &win2[1], &win2[2]);


 //   GLdouble mmv[16], mp[16];
	//setFrustum(fov, (float)windowWidth / windowHeight, nearPlane, farPlane);
	//for (int i = 0; i < 16; i++)
	//{
	//	mmv[i] = cameraMatrix.get()[i];
	//	mp[i] = matrixProjection.get()[i];
	//}
	//GLdouble obt[3] = { 1.0,0.0,0.0 };
	//GLdouble obj1[3] = { cameraTarget.x,cameraTarget.y,0.0 };
 //   GLdouble win[3], win2[3];
 //   //三维坐标转为屏幕坐标
 //   gluProject(obt[0], obt[1], obt[2], mmv, mp, viewport, &win[0], &win[1], &win[2]);
 //   gluProject(obj1[0], obj1[1], obj1[2], mmv, mp, viewport, &win2[0], &win2[1], &win2[2]);
 //   
 //   obt[0] = win[0]; obt[1] = viewport[3]-win[1]; obt[2] = win[2];
 //   obj1[0] = win2[0]; obj1[1] = viewport[3]-win2[1]; obj1[2] = win2[2];

 //   screen2wcs((int)obt[0], (int)obt[1], win[0], win[1], win[2]);
 //   screen2wcs((int)obj1[0], (int)obj1[1], win2[0], win2[1], win2[2]);

 //   //屏幕坐标转为三维坐标
 //   gluUnProject(obt[0], obt[1], obt[2], mmv, mp, viewport, &win[0], &win[1], &win[2]);
 //   gluUnProject(obj1[0], obj1[1], obj1[2], mmv, mp, viewport, &win2[0], &win2[1], &win2[2]);
 //   //PS:鼠标移动没有z轴深度坐标,得到的坐标是? 一般投射到坐标系的xy平面内,
       
}

///////////////////////////////////////////////////////////////////////////////
// transition screen to wcs
///////////////////////////////////////////////////////////////////////////////
void ModelGL::screen2wcs(int x, int y, double& dx, double& dy, double& dz)
{
    Vector3 obj;
    const Vector3  plan_pos = { 0, 0, 0 };
    const Vector3 plan_nomal = { 0, 0, 1 };
	ScreenCord2Wcs(x, y, plan_pos, plan_nomal, obj);
	dx = obj[0];
	dy = obj[1];
	dz = obj[2];
}
void ModelGL::ScreenCord2Wcs(int x, int y, const Vector3& plane_pos, const Vector3& plane_norm, Vector3& pdCoord)
{
	const TViewMatInfo* viewMat = GetViewMatrixInfo();

	TVector3GLd objPnt, screenNorm;
	GLUnProject(viewMat, x, viewMat->viewport.vec[3] - y, objPnt.vec);
	GLGetScreenNorm(viewMat, x, viewMat->viewport.vec[3] - y, screenNorm.vec);


    float t = 0.0;
	const float num = plane_norm.dot(plane_pos) - plane_norm.dot(Vector3((float)objPnt.vec[0], (float)objPnt.vec[1], (float)objPnt.vec[2]));
	const float denom = plane_norm.dot(Vector3((float)screenNorm.vec[0], (float)screenNorm.vec[1], (float)screenNorm.vec[2]));

	t = num / denom;

	pdCoord[0] = (float)objPnt.vec[0] + t * (float)screenNorm.vec[0];
	pdCoord[1] = (float)objPnt.vec[1] + t * (float)screenNorm.vec[1];
	pdCoord[2] = (float)objPnt.vec[2] + t * (float)screenNorm.vec[2];
}
void ModelGL::SetViewMatrixInfo()
{
	GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    setFrustum(fov, (float)windowWidth / windowHeight, nearPlane, farPlane);
    for(int i = 0;i < 4;i++)
	    m_viewMatInfo.viewport.vec[i] = viewport[i];
	for (int i = 0; i < 16; i++)
        m_viewMatInfo.mmv._mat[i] = cameraMatrix.get()[i];
	for (int i = 0; i < 16; i++)
        m_viewMatInfo.mp._mat[i] = matrixProjection.get()[i];
}

///////////////////////////////////////////////////////////////////////////////
// load obj model
///////////////////////////////////////////////////////////////////////////////
bool ModelGL::loadObjs()
{
    objModel.read(OBJ_MODEL.c_str());
    objCam.read(OBJ_CAM.c_str());
    if(objModel.getVertexCount() > 0 && objCam.getVertexCount() > 0)
        objLoaded = true;
    else
        objLoaded = false;

    // create VBOs for OBJ model
    if(!vboReady)
    {
        glExtension& extension = glExtension::getInstance();
        vboSupported = extension.isSupported("GL_ARB_vertex_buffer_object");
        if(vboSupported)
        {
            createVertexBufferObjects();
            vboReady = true;
        }
        else
            vboReady = false;
    }

    return objLoaded;
}



///////////////////////////////////////////////////////////////////////////////
// create VBOs
///////////////////////////////////////////////////////////////////////////////
void ModelGL::createVertexBufferObjects()
{
    // create/setup VBO for model
    const float* interleavedVertices = objModel.getInterleavedVertices();
    unsigned int dataSize = objModel.getInterleavedVertexSize();
    glGenBuffers(1, &vboModel);
    glBindBuffer(GL_ARRAY_BUFFER, vboModel);
    glBufferData(GL_ARRAY_BUFFER, dataSize, interleavedVertices, GL_STATIC_DRAW);

    // create VBO array for indices
    iboModel.clear();
    int count = objModel.getGroupCount();
    iboModel.resize(count);
    glGenBuffers(count, &iboModel[0]);
    
    // setup vbos for indices
    for(int i = 0; i < count; ++i)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboModel[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, objModel.getIndexCount(i)*sizeof(int), (void*)objModel.getIndices(i), GL_STATIC_DRAW);
    }
    glFlush();

    // create / setup VBO for camera
    interleavedVertices = objCam.getInterleavedVertices();
    dataSize = objCam.getInterleavedVertexSize();
    glGenBuffers(1, &vboCam);
    glBindBuffer(GL_ARRAY_BUFFER, vboCam);
    glBufferData(GL_ARRAY_BUFFER, dataSize, interleavedVertices, GL_STATIC_DRAW);

    // create VBO for camera model indices
    iboCam.clear();
    count = objCam.getGroupCount();
    iboCam.resize(count);
    glGenBuffers(count, &iboCam[0]);
    for(int i = 0; i < count; ++i)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboCam[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, objCam.getIndexCount(i)*sizeof(int), (void*)objCam.getIndices(i), GL_STATIC_DRAW);
    }
}



///////////////////////////////////////////////////////////////////////////////
// draw obj model
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawObj()
{
    if(glslReady)
        glUseProgramObjectARB(progId2);

    // enable vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    // before draw, specify vertex arrays
    const float* vertices = objModel.getInterleavedVertices();
    int stride = objModel.getInterleavedStride();
    glVertexPointer(3, GL_FLOAT, stride, vertices);
    glNormalPointer(GL_FLOAT, stride, vertices + 3);

    for(int i = 0; i < objModel.getGroupCount(); ++i)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);

        glDrawElements(GL_TRIANGLES, (GLsizei)objModel.getIndexCount(i), GL_UNSIGNED_INT, objModel.getIndices(i));
    }

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);

    // reset shader
    if(glslReady)
        glUseProgramObjectARB(0);
}



///////////////////////////////////////////////////////////////////////////////
// draw OBJ model with VBOs
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawObjWithVbo()
{
    if(glslReady)
        glUseProgramObjectARB(progId2);

    glBindBuffer(GL_ARRAY_BUFFER, vboModel);

    // before draw, specify vertex and index arrays with their offsets and stride
    int stride = objModel.getInterleavedStride();
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, stride, (void*)(sizeof(float)*3));
    glVertexPointer(3, GL_FLOAT, stride, 0);

    for(int i = 0; i < (int)iboModel.size(); ++i)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboModel[i]);
        glDrawElements(GL_TRIANGLES, objModel.getIndexCount(i), GL_UNSIGNED_INT, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // reset shader
    if(glslReady)
        glUseProgramObjectARB(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



///////////////////////////////////////////////////////////////////////////////
// draw camera
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawCamera()
{
    if(glslReady)
        glUseProgramObjectARB(progId2);

    // enable vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    // before draw, specify vertex arrays
    const float* vertices = objCam.getInterleavedVertices();
    int stride = objCam.getInterleavedStride();
    glVertexPointer(3, GL_FLOAT, stride, vertices);
    glNormalPointer(GL_FLOAT, stride, vertices + 3);

    for(int i = 0; i < objCam.getGroupCount(); ++i)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, camAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, camDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, camSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, camShininess);

        glDrawElements(GL_TRIANGLES, (GLsizei)objCam.getIndexCount(i), GL_UNSIGNED_INT, objCam.getIndices(i));
    }

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);

    // reset shader
    if(glslReady)
        glUseProgramObjectARB(0);
}



///////////////////////////////////////////////////////////////////////////////
// draw camera with VBOs
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawCameraWithVbo()
{
    if(glslReady)
        glUseProgramObjectARB(progId2);

    glBindBuffer(GL_ARRAY_BUFFER, vboCam);

    // before draw, specify vertex and index arrays with their offsets and stride
    int stride = objCam.getInterleavedStride();
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, stride, (void*)(sizeof(float)*3));
    glVertexPointer(3, GL_FLOAT, stride, 0);

    for(int i = 0; i < (int)iboCam.size(); ++i)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, camAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, camDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, camSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, camShininess);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboCam[i]);
        glDrawElements(GL_TRIANGLES, objCam.getIndexCount(i), GL_UNSIGNED_INT, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // reset shader
    if(glslReady)
        glUseProgramObjectARB(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



///////////////////////////////////////////////////////////////////////////////
// draw Field Of View
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawFov()
{
    // draw backface first
    glCullFace(GL_FRONT);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glBegin(GL_TRIANGLES);
    // top
    glNormal3fv(&fovNormals[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[2][0]);
    glVertex3fv(&fovVertices[1][0]);
    // bottom
    glNormal3fv(&fovNormals[1][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[3][0]);
    glVertex3fv(&fovVertices[4][0]);
    // left
    glNormal3fv(&fovNormals[2][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[1][0]);
    glVertex3fv(&fovVertices[3][0]);
    // right
    glNormal3fv(&fovNormals[3][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[4][0]);
    glVertex3fv(&fovVertices[2][0]);
    glEnd();
    //
    glBegin(GL_LINES);
	// x-axis
	glColor4f(1, 0, 0, 1.0f);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);

	// y-axis
	glColor4f(0, 1, 0, 1.0f);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);

	// z-axis
	glColor4f(0, 0, 1, 1.0f);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);

	glEnd();

    // draw frontface second
    glCullFace(GL_BACK);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glBegin(GL_TRIANGLES);
    // top
    glNormal3fv(&fovNormals[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[2][0]);
    glVertex3fv(&fovVertices[1][0]);
    // bottom
    glNormal3fv(&fovNormals[1][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[3][0]);
    glVertex3fv(&fovVertices[4][0]);
    // left
    glNormal3fv(&fovNormals[2][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[1][0]);
    glVertex3fv(&fovVertices[3][0]);
    // right
    glNormal3fv(&fovNormals[3][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3fv(&fovVertices[0][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[4][0]);
    glVertex3fv(&fovVertices[2][0]);
    glEnd();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
    glVertex3fv(&fovVertices[0].x);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[1][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
    glVertex3fv(&fovVertices[0].x);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[2][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
    glVertex3fv(&fovVertices[0].x);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[3][0]);
    glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
    glVertex3fv(&fovVertices[0].x);
    glColor4f(0.5f, 0.5f, 0.5f, 0.0f);
    glVertex3fv(&fovVertices[4][0]);
    glEnd();

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



///////////////////////////////////////////////////////////////////////////////
// compute vertices for FOV
///////////////////////////////////////////////////////////////////////////////
void ModelGL::computeFovVertices(float fov)
{
    const float DEC2RAD = 3.141592f / 180.0f;
    const float DIST = 11.0f;
    float halfFov = fov * 0.5f * DEG2RAD;
    float ratio = 1.0f;

    // compute 5 vertices of the fov
    // origin
    fovVertices[0].x = 0;
    fovVertices[0].y = 0;
    fovVertices[0].z = 0;

    // top-left
    fovVertices[1].x = tanf(halfFov * ratio) * DIST;
    fovVertices[1].y = tanf(halfFov) * DIST;
    fovVertices[1].z = DIST;

    // top-right
    fovVertices[2].x =-tanf(halfFov * ratio) * DIST;
    fovVertices[2].y = tanf(halfFov) * DIST;
    fovVertices[2].z = DIST;

    // bottom-left
    fovVertices[3].x = tanf(halfFov * ratio) * DIST;
    fovVertices[3].y =-tanf(halfFov) * DIST;
    fovVertices[3].z = DIST;

    // bottom-right
    fovVertices[4].x =-tanf(halfFov * ratio) * DIST;
    fovVertices[4].y =-tanf(halfFov) * DIST;
    fovVertices[4].z = DIST;

    // compute normals
    // top
    fovNormals[0] = (fovVertices[2] - fovVertices[0]).cross(fovVertices[1] - fovVertices[0]);
    fovNormals[0].normalize();

    // bottom
    fovNormals[1] = (fovVertices[3] - fovVertices[0]).cross(fovVertices[4] - fovVertices[0]);
    fovNormals[1].normalize();

    // left
    fovNormals[2] = (fovVertices[1] - fovVertices[0]).cross(fovVertices[3] - fovVertices[0]);
    fovNormals[2].normalize();

    // right
    fovNormals[3] = (fovVertices[4] - fovVertices[0]).cross(fovVertices[2] - fovVertices[0]);
    fovNormals[3].normalize();
}



