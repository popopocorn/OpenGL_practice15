#version 330 core

layout(location = 0) in vec3 vPos;          // 정점 위치
layout(location = 1) in vec3 in_color;      // 입력 색상

out vec3 out_color;                         // 프래그먼트 셰이더로 전달할 색상

uniform mat4 uModelMatrix;                  // 이동 및 회전 변환을 위한 모델 행렬

void main()
{
    // 최종 위치 계산: 모델 행렬을 통해 정점 변환
    gl_Position = uModelMatrix * vec4(vPos, 1.0);
    out_color = in_color;                    // 색상 전달
}
