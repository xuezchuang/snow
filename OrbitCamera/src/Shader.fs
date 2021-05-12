//片元着色器代码
#version 330 core
in vec3 Normal;
in vec3 FragPos;
in mat4 model_matrix;
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 gridColor;
uniform bool grid;
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
    vec3 ab;
    if(grid){
        FragColor = gridColor;
    }
    else{
        // vec3 result = (ambient + diffuse) * objectColor;
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0f);
    }
}