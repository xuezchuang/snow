#include "raytrace.hpp"
#include "../helpful.hpp"
#include "../Math/Quaternion.hpp"

const Vector3f Vector3f::Zero = Vector3f(0.0f, 0.0f, 0.0f);

const Vector3f Vector3f::One = Vector3f(1.0f, 1.0f, 1.0f);

const Vector3f Vector3f::Up = Vector3f(0.0f, 1.0f, 0.0f);

const Vector3f Vector3f::Right = Vector3f(1.0f, 0.0f, 0.0f);

const Vector3f Vector3f::Forward = Vector3f(0.0f, 0.0f, 1.0f);

const float MathUtility::PI = 3.141592653f;

const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 1.0f, 0.0f);

const Matrix3x3 Matrix3x3::Identity = Matrix3x3(Vector3f::Right, Vector3f::Up, Vector3f::Forward);

Shader_raytrace::Shader_raytrace()
{

    checkGlError("Error at start of shader setup");


    const char* vsText = textFileRead("shaders/raytrace/shader.vs");
    const char* fsText = textFileRead("shaders/raytrace/shader.fs");


    std::string vsource;// = ShaderManager::getVersion() + "\n";
    std::string fsource;// = ShaderManager::getVersion() + "\n";

    //vsource += ShaderManager::getGlobalSettingsUniform();

    vsource += vsText;
    fsource += fsText;



    checkGlError("Error during shader setup");

    std::ofstream log;
    getShaderLog(log, "raytrace");

    std::array<GLuint, 3> out = loadShaders(log,vsource,fsource);

    shader_id = out[0];
    shader_vp = out[1];
    shader_fp = out[2];

    glBindAttribLocation(shader_id, 0, "in_Vertex");
    glBindAttribLocation(shader_id, 1, "in_UV");
    //glBindFragDataLocation(shader_id, 0, "out_Scene");



    glAttachShader(shader_id, shader_fp);
    glAttachShader(shader_id, shader_vp);
    glLinkProgram(shader_id);
    glUseProgram(shader_id);

    

    //bindGlobalUniformBlock(shader_id);


    uniforms.sphereBinding = ShaderManager::getBlockBinding("raytrace sphere");
    GLuint sphereIndex = glGetUniformBlockIndex(shader_id, "settings");


    glUniformBlockBinding(shader_id, sphereIndex, uniforms.sphereBinding);


	Quaternion abc;
	abc.FromTo(Vector3f(0, 0, 1), Vector3f(0, 0, 0));
    glUniform1i(glGetUniformLocation(shader_id, "bnew"), 0);


    checkGlError("Error after shader setup");
}

