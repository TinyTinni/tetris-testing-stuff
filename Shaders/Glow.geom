#version 330

layout (lines) in; // position should be in view space (cameraposition as origin)
layout (triangle_strip, max_vertices = 20) out;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;
uniform float uGlowExpansion;

//should be a uniform variable
const vec3 upVec = vec3(0.0,1.0,0.0);
const float quadSize = 0.05;

in vec3 vColor[];
out vec4 gColor;

/*
generateQuad from line
generate glow
*/ 

void createSideQuad(vec4 startPos, vec4 endPos, vec4 startColor, vec4 endColor, vec4 expansion)
{
	gl_Position = uProjection*uModelMatrix*(startPos);
	gColor = startColor;
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(startPos + expansion);
	gColor = vec4(startColor.xyz,0.0);
	//gColor = vec4(0.0);
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(endPos);
	gColor = endColor;
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(endPos + expansion );
	gColor = vec4(endColor.xyz,0.0);
	//gColor = vec4(0.0);
    EmitVertex();	
	EndPrimitive();
}

void createLinkingFan(vec4 pos, vec4 from, vec4 to, vec4 posColor, vec4 upOffset)
{
	gl_Position = uProjection*uModelMatrix*(pos - upOffset);
	gColor = vec4(posColor.xyz,0.0);
	EmitVertex();
	gl_Position = uProjection*uModelMatrix*(from);
	//gColor = vec4(0.0);;
	gColor = vec4(posColor.xyz,0.0);
	EmitVertex();
	gl_Position = uProjection*uModelMatrix*(pos);
	gColor = posColor;
	//gColor.a /= gColor.a;
	EmitVertex();
	EndPrimitive();

	gl_Position = uProjection*uModelMatrix*(pos - upOffset);
	gColor = vec4(posColor.xyz,0.0);
	EmitVertex();
	gl_Position = uProjection*uModelMatrix*(to);
	//gColor = vec4(0.0);;
	gColor = vec4(posColor.xyz,0.0);
	EmitVertex();
	gl_Position = uProjection*uModelMatrix*(pos);
	gColor = posColor;
	//gColor.a /= gColor.a;
	EmitVertex();
	EndPrimitive();
}

void main()
{

	vec4 startPos = gl_in[0].gl_Position;
	vec4 startColor = vec4(vColor[0],1.0);//;
	vec4 startColorT = vec4(0.0);

	vec4 endPos = gl_in[1].gl_Position;
	vec4 endColor = vec4(vColor[1],1.0);
	vec4 endColorT = vec4(0.0);

	vec3 viewDirection = vec3(0.0,0.0,1.0);
	vec3 rightVec = normalize( cross(viewDirection,(startPos-endPos).xyz) );

	vec4 rightOffset = uGlowExpansion* vec4(rightVec,0.0);
	vec4 upOffset = normalize(startPos-endPos);


	// draw left an right quad
	createSideQuad(startPos,endPos,startColor,endColor,-rightOffset);
	createSideQuad(startPos,endPos,startColor,endColor,+rightOffset);
	createLinkingFan(startPos,startPos-rightOffset,startPos+rightOffset,startColor,-1.0*upOffset);
	createLinkingFan(endPos,endPos-rightOffset,endPos+rightOffset,endColor,1.0*upOffset);

}
