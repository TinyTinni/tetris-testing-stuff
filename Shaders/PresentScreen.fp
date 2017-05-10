#version 330 core

uniform sampler2DRect  uTexture;
out vec4 color;

void main()
{
	color = texture(uTexture,gl_FragCoord.xy);
}