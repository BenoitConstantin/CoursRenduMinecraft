#version 400

in vec3 normal;
in vec4 color;
in vec4 worldPos;

out vec4 p_color;

uniform vec3 light_pos;
uniform vec4 sun_color;
uniform float slider_1;
uniform float slider_2;
uniform float slider_3;
uniform float elapsed;
uniform vec3 cam_pos;
uniform float slider_4;
uniform float slider_5;
uniform float slider_6;
uniform int reflexionMapPass;
uniform float screen_width;
uniform float screen_height;

uniform sampler2D TexCustom_1;
uniform sampler2D TexCustom_2;
uniform sampler2D TexCustom_3;
uniform sampler2D TexReflec;
uniform float water_height;

flat in int type;
in vec2 uvModel;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0

float noiseWater(vec2 v)
{
	return -5 + (sin(v.x/30 + 0.8*elapsed)*4 + sin((v.x+v.y)/15 + 0.1*elapsed)/2.0 + sin((v.x+v.y)/7 + 0.02*elapsed)/10.0)*20*slider_4;
}

void main()
{	
	vec3 realNormal = normal;
	vec3 camDir = normalize(cam_pos - worldPos.xyz);	
	vec4 realColor = color;
	
	if(type == CUBE_EAU)
	{
		vec4 textNormal = texture2D( TexCustom_1 , uvModel );
		vec4 A = worldPos;
		vec4 B = worldPos + vec4(1,0,0,0);
		vec4 C = worldPos + vec4(0,1,0,0);
		
		A.z += noiseWater(A.xy);
		B.z += noiseWater(B.xy);
		C.z += noiseWater(C.xy);
		
		vec3 AB = normalize(B.xyz - A.xyz);
		vec3 AC = normalize(C.xyz - A.xyz);
		
		realNormal = cross(AB, AC);
		realNormal = mix(realNormal, normalize(textNormal.xyz), 1*slider_6);
		
		float fresnel = 1 - max(0,dot(normal , camDir));
		fresnel = pow(fresnel,10*slider_5);
		realColor.a = 0.3*(fresnel * color.a) + 0.7 * color.a;
		
		vec3 colReflec = vec3(0,0,0);	
		vec2 uvRefl = vec2(0,0);
		

		//Filtre gaussien
		float maxDistance  = pow(pow(4,2)+pow(4,2),0.5);
		
		for(float i =-4; i < 4; i++)
		{
			for(float j = -4; j < 4 ;j++)
			{
				uvRefl = vec2((gl_FragCoord.x+i)/screen_width, (gl_FragCoord.y+j)/screen_height);
				uvRefl.y = 1 - uvRefl.y;
				uvRefl -= realNormal.xy/(30) ;
			
			
				float distance = pow(pow(i,2)+pow(j,2),0.5)/ maxDistance;
				//Fonction de lissage
				float lisseFonction = 6 * (pow((1 - distance), 5)) - 15*(pow((1 - distance) , 4)) + 10 * (pow((1 - distance) , 3));
				
				colReflec += texture2D( TexReflec , uvRefl ).xyz*lisseFonction/16;
			}	
		}
		
		
		realColor.xyz = mix(realColor.xyz, colReflec, 0.4);	
	}		
	else if(type == CUBE_TERRE)
	{
		realColor = texture2D(TexCustom_2, uvModel);
	}
	else if (type == CUBE_HERBE)
	{
		if(realNormal.z < 0.9)
			realColor = texture2D(TexCustom_3, -uvModel);
	}

	//p_color = vec4(realNormal,1);	
	vec3 lightDir = normalize(light_pos - cam_pos);
	
	float diffuse = max(0,dot(realNormal, lightDir));
	
	//specular
	vec3 halfVec = normalize(camDir + lightDir);
	
	float specular = max(0,dot(halfVec, realNormal));
	specular = 10*slider_3*pow(specular, 300*slider_2);
	
	if(type != CUBE_EAU)
		specular = 0;
	
	if (specular > 0.6)
		specular = 1;
	else if(specular > 0.2)
		specular = 0.4;
	
	p_color = (diffuse + (1.0-diffuse)*slider_1/2.0)*realColor;	
	p_color += specular * sun_color;	
	p_color.a = realColor.a;
	//p_color = color;
	
	//if(reflexionMapPass < 0.5)
		//p_color = vec4(colReflec,1.0);
}