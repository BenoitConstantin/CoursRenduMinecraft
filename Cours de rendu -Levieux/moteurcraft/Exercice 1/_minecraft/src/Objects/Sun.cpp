#include "Sun.h"

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"
#include <time.h> 
#include <math.h>

Sun::Sun(time_t time)
{
	tm* t = gmtime(&time);

	m_position = NYVert3Df(-20, 0, 25);
	m_offset = m_position;

	m_position.rotate(NYVert3Df(1, 0, 0), M_PI + 2.f*M_PI*(1.0f / 24.0f*t->tm_hour + 1.0f / 1440.0f * t->tm_min + 1.0f / 86400.0f*t->tm_sec));

	currentTime = time;

	diffuse[0] = 0.5f;
	diffuse[1] = 0.5f;
	diffuse[2] = 0.5f;
	diffuse[3] = 0;

	specular[0] = 0.5f;
	specular[1] = 0.5f;
	specular[2] = 0.5f;
	specular[3] = 0;

	ambient[0] = 0.3f;
	ambient[1] = 0.3f;
	ambient[2] = 0.3f;
	ambient[3] = 0;
}


Sun::~Sun()
{
}

void Sun::Draw()
{
	//On définit une lumière 

	glPushMatrix();
	glColor3d(sunColor.R, sunColor.V, sunColor.B);
	glTranslated(m_position.X, m_position.Y, m_position.Z);
	glutSolidSphere(1,20,20);
	glPopMatrix();
}

void Sun::EmitLight(NYRenderer* g_renderer)
{
	float position[4] = { m_position.X,m_position.Y,m_position.Z,1 }; // w = 1 donc c'est une point light (w=0 -> directionelle, point à l'infini)
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	NYColor skyColor;

	tm* t = gmtime(&currentTime);


	if (t->tm_hour >= 6 && t->tm_hour <= 18)
	{
		skyColor = blueSkyColor.interpolate(orangeSkyColor, abs(12.f * 3600.f - (t->tm_hour*3600.f + t->tm_min*60.f + t->tm_sec)) / (6.f * 3600.0f));
		sunColor = NYColor(1.f, 222.f / 255.f, 3.f / 255.f, 1.f);
	}
	else if (t->tm_hour > 18)
	{
		skyColor = NYColor(0, 0, 0, 1).interpolate(orangeSkyColor, (24.f*3600.f - (t->tm_hour*3600.f + t->tm_min*60.f + t->tm_sec)) / (6.f * 3600.0f)); //Midnight
		sunColor = NYColor(1.f, 1.f,1.f, 1.f);
	}
	else
	{
		skyColor = NYColor(0, 0, 0, 1).interpolate(orangeSkyColor, (t->tm_hour*3600.f + t->tm_min*60.f + t->tm_sec) / (6.f * 3600.0f)); //Midnight
		sunColor = NYColor(1.f, 1.f, 1.f, 1.f);
	}

	g_renderer->setBackgroundColor(skyColor);
}


void Sun::Update(NYCamera camera)
{
	float delta = g_tweak_time * NYRenderer::_DeltaTime;

	//m_position.rotate(NYVert3Df(1, 0, 0), delta / 86400.0f * 2.f*M_PI); //86400 est le nombre de secondes en une journée ( ;) )

	deltaTime += delta;

	float intPart;
	float fract = modf(deltaTime, &intPart);

	currentTime += (int)intPart;
	deltaTime -= intPart;

	m_position = NYVert3Df(0, 0, 0) + m_offset;
	tm* t = gmtime(&currentTime);
	m_position.rotate(NYVert3Df(1, 0, 0), M_PI + 2.f*M_PI*(1.0f / 24.0f*t->tm_hour + 1.0f / 1440.0f * t->tm_min + 1.0f / 86400.0f*t->tm_sec));

	m_position += camera._LookAt;
}
