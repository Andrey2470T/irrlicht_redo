#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 0) in vec4 color;
layout (location = 0) in vec2 uv;

out vec4 vColor;
out vec2 vUV;

void main()
{
	vColor = color
	vUV = uv;

	gl_Position = vec4(pos, 0.0, 1.0);
}
