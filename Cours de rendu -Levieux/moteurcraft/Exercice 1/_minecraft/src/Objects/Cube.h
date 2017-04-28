#pragma once

#include <stdio.h>

enum NYCubeType
{
	CUBE_HERBE,
	CUBE_EAU,
	CUBE_TERRE, 
	CUBE_AIR
};

class NYCube
{
	public :
		bool _Draw;
		NYCubeType _Type;
		static const int CUBE_SIZE = 1;

		NYCube()
		{
			_Draw = false;
			_Type = CUBE_AIR;
		}

		bool isSolid(void)
		{
			return (_Type != CUBE_AIR);
		}

		void saveToFile(FILE * fs)
		{
			fputc(_Draw ? 1 : 0,fs);
			fputc(_Type,fs);
		}

		void loadFromFile(FILE * fe)
		{
			_Draw = fgetc(fe) ? true : false;
			_Type = (NYCubeType)fgetc(fe);
		}

		void draw(int x,int y,int z)
		{
			glPushMatrix();

			float specular[4] = { 0.05f, 0.05f,0.05f, 1 };
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
			//glMaterialfv(GL_FRONT, GL_SHININESS, );

			float color[4] = { 0,0,0,1 };

			switch (_Type)
			{
				case NYCubeType::CUBE_HERBE: color[1] = 1; break;
				case NYCubeType::CUBE_EAU: color[2] = 1; break;
				case NYCubeType::CUBE_TERRE:
					color[0] = 0.3094117647;
					color[1] = 0.11764705882; break;
				case NYCubeType::CUBE_AIR: return;
			}

			glMaterialfv(GL_FRONT, GL_AMBIENT, color);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

			glTranslated(x, y, z);
			glutSolidCube(1);
			glPopMatrix();
		}

		void FillColorBuffer(int bufferCount, float _WorldCols[])
		{
			float variation = randf()*0.1;

			switch (_Type)
			{
			case NYCubeType::CUBE_HERBE:

				for (int i = bufferCount; i < bufferCount + 108; i += 3)
				{
					_WorldCols[i] = 0 + variation;
					_WorldCols[i + 1] = 1 - variation;
					_WorldCols[i + 2] = 0 + variation;
				}

				break;
			case NYCubeType::CUBE_EAU:

				for (int i = bufferCount; i < bufferCount + 108; i += 3)
				{
					_WorldCols[i] = 0 + variation;
					_WorldCols[i + 1] = 0 + variation;
					_WorldCols[i + 2] = 1 - variation;
				} break;
			case NYCubeType::CUBE_TERRE:

				for (int i = bufferCount; i < bufferCount + 108; i += 3)
				{
					_WorldCols[i] = 0.3094117647 + variation;
					_WorldCols[i + 1] = 0.11764705882 + variation;
					_WorldCols[i + 2] = 0 + variation;
				} break;
			}
		}


		void FillVertAndNormBuffer(int x, int y, int z, int bufferCount, float _WorldVert[], float _WorldNorm[] )
		{

			float* ptrVert = _WorldVert + bufferCount;
			float* ptrNorm = _WorldNorm + bufferCount;

			//Face1
			//Triangle1
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z;  ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;


			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			//Triangle2
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;


			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;


			//Face2
			//Triangle1
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			//Triangle2
			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;


			//Face3
			//Triangle1
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			//Triangle2
			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;


			//Face4
			//Triangle1
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;

			//Triangle2
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = -1; ptrNorm++;



			//Face5
			//Triangle1
			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;

			//Triangle2
			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = 0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;


			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;

			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 1; ptrNorm++;



			//Face6
			//Triangle1
			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;


			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			//Triangle2
			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = 0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = 0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z; ptrVert++;

			*ptrVert = -0.5 + x; ptrVert++;
			*ptrVert = -0.5 + y; ptrVert++;
			*ptrVert = -0.5 + z;


			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;

			*ptrNorm = -1; ptrNorm++;
			*ptrNorm = 0; ptrNorm++;
			*ptrNorm = 0;
		}
};