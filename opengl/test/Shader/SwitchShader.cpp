#include "SwitchShader.hpp"
#include "allShaders/allShaders.hpp"


SwitchShader::~SwitchShader()
{}


void ShaderManager::init()
{
  bbCount = UB_USER;



  shaders[SH_raytrace] = new Shader_raytrace;

    glUseProgram(0);
    bound = 0;






    std::cout << "Set up shaders" << std::endl;

}

int ShaderManager::getBlockBinding(std::string name)
{
  auto it = blockBindings.find(name);
  if(it != blockBindings.end())
  {
     return it->second;
  }
  else
  {
    GLuint newBlock = bbCount;
    blockBindings[name] = newBlock;
    ++bbCount;
    return newBlock;
  }
}

std::string ShaderManager::getVersion()
{
    return "#version 140";
}

std::string ShaderManager::getGlobalSettingsUniform()
{
    return "\
layout( std140 ) uniform GlobalSettings {\n\
    mat4 matrix;\n\
};\n\
\n\
";
}

std::string ShaderManager::getViewSettingsUniform()
{
    return "\
layout( std140 ) uniform ViewSettings {\n\
    mat4 viewMatrix;\n\
};\n\
\n\
";
}

std::string ShaderManager::getLightSettingsUniform()
{

return "\
layout( std140 ) uniform LightSettings {\n\
    vec3 light[3];\n\
};\n\
";
}


std::map<int, SwitchShader*> ShaderManager::shaders;
int ShaderManager::bound;
std::map<std::string, GLuint> ShaderManager::blockBindings;
int ShaderManager::bbCount;
