#version 330 core

layout(location = 0) in vec3 vPos;          // ���� ��ġ
layout(location = 1) in vec3 in_color;      // �Է� ����

out vec3 out_color;                         // �����׸�Ʈ ���̴��� ������ ����

uniform mat4 uModelMatrix;                  // �̵� �� ȸ�� ��ȯ�� ���� �� ���

void main()
{
    // ���� ��ġ ���: �� ����� ���� ���� ��ȯ
    gl_Position = uModelMatrix * vec4(vPos, 1.0);
    out_color = in_color;                    // ���� ����
}
