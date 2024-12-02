#version 330 core

in vec3 FragPos; // 프래그먼트 위치 (월드 좌표계)
in vec3 Normal;  // 법선 벡터
in vec3 objectColor; // 물체 색상

out vec4 FragColor; // 출력 색상

uniform vec3 lightColor; // 조명 색상
uniform vec3 lightPos;   // 조명 위치
uniform vec3 viewPos;    // 카메라 위치
uniform bool light_on;

void main()
{
    // Ambient (주변광)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse (난반사광)
    vec3 norm = normalize(Normal); // 법선 벡터 정규화
    vec3 lightDir = normalize(lightPos - FragPos); // 조명 방향
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    // Specular (반사광)
    int shininess = 128;
    vec3 viewDir = normalize(viewPos - FragPos); // 뷰 방향
    vec3 reflectDir = reflect(-lightDir, norm); // 반사 벡터
    float spec = max(dot(viewDir, reflectDir), 0.0);
    spec = pow(spec, shininess);
    vec3 specular = spec * lightColor;

    // 최종 색상 계산
    if(light_on){
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }else{
        vec3 result = (ambient) * objectColor;
        FragColor = vec4(result, 1.0);
    }
}
