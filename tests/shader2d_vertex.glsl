#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 uv;

out vec3 vColor;
out vec2 vUV;

void main(void)
{
	vColor = color.rgb;
	vUV = uv;

	gl_Position = vec4(pos, 0.0, 1.0);
}
