#version 330 core

in vec4 gColor;
out vec4 color;

void main()
{
	//color = gColor;
	//color = vec4(gColor.xyz*gColor.a,gColor.a);
	color = gColor*gColor.a;
}