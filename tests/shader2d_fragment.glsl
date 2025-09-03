#version 330 core

uniform sampler2D mTexture0;

in vec4 vColor;
in vec2 vUV;

out vec4 outColor;

void main()
{
	outColor = vec4(vColor.rgb * texture2D(mTexture0, vUV).rgb, 1.0);
}
