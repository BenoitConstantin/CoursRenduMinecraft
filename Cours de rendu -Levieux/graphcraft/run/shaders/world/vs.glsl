#version 400

//LE MONDE

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 nmat;
uniform int reflexionMapPass;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in float vs_type_in;
layout(location=2) in vec3 vs_normal_in;
layout(location=3) in vec2 vs_uv_in;

//Variables en sortie
out vec3 normal;
out vec4 color;
out vec4 worldPos;

uniform float slider_0;
uniform float elapsed;
uniform float slider_4;
uniform float water_height;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0

flat out int type;
out vec2 uvModel;


float noiseWater(vec2 v)
{
	return -5 + (sin(v.x/30 + 0.8*elapsed)*4 + sin((v.x+v.y)/15 + 0.1*elapsed)/2.0 + sin((v.x+v.y)/7 + 0.02*elapsed)/10.0)*20*slider_4;
}

void main()
{
	// Transforming The Vertex
	vec4 posIn = vec4(vs_position_in,1.0);
	vec4 posWorld = m * posIn;
	vec4 posCam = v * posWorld;

	if(reflexionMapPass > 0)
		gl_ClipDistance[0] = posWorld.z - water_height;
	else
		gl_ClipDistance[0] = 1;
	
	//Deformation rigolote
	//posIn.z = posIn.z + sin(posWorld.x/30.0 + elapsed)*10.0;
	
	float distXZ = length(posCam.xz);
	
	
	if(vs_type_in == CUBE_EAU)
	{
		posIn.z = posIn.z + noiseWater(posWorld.xy);
	}
	
//Deformation	
	//posIn.z = posIn.z - pow(distXZ/(10+30*slider_0),2);
		
	//gl_Position = p * v * m * vec4(vs_position_in,1.0);;
	gl_Position = p * v * m * posIn;

	if(vs_type_in == CUBE_HERBE)
		color = vec4(0.15,0.35,0,1);
	if(vs_type_in == CUBE_TERRE)
		color = vec4(0.9,0.6,0.2,1);
	if(vs_type_in == CUBE_EAU)
		color = vec4(0,0.2,0.3,0.6);

	normal = normalize(vec3(nmat*vec4(vs_normal_in,1.0)));
	worldPos = posWorld;
	type = int(vs_type_in);
	uvModel = vs_uv_in;
}