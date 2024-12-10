#version 330 core

in vec3 FragPos; // �����׸�Ʈ ��ġ (���� ��ǥ��)
in vec3 Normal;  // ���� ����
in vec3 objectColor; // ��ü ����

out vec4 FragColor; // ��� ����

uniform vec3 lightColor; // ���� ����
uniform vec3 lightPos;   // ���� ��ġ
uniform vec3 viewPos;    // ī�޶� ��ġ
uniform bool light_on;
uniform bool glass;

void main()
{
    // Ambient (�ֺ���)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse (���ݻ籤)
    vec3 norm = normalize(Normal); // ���� ���� ����ȭ
    vec3 lightDir = normalize(lightPos - FragPos); // ���� ����
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    // Specular (�ݻ籤)
    int shininess = 32;
    vec3 viewDir = normalize(viewPos - FragPos); // �� ����
    vec3 reflectDir = reflect(-lightDir, norm); // �ݻ� ����
    float spec = max(dot(viewDir, reflectDir), 0.0);
    spec = pow(spec, shininess);
    vec3 specular = spec * lightColor;

    // ���� ���� ���
    if(light_on){
        if(glass){
            vec3 result = (ambient + diffuse) * objectColor;
            FragColor = vec4(result, 0.5);
        }else{
            vec3 result = (ambient + diffuse) * objectColor;
            FragColor = vec4(result, 1.0);
        }

    }else{
        if(glass){
            vec3 result = (ambient) * objectColor;
            FragColor = vec4(result, 0.5);
        }
        else{
            vec3 result = (ambient) * objectColor;
            FragColor = vec4(result, 1.0);
        }
    }
}
