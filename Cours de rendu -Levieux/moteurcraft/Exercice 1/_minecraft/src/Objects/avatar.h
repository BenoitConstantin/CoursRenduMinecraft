#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "world.h"

class NYAvatar
{
	public :
		NYVert3Df Position;
		NYVert3Df Speed;

		bool Move;
		bool Jump;
		float Height;
		float CurrentHeight;
		float Width;
		bool avance;
		bool recule;
		bool gauche;
		bool droite;
		bool Standing;
		bool InWater;
		bool Crouch;
		bool Run;

		NYCamera * Cam;
		NYWorld * World;

		NYTimer _TimerStanding;

		NYWorld::Collision collision = NYWorld::Collision::NONE;


		NYAvatar(NYCamera * cam, NYWorld * world)
		{
			Position = NYVert3Df((MAT_SIZE_CUBES*NYCube::CUBE_SIZE) / 2, (MAT_SIZE_CUBES*NYCube::CUBE_SIZE) / 2, (MAT_HEIGHT_CUBES*NYCube::CUBE_SIZE));
			Height = 0.9;
			CurrentHeight = Height;
			Width = 0.9;
			Cam = cam;
			avance = false;
			recule = false;
			gauche = false;
			droite = false;
			Standing = false;
			Jump = false;
			World = world;
			InWater = false;
			Crouch = false;
			Run = false;
		}


		void render(void)
		{
			glPushMatrix();
			glTranslated(Position.X, Position.Y, Position.Z);
			glColor3f(0, 0, 0);
			glutSolidCube(Width);
			glPopMatrix();
		}

	/*	void update(float elapsed)
		{
			collision = World->collide_with_world(Position);

			switch (collision)
			{
				case NYWorld::Collision::NY_COLLIDE_BOTTOM: Position.Z = max(2,World->_MatriceHeights[(int)Position.X][(int)Position.Y] +2); Speed.Z = 20; break;
			}


			Speed.Z -= 15*elapsed;
			Position += Speed * elapsed;
			Position.Z -= 15 / 2.f*elapsed*elapsed;
		}*/



		void update(float elapsed)
		{
			//Par defaut, on applique la gravité (-100 sur Z), la moitie si dans l eau
			NYVert3Df force = NYVert3Df(0, 0, -1) * 100.0f / 20.0f;
			if (InWater)
				force = NYVert3Df(0, 0, -1) * 5.0f / 20.0f;

			float lastheight = CurrentHeight;
			CurrentHeight = Height;
			if (Crouch)
				CurrentHeight = Height / 2;

			//Pour ne pas s'enfoncer dans le sol en une frame quand on se releve
			if (CurrentHeight > lastheight)
				Position.Z += Height / 4;

			//Si l'avatar n'est pas au sol, alors il ne peut pas sauter
			/*if (!Standing && !InWater) //On jump tout le temps
			Jump = false;*/

			float accel = 400 / 20.0f;
			if (Crouch)
				accel = 200 / 20.0f;
			if (!Standing)
				accel = 50 / 20.0f;
			if (Run)
				accel = 800 / 20.0f;

			NYVert3Df forward(Cam->_Direction.X, Cam->_Direction.Y, 0);
			forward.normalize();
			NYVert3Df right(Cam->_NormVec.X, Cam->_NormVec.Y, 0);
			right.normalize();

			//On applique les controles en fonction de l'accélération
			if (avance)
				force += forward * accel;
			if (recule)
				force += forward * -accel;
			if (gauche)
				force += right * -accel;
			if (droite)
				force += right * accel;


			//On applique le jump
			if (Jump)
			{
				force += NYVert3Df(0, 0, 1) * 55.0f/10.0f / elapsed; //(impulsion, pas fonction du temps)
				Jump = false;
			}

			//On applique les forces en fonction du temps écoulé
			Speed += force * elapsed;

			//On met une limite a sa vitesse horizontale
			float speedmax = 70 / 20.0f;
			if (Crouch)
				speedmax = 45 / 20.0f;
			if (!Standing)
				speedmax = 70 / 20.0f;
			if (Run)
				speedmax = 140 / 20.0f;

			NYVert3Df horSpeed = Speed;
			horSpeed.Z = 0;
			if (horSpeed.getSize() > speedmax)
			{
				horSpeed.normalize();
				horSpeed *= speedmax;
				Speed.X = horSpeed.X;
				Speed.Y = horSpeed.Y;
			}

			//On le déplace, en sauvegardant son ancienne position
			NYVert3Df oldPosition = Position;
			Position += (Speed * elapsed);

			//Log::log(Log::ENGINE_INFO, ("zS " + toString(Speed.Z)).c_str());

			if (_TimerStanding.getElapsedSeconds() > 0.01)
				Standing = false;

			for (int pass = 0; pass<2; pass++)
			{
				for (int i = 0; i < 6; i++)
				{
					float valueColMin = 0;
					NYAxis axis = World->getMinCol(Position, Speed, Width, CurrentHeight, valueColMin, i<3);
					//Log::log(Log::ENGINE_INFO,"check");
					if (axis != 0)
					{
						//valueColMin = nymax(nyabs(valueColMin), 0.0001f) * (valueColMin > 0 ? 1.0f : -1.0f);
						if (axis & NY_AXIS_X)
						{
							//Log::log(Log::ENGINE_INFO,("x " + toString(valueColMin)).c_str());
							Position.X += valueColMin + 0.001*nysign(valueColMin);
							Speed.X = 0;
						}
						if (axis & NY_AXIS_Y)
						{
							//Log::log(Log::ENGINE_INFO, ("y " + toString(valueColMin)).c_str());
							Position.Y += valueColMin + 0.001*nysign(valueColMin);
							Speed.Y = 0;
						}
						if (axis & NY_AXIS_Z)
						{
							//Log::log(Log::ENGINE_INFO, ("z " + toString(valueColMin)).c_str());
							Speed.Z = 0;
							Position.Z += valueColMin + 0.001*nysign(valueColMin);
							Standing = true;
							_TimerStanding.start();
						}
					}
				}
			}


			int x = (int)(Position.X / NYCube::CUBE_SIZE);
			int y = (int)(Position.Y / NYCube::CUBE_SIZE);
			int z = (int)(Position.Z / NYCube::CUBE_SIZE);


			//Escaliers
			float floatheight = 1.0f;
			float zpied = Position.Z - (Height / 2.0f);
			float zfloatpied = zpied - floatheight;
			int izCubeDessousFloat = (int)((zfloatpied) / NYCube::CUBE_SIZE);
			float zCubeDessous2Float = zfloatpied - NYCube::CUBE_SIZE;
			int izCubeDessous2Float = (int)(zCubeDessous2Float / NYCube::CUBE_SIZE);


			//Si on est dans l'eau
			InWater = false;
			if (World->getCube(x, y, z)->_Type == CUBE_EAU)
				InWater = true;

			if (InWater)
			{
				//Standing = true;
				Speed *= pow(0.01f, elapsed);
			}

			if (Standing)
				Speed *= pow(0.01f, elapsed);

		}

		int nysign(float value)
		{
			if (value < 0)
				return -1;
			else if (value > 0)
				return 1;
			else
				return 0;
		}
};

#endif