#version 400

//OISEAU
uniform float slider_0;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mvp;
layout(location=0) in vec3 vs_position_in;

void main()
{
		
	// Transforming The Vertex
	vec4 posIn = vec4(vs_position_in,1.0);
	vec4 posWorld = m * posIn;
	vec4 posCam = v * posWorld;

	
	//Deformation rigolote
	//posIn.z = posIn.z + sin(posWorld.x/30.0 + elapsed)*10.0;
	
	posIn.z = sin(posWorld.x);
		
	//float distXZ = length(posCam.xz);
		
	//posIn.z = posIn.z - pow(distXZ/(10+30*slider_0),2);
	
	
	gl_Position = mvp * posIn;
	
}