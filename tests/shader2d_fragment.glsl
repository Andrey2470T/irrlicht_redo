#version 330 core

uniform sampler2D mTexture0;

in vec3 vColor;
in vec2 vUV;

out vec4 outColor;

void main(void)
{
	vec4 texColor = texture(mTexture0, vUV);
	outColor = vec4(texColor.rgb * vColor, texColor.a);
}
