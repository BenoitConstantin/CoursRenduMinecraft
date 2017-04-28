#include "Cube.h"

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"


Cube::Cube()
{
	m_position = NYVert3Df(0, 0, 0);
	m_rotation = NYVert3Df(0, 0, 0);
}

void Cube::Draw()
{

	//Diffuse
	GLfloat materialDiffuse[] = { 0,0,0,1 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//Speculaire
	GLfloat whiteSpecularMaterial[] = { 0.8, 0.8, 0.8,1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	GLfloat mShininess = 100;
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	//Emissive
	GLfloat emissive[] = { 0.1, 0.1, 0.1,1.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);

	//Ambient
	GLfloat materialAmbient[] = { 0.1, 0.1, 0.1,1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient); 


	glPushMatrix();

	glTranslated(m_position.X, m_position.Y, m_position.Z);

	glRotated(m_rotation.X, 1, 0, 0);
	glRotated(m_rotation.Y, 0, 1, 0);
	glRotated(m_rotation.Z, 0, 0, 1);

	//Face 1
	glBegin(GL_QUADS);

	materialDiffuse[0] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(0, 0, +1);

	glVertex3d(- 0.5f, -0.5f,  +0.5f);
	glVertex3d( + 0.5f ,  -0.5f ,  +0.5f);
	glVertex3d( + 0.5f,  + 0.5f, +0.5f);
	glVertex3d( - 0.5f,  + 0.5f,  +0.5f);


	//Face2
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(-1, 0, 0);

	glVertex3d( - 0.5f,  - 0.5f,  + 0.5f);
	glVertex3d( - 0.5f,  + 0.5f,  + 0.5f);
	glVertex3d( - 0.5f,  + 0.5f,  - 0.5f);
	glVertex3d( - 0.5f,  - 0.5f,  - 0.5f);


	//Face3
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(+1, 0, 0);

	glVertex3d( + 0.5f,  - 0.5f,  + 0.5f);
	glVertex3d( + 0.5f,  - 0.5f,  - 0.5f);
	glVertex3d( + 0.5f,  + 0.5f,  - 0.5f);
	glVertex3d( + 0.5f,  + 0.5f,  + 0.5f);


	//Face4
	materialDiffuse[2] = 0;
	materialDiffuse[0] = 1;
	materialDiffuse[1] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(0, 0, -1);

	glVertex3d( - 0.5f,  - 0.5f,  - 0.5f);
	glVertex3d( - 0.5f,  + 0.5f,  - 0.5f);
	glVertex3d( + 0.5f,  + 0.5f,  - 0.5f);
	glVertex3d( + 0.5f,  - 0.5f,  - 0.5f);


	//Face5
	materialDiffuse[1] = 0;
	materialDiffuse[0] = 1;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(0, +1, 0);

	glVertex3d(+0.5f, +0.5f, -0.5f);
	glVertex3d(-0.5f, +0.5f, -0.5f);
	glVertex3d(-0.5f, +0.5f, +0.5f);
	glVertex3d(+0.5f, +0.5f, +0.5f);


	//Face6
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 1;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//On set la normale
	glNormal3f(0, -1, 0);

	glVertex3d(+0.5f, -0.5f, -0.5f);
	glVertex3d(-0.5f, -0.5f, -0.5f);
	glVertex3d(-0.5f, -0.5f, +0.5f);
	glVertex3d(+0.5f, -0.5f, +0.5f);


	glEnd(); 
	glPopMatrix();
}

void Cube::Update()
{
//	m_rotation += NYVert3Df(60 * NYRenderer::_DeltaTime, 120 * NYRenderer::_DeltaTime, 80* NYRenderer::_DeltaTime);
}

Cube::~Cube()
{
}
