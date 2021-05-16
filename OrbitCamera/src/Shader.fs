//片元着色器代码
#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec4 glcolor;
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 ambientlight;
uniform vec3 diffuselight;
uniform vec3 specularlight;

//
uniform vec3 ambientmaterial;
uniform vec3 diffusematerial;
uniform vec3 specluarmaterial;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 gridColor;
uniform bool grid;
uniform bool bobjectColor;
uniform bool bspecular;
uniform bool bmodel;
void main()
{
    //环境光
    float ambientStrength = 1;
    vec3 ambient = ambientStrength * ambientlight * ambientmaterial;

    //漫反射光
    vec3 norm = normalize(Normal);
    //vec3 rlightPos = mat3(model_matrix * view_matrix) * lightPos;
    vec3 rlightPos = normalize(lightPos);
    vec3 lightDir = normalize(rlightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuselight * diffusematerial;

    //镜面高光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * specularlight * specluarmaterial;
    if(grid){
        FragColor = gridColor;
    }
    else{
        if(bobjectColor){
            // vec3 result = (ambient + diffuse) * objectColor;

            vec3 result = ambient + diffuse;
            if(bspecular){
                result += specular;
            }
            FragColor = vec4(result, 1.0f);
        }
        else{
            FragColor = glcolor;
        }
    }
}