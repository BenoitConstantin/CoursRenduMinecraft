#version 120

uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;

float LinearizeDepth (float z)
{
	float n = 0.5;
	float f = 10000.0;
	return (2.0*n)/(f+n-z*(f-n));
}

void main (void)
{
	vec2 pixelSize = vec2(1.0/screen_width,1.0/screen_height);

	vec4 coord = gl_TexCoord[0];
	vec4 color = texture2D( TexColor , vec2(coord ) );
	float depth = texture2D( TexDepth , vec2(coord ) ).x;
	float lum = 0.333*color.r + 0.333*color.g +0.333*color.b;
	
	//Noir et Blanc
	/*if (gl_TexCoord[0].x > 0.5)
	{
		color.rgb = vec3(lum,lum,lum);
	}*/
	
	
	//Contours
	float distContour = 2;
	float depthAccum = 0;
	float nb = 0;
	
	for(float x = -distContour; x < distContour; x++)
		for(float y = -distContour; y < distContour; y++)
		{
			float d = texture2D( TexDepth , vec2(coord) + vec2(x,y)*pixelSize ).x;
			//depthAccum -= LinearizeDepth(d);
			depthAccum += LinearizeDepth(d);
			nb++;
		}	
	
	depthAccum /= nb;
	
	
	if(abs(depthAccum - depth) < 1.0-1.0/4.0 && abs(depthAccum - depth) > 1.0/4.0)
		color.rgb = vec3(0,0,0);
	
	/*depthAccum += nb*depth;
	depthAccum = pow(clamp(depthAccum*30,0,1),2);
	
	color.rgb += vec3(depthAccum,depthAccum,depthAccum)*0.15;*/
	
/*if(coord.x > 0.5);	
	color.rgb = sqrt(color.rgb);*/
	
	gl_FragColor = vec4(color.rgb,1.0);
}