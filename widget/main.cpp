/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "vapp.h"
#include "vutils.h"

#include "vmath.h"

#include "LoadShaders.h"

#include <stdio.h>
#include "interface_intern.h"
using namespace vmath;

// Define USE_PRIMITIVE_RESTART to 0 to use two separate draw commands
#define USE_PRIMITIVE_RESTART 1

BEGIN_APP_DECLARATION(DrawCommandExample)
// Override functions from base class
virtual void Initialize(const char* title);
virtual void Display(bool auto_redraw);
virtual void Finalize(void);
virtual void Reshape(int width, int height);

// Member variables
float aspect;
GLuint render_prog;
GLuint vao[1];
GLuint vbo[1];
GLuint ebo[1];

GLint modelview_matrix_loc;
GLint project_matrix_loc;
GLint parameters_loc;
GLint test_loc;
uiWidgetBaseParameters widgetParam;
END_APP_DECLARATION()

DEFINE_APP(DrawCommandExample, "Drawing Commands Example")

void DrawCommandExample::Initialize(const char* title)
{
    base::Initialize(title);

    static ShaderInfo shader_info[] =
    {
        //{ GL_VERTEX_SHADER, "./Shader/primitive_restart.vs.glsl" },
        //{ GL_FRAGMENT_SHADER, "./Shader/primitive_restart.fs.glsl" },
		{ GL_VERTEX_SHADER, "./Shader/gpu_shader_2D_widget_base_vert.glsl" },
		{ GL_FRAGMENT_SHADER, "./Shader/gpu_shader_2D_widget_base_frag.glsl" },
		{ GL_NONE, NULL }
	};

    render_prog = LoadShaders(shader_info);

    glUseProgram(render_prog);

    // "model_matrix" is actually an array of 4 matrices
    //modelview_matrix_loc = glGetUniformLocation(render_prog, "ModelViewProjectionMatrix");
    modelview_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
    project_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
    parameters_loc = glGetUniformLocation(render_prog, "parameters");
    test_loc = glGetUniformLocation(render_prog, "btest");
	// Indices for the triangle strips
	static const GLushort vertex_indices[] =
	{
		0, 1,2,3
	};
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
    glClearColor(0.f, 0.f, 0.f, 1.0f);

}

/* keep in sync with shader */
#define MAX_WIDGET_BASE_BATCH 6
#define MAX_WIDGET_PARAMETERS 12

void DrawCommandExample::Display(bool auto_redraw)
{
    float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
    static float q = 0.0f;
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    mat4 model_matrix;

    // Setup
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate simple shading program
    glUseProgram(render_prog);

    // Set up the model and projection matrix
    vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 400.0f));
    glUniformMatrix4fv(project_matrix_loc, 1, GL_FALSE, projection_matrix);
    // Set up for a glDrawElements call
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    //// Draw Arrays...
    model_matrix = vmath::translation(-3.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(modelview_matrix_loc, 1, GL_FALSE, model_matrix);
    widgetParam.rect.xmin = -1.f;
    widgetParam.rect.xmax = 1.f;
    widgetParam.rect.ymin = -1.f;
    widgetParam.rect.ymax = 1.f;

    widgetParam.recti.xmin = -1.4f;
    widgetParam.recti.xmax = -1.f;
    widgetParam.recti.ymin = -1.f;
    widgetParam.recti.ymax = -1.f;
    glUniform4fv(parameters_loc, MAX_WIDGET_PARAMETERS, (float*)&widgetParam);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL, 1, 0, 0);
    base::Display();
}

void DrawCommandExample::Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(render_prog);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
}

void DrawCommandExample::Reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    aspect = float(height) / float(width);
}
