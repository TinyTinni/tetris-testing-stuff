#version 330 core

uniform sampler2D  uTexture;

in vec2 vTexCoords;
out vec4 color;

void main()
{
	//color = gColor;
	//color = vec4(gColor.xyz*gColor.a,gColor.a);
	color = texture(uTexture,vTexCoords).x*vec4(1.0,1.0,1.0,1.0);
}