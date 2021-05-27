#include "raytrace.hpp"
#include "../helpful.hpp"

Shader_raytrace::Shader_raytrace()
{

    checkGlError("Error at start of shader setup");


    const char* vsText = textFileRead("shaders/raytrace/shader.vert");
    const char* fsText = textFileRead("shaders/raytrace/shader.frag");


    std::string vsource = ShaderManager::getVersion() + "\n";
    std::string fsource = ShaderManager::getVersion() + "\n";

    vsource += ShaderManager::getGlobalSettingsUniform();

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

    bindGlobalUniformBlock(shader_id);


    uniforms.sphereBinding = ShaderManager::getBlockBinding("raytrace sphere");
    GLuint sphereIndex = glGetUniformBlockIndex(shader_id, "settings");


    glUniformBlockBinding(shader_id, sphereIndex, uniforms.sphereBinding);

    checkGlError("Error after shader setup");
}

