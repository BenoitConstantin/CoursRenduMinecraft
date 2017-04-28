//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"

#include "Objects/Cube.h"
#include "Objects/Sun.h"
#include <time.h> 
//Pour avoir le monde
#include "Objects/world.h"
#include "Objects/avatar.h"
#include "../base/my_physics.h"

NYRenderer * g_renderer = NULL;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;

//GUI 
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUILabel * LabelCam = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider;


//Forme
Sun* sun;
NYAvatar* avatar;

//Variable globale
NYWorld * g_world;
int profMax = -1;


bool translateWorld = false;

//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void update(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);

	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;
	g_nb_frames++;
	if(g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}

	LabelCam->Text = string("Camera Position : ") +string("(") + to_string((g_renderer->_Camera->_Position.X)) + string(",") + to_string(g_renderer->_Camera->_Position.Y) + string(",") + to_string(g_renderer->_Camera->_Position.Z) + string(") // ");
	LabelCam->Text += string("Camera Direction : ") + string("(") + to_string((g_renderer->_Camera->_Direction.X)) + string(",") + to_string(g_renderer->_Camera->_Direction.Y) + string(",") + to_string(g_renderer->_Camera->_Direction.Z) + string(")");

	sun->Update(*(g_renderer->_Camera));
	avatar->update(elapsed);

	if (g_renderer->_Camera->_Mode == NYCamera::Mode::THIRD_PERSON)
	{
		g_renderer->_Camera->_Position = avatar->Position + (g_renderer->_Camera->_Position - avatar->Position).normalize() * 20.0f;
		g_renderer->_Camera->setLookAt(avatar->Position);
	}
	else
		g_renderer->_Camera->moveTo(avatar->Position);


	//Rendu
	g_renderer->render(elapsed);
}



void tracePicking(NYVert3Df begin, NYVert3Df end)
{
	glLineWidth(2.5);
	glColor3d(1.0f, 0, 0);

	glBegin(GL_LINES);
	glVertex3d(begin.X, begin.Y, begin.Z);
	glVertex3d(end.X, end.Y, end.Z);
	glEnd();
}

void render2d(void)
{
	g_screen_manager->render();
}

void renderObjects(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glBegin(GL_LINES);
	glColor3d(1,0,0);
	glVertex3d(0,0,0);
	glVertex3d(10000,0,0);
	glColor3d(0,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,10000,0);
	glColor3d(0,0,1);
	glVertex3d(0,0,0);
	glVertex3d(0,0,10000);

	glEnd();


	sun->Draw();

	//A rajouter pour debug : rotation dans le temps
	//glRotatef(NYRenderer::_DeltaTimeCumul * 100, g_slider->Value*10.0f, 1, cos(NYRenderer::_DeltaTimeCumul));

	//On desactive le back face culling
	//glDisable(GL_CULL_FACE);

	//printf("%f\n", g_renderer->_Camera->_Position.X);
	NYVert3Df speed = avatar->Speed;
	tracePicking(avatar->Position, avatar->Position +(speed.normalize()*20));


/*	NYVert3Df speed = avatar->Speed;
	NYVert3Df intersect;
	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			if (My_Physics::IntersectCube(avatar->Position, avatar->Position + (speed.normalize() * 20), g_world->getCube(avatar->Position.X + i, avatar->Position.Y + j, avatar->Position.Z), &intersect))
			{
				glColor3d(1.0f, 0, 0);
				glPushMatrix();
				glTranslatef(intersect.X, intersect.Y, intersect.Z);
				glutSolidSphere(1.0, 50, 50);
				glPopMatrix();
			}
		}
	}*/


	//On active l'illumination 
	glEnable(GL_LIGHTING);
	//On active la light 0
	glEnable(GL_LIGHT0);


	//CUBE
	//Au lieu de rendre notre cube dans sa sph�re (mais on laisse le soleil)
	glPushMatrix();


	if (g_renderer->_Camera->_Mode == NYCamera::Mode::THIRD_PERSON)
		avatar->render();

	//Parce que le centre la position des cubes c'est leur centre :'(
	if(translateWorld)
		glTranslatef(0.5, 0.5, 0.5);

	//g_world->render_world_old_school();
	g_world->render_world_vbo();
	glPopMatrix();


}

void setLights(void)
{
	//Changement de la couleur de fond
	sun->EmitLight(g_renderer);
}

void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width,height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////

bool ctrlDown = false;
void specialDownFunction(int key, int p1, int p2)
{
	//On change de mode de camera
	if(key == GLUT_KEY_LEFT)
	{
	}

	if(key == GLUT_KEY_CTRL_L)
		ctrlDown = true;

}

void specialUpFunction(int key, int p1, int p2)
{
	if (key == GLUT_KEY_CTRL_L)
		ctrlDown = false;
}


