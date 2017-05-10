#version 330

layout (lines) in; // position should be in view space (cameraposition as origin)
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;

//should be a uniform variable
const vec3 upVec = vec3(0.0,1.0,0.0);
const float quadSize = 0.05;

void main()
{

	vec4 startPos = gl_in[0].gl_Position;
	vec4 endPos = gl_in[1].gl_Position;
	vec3 viewDirection = vec3(0.0,0.0,1.0);
	vec3 rightVec = normalize( cross(viewDirection,(startPos-endPos).xyz) );

	vec4 rightOffset = 0.0125*vec4(rightVec,0.0);

	const float alpha = 0.3;


	// draws a quad
	gl_Position = uProjection*uModelMatrix*(startPos + rightOffset);
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(startPos - rightOffset);
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(endPos + rightOffset );
    EmitVertex();
	gl_Position = uProjection*uModelMatrix*(endPos - rightOffset );
    EmitVertex();	
	EndPrimitive();
}
