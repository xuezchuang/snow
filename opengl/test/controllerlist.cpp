#include <stdlib.h>
#include <GL/glew.h>
//#include <unistd.h>
#include "controllerlist.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Game.hpp"
#include "Shader/SwitchShader.hpp"
#include "Dimension.hpp"
#include "Shader/allShaders/raytrace.hpp"
#include "helpful.hpp"




SceneController::SceneController()
{
    glDisable(GL_DEPTH_TEST);


    ShaderManager::bind(SH_raytrace);

    glGenBuffers(1, &ubo);


	glm::vec4 spheres[] = {
		glm::vec4(0,0,15,3),
		glm::vec4(-5,0,10,1),
		glm::vec4(2,1,7,1),
		glm::vec4(3,-1,7,1),
	};


	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 4, spheres, GL_DYNAMIC_DRAW);

	GLuint bindPoint = ShaderManager::getBoundUniform<Shader_raytrace>().sphereBinding;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ubo);

    

    float bounds = Dimension::getRight();

    float quad[] = {
        -bounds, 1.0f,
        bounds, 1.0f,
        bounds, -1.0f,
        -bounds, -1.0f,
        //bounds, -1.0f,
        //bounds, 1.0f
    };


    float uv[] = {
		-bounds, 1.0f,
		bounds, 1.0f,
		bounds, -1.0f,
		-bounds, -1.0f,
        //bounds, -1.0f,
        //bounds, 1.0f
    };



    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * 2, quad, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * 2, uv, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);


}
int z = 0;
void SceneController::update()
{
    fCount++;

    checkGlError("Error at start of frame");


    glViewport( 0, 0, Dimension::getWidth(), Dimension::getHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    

    glm::vec4 spheres[] = {
        glm::vec4(0,0,15 + 2*cos(fCount/100.f),3),
        glm::vec4(-5,0,10,1),
        glm::vec4(2 + sin(fCount/200.f),1,7,1),
        glm::vec4(3,-1,2,1)
    };
    for (int i = 0; i < 4; i++)
        spheres[i][2] += 0.1*z;

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4)*4, spheres, GL_DYNAMIC_DRAW);


    Game::matrix().useFloat();

    checkGlError("Error using matrix");

    glDisable(GL_DEPTH_TEST);

    ShaderManager::bind(SH_raytrace);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //glDrawArrays(GL_TRIANGLES, 0, 6);


    Game::flip();



    glfwPollEvents();

    checkGlError("Error at end of frame");
}
bool bnew = 0;

void SceneController::keyboard(int key, int action)
{
    if (action == 0)
        return;
    if (key == 334)
    {
        z++;
        //for (int i = 0; i < 4; i++)
            //spheres[i][0] += 1;
    }
    else if (key == 66)
    {
        bnew = !bnew;
		glUniform1i(glGetUniformLocation(3, "bnew"),bnew);
    }
    else
    {
        z--;
        //for (int i = 0; i < 4; i++)
            //spheres[i][0] -= 1;
    }

}