void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	if(key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);	
		exit(0);
	}

	if (key == 'w')
		translateWorld = !translateWorld;

	if(key == 'f')
	{
		if(!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		} else if(g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0,0);
			g_fullscreen = false;
		}
	}

	if (key == 'v')
	{
		switch (g_renderer->_Camera->_Mode)
		{
		case(NYCamera::Mode::FIRST_PERSON):
			g_renderer->_Camera->_Mode = NYCamera::Mode::THIRD_PERSON; break;
		case(NYCamera::Mode::THIRD_PERSON) :
			g_renderer->_Camera->_Mode = NYCamera::Mode::FIRST_PERSON; break;
		}
	}

	if (key == '+')
		sun->g_tweak_time += 200;
	if (key == '-')
		sun->g_tweak_time-= 200;

	if (key == 'r')
	{
		profMax++;
		g_world->init_world(profMax, false);
		g_world->add_world_to_vbo();
	}

	if (key == 'l')
	{
		g_world->init_world(profMax, true);
		g_world->add_world_to_vbo();
	}

	if (key == 'z')
		avatar->avance = true;

	if (key == 'q')
		avatar->gauche = true;

	if (key == 'd')
		avatar->droite = true;

	if (key == 's')
		avatar->recule = true;

	if (key == 'a')
		avatar->Jump = true;

	if (key == 'e')
	{
		NYVert3Df speed = avatar->Speed;
		NYVert3Df inter;
		int xCube; int yCube; int zCube;
		if (g_world->getRayCollision(avatar->Position, avatar->Position + (speed.normalize() * 20), inter, xCube, yCube, zCube))
		{
			g_world->deleteCube(xCube, yCube, zCube);
			g_world->updateCube(xCube, yCube, zCube);
		}
	}
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{
	if (key == 'z')
		avatar->avance = false;

	if (key == 'q')
		avatar->gauche = false;

	if (key == 'd')
		avatar->droite = false;

	if (key == 's')
		avatar->recule = false;


	if (key == 'a')
		avatar->Jump = false;
}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{
	if (ctrlDown)
	{
		g_renderer->_Camera->moveLikeUnity(NYVert3Df(0, 0, dir*10));
	}
}

void mouseFunction(int button, int state, int x, int y)
{
	//Gestion de la roulette de la souris
	if((button & 0x07) == 3 && state)
		mouseWheelFunction(button,1,x,y);
	if((button & 0x07) == 4 && state)
		mouseWheelFunction(button,-1,x,y);

	//GUI
	g_mouse_btn_gui_state = 0;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_mouse_btn_gui_state |= GUI_MLBUTTON;
	
	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
}

int lastx;
int lasty;

void mouseMoveFunction(int x, int y, bool pressed)
{
	bool mouseTraite = false;

	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
	if(pressed && mouseTraite)
	{
		//Mise a jour des variables li�es aux sliders
	}

	if (pressed)
	{
		if (ctrlDown)
		{
			g_renderer->_Camera->moveLikeUnity(NYVert3Df(-0.5*(x - lastx), 0.5*(y - lasty), 0));
		}
		else
		{
			switch (g_renderer->_Camera->_Mode)
			{
			case(NYCamera::Mode::THIRD_PERSON): g_renderer->_Camera->rotateAround( 0.01*(x - lastx)); g_renderer->_Camera->rotateUpAround(0.01*(y - lasty)); break;
			case (NYCamera::Mode::FIRST_PERSON): g_renderer->_Camera->rotate(-0.005*(x - lastx)); g_renderer->_Camera->rotateUp(-0.005*(y - lasty)); break;
			}
		}
	}

	lastx = x;
	lasty = y;
}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,true);
}
void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,false);
}


void clickBtnParams (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}

/**
  * POINT D'ENTREE PRINCIPAL
  **/
