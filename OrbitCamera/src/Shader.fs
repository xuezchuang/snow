//ƬԪ��ɫ������
#version 330 core
in vec3 Normal;
in vec3 FragPos;
in mat4 model_matrix;
in vec4 glcolor;
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 gridColor;
uniform bool grid;
uniform bool bobjectColor;
void main()
{
    //������
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //�������
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //����߹�
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    if(grid){
        FragColor = gridColor;
    }
    else{
        if(bobjectColor){
            // vec3 result = (ambient + diffuse) * objectColor;
            vec3 result = (ambient + diffuse + specular) * objectColor;
            FragColor = vec4(result, 1.0f);
        }
        else{
            FragColor = glcolor;
        }

    

    }
}