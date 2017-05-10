#version 330

layout (points) in; // position should be in view space (cameraposition as origin)
layout (triangle_strip, max_vertices = 8) out;

uniform mat4 uProjection;

//should be a uniform variable
const vec3 upVec = vec3(0.0,1.0,0.0);
const float quadSize = 0.05;


in vec3 vColor[];

out vec4 gColor;

void main()
{

	vec4 pointPosition = gl_in[0].gl_Position;
	vec3 camVec = pointPosition.xyz;
	vec3 rightVec = vec3(1.0,0.0,0.0);//normalize(cross(upVec,camVec));

	vec4 rightOffset = quadSize * vec4(rightVec,0.0);
	vec4 upOffset = quadSize * vec4(upVec,0.0);

	vec3 pointColor = vColor[0];

	const float alpha = 0.3;


	// draws a quad
	gl_Position = uProjection*(pointPosition + rightOffset - upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();
    gl_Position = uProjection*(pointPosition + rightOffset + upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();

	gl_Position = uProjection*(pointPosition);
    gColor = vec4(0.0);
    EmitVertex();

	gl_Position = uProjection*(pointPosition - rightOffset + upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();

	gl_Position = uProjection*(pointPosition - rightOffset - upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();

	EndPrimitive();

	gl_Position = uProjection*(pointPosition);
    gColor = vec4(0.0);
    EmitVertex();

	gl_Position = uProjection*(pointPosition + rightOffset - upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();

	gl_Position = uProjection*(pointPosition - rightOffset - upOffset);
    gColor = vec4(pointColor,alpha);
    EmitVertex();

	EndPrimitive();
}