int main(int argc, char* argv[])
{ 
	NYVert3Df intersect = NYVert3Df(0, 0, 0);
	My_Physics::SegmentPlanIntersect(NYVert3Df(0,0,0), NYVert3Df(2,0,0), NYVert3Df(1,0,0), NYVert3Df(1,0,0), &intersect);
	printf("Debug intersect plan 1 : \nSegment(1,0,0)  + Plan(1,0,0)\nResult : (%f,%f, %f)\n", intersect.X, intersect.Y, intersect.Z);
	printf(My_Physics::SegmentPlanIntersect(NYVert3Df(0, 0, 0), NYVert3Df(2, 0, 0), NYVert3Df(1, 0, 0), NYVert3Df(1, 0, 0), &intersect)? "true" : "false");
	printf("\n");

	My_Physics::SegmentPlanIntersect(NYVert3Df(0, 0, 0), NYVert3Df(0.1, 0, 0), NYVert3Df(1, 0, 0), NYVert3Df(1, 0, 0), &intersect);
	printf("Debug intersect plan 2 : \nSegment(1,0,0)  + Plan(1,0,0)\nResult : (%f,%f, %f)\n", intersect.X, intersect.Y, intersect.Z);
	printf(My_Physics::SegmentPlanIntersect(NYVert3Df(0, 0, 0), NYVert3Df(0.1, 0, 0), NYVert3Df(1, 0, 0), NYVert3Df(1, 0, 0), &intersect) ? "true" : "false");
	printf("\n");

	LogConsole::createInstance();

	int screen_width = 800;
	int screen_height = 600;

	glutInit(&argc, argv); 
	glutInitContextVersion(3,0);
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(screen_width,screen_height);
	glutInitWindowPosition (0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );

	glEnable(GL_MULTISAMPLE);

	Log::log(Log::ENGINE_INFO, (toString(argc) + " arguments en ligne de commande.").c_str());	
	bool gameMode = true;
	for(int i=0;i<argc;i++)
	{
		if(argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO,"Arg w mode fenetre.\n");
			gameMode = false;
		}
	}

	if(gameMode)
	{
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);
		
		char gameModeStr[200];
		sprintf(gameModeStr,"%dx%d:32@60",width,height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("MyNecraft");
		glutReshapeWindow(screen_width,screen_height);
	}

	if(g_main_window_id < 1) 
	{
		Log::log(Log::ENGINE_ERROR,"Erreur creation de la fenetre.");
		exit(EXIT_FAILURE);
	}
	
	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR,("Erreur init glew " + std::string((char*)glewGetErrorString(glewInitResult))).c_str());
		_cprintf("ERROR : %s",glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Affichage des capacit�s du syst�me
	Log::log(Log::ENGINE_INFO,("OpenGL Version : " + std::string((char*)glGetString(GL_VERSION))).c_str());

	glutDisplayFunc(update);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	glutIgnoreKeyRepeat(1);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRender2DFun(render2d);
	g_renderer->setLightsFun(setLights);
	g_renderer->setBackgroundColor(NYColor());
	g_renderer->initialise();

	//On applique la config du renderer
	glViewport(0, 0, g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);
	g_renderer->resize(g_renderer->_ScreenWidth,g_renderer->_ScreenHeight);
	
	//Ecran de jeu
	uint16 x = 10;
	uint16 y = 10;
	g_screen_jeu = new GUIScreen(); 

	g_screen_manager = new GUIScreenManager();
		
	//Bouton pour afficher les params
	BtnParams = new GUIBouton();
	BtnParams->Titre = std::string("Params");
	BtnParams->X = x;
	BtnParams->setOnClick(clickBtnParams);
	g_screen_jeu->addElement(BtnParams);

	y += BtnParams->Height + 1;

	LabelFps = new GUILabel();
	LabelFps->Text = "FPS";
	LabelFps->X = x;
	LabelFps->Y = y;
	LabelFps->Visible = true;
	g_screen_jeu->addElement(LabelFps);


	//Label camera
	LabelCam = new GUILabel();
	LabelCam->Text = "";
	LabelCam->X = 10;
	LabelCam->Y = 50;
	LabelCam->Visible = true;
	g_screen_jeu->addElement(LabelCam);

	//Ecran de parametrage
	x = 10;
	y = 10;
	g_screen_params = new GUIScreen();

	GUIBouton * btnClose = new GUIBouton();
	btnClose->Titre = std::string("Close");
	btnClose->X = x;
	btnClose->setOnClick(clickBtnCloseParam);
	g_screen_params->addElement(btnClose);

	y += btnClose->Height + 1;
	y+=10;
	x+=10;

	GUILabel * label = new GUILabel();
	label->X = x;
	label->Y = y;
	label->Text = "Param :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider = new GUISlider();
	g_slider->setPos(x,y);
	g_slider->setMaxMin(1,0);
	g_slider->Visible = true;
	g_screen_params->addElement(g_slider);

	y += g_slider->Height + 1;
	y+=10;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);
	
	//Init Camera
	g_renderer->_Camera->setPosition(NYVert3Df(10,10,10));
	g_renderer->_Camera->setLookAt(NYVert3Df(0,0,0));
	

	//Fin init moteur

	//Init application

	//Init Sun
	sun = new Sun(time(0));

	//A la fin du main, on genere un monde
	g_world = new NYWorld();
	g_world->init_world();
	g_world->add_world_to_vbo();

	//Init Avatar
	avatar = new NYAvatar(g_renderer->_Camera, g_world);

	//Init Timer
	g_timer = new NYTimer();
	
	//On start
	g_timer->start();

	glutMainLoop(); 

	return 0;
}


