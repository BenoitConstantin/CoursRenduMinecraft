#pragma once

#include "engine/render/renderer.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class NYChunk
{
	public :

		static const int CHUNK_SIZE = 16; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		NYCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		GLuint _BufWorld; ///< Identifiant du VBO pour le monde
		
		static float _WorldVert[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*3*12]; ///< Buffer pour les sommets
		static float _WorldCols[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*3*12]; ///< Buffer pour les couleurs
		static float _WorldNorm[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*3*12]; ///< Buffer pour les normales

		static const int SIZE_VERTICE = 3 * sizeof(float); ///< Taille en octets d'un vertex dans le VBO
		static const int SIZE_COLOR = 3 * sizeof(float);  ///< Taille d'une couleur dans le VBO
		static const int SIZE_NORMAL = 3 * sizeof(float);  ///< Taille d'une normale dans le VBO
		
		int _NbVertices; ///< Nombre de vertices dans le VBO (on ne met que les faces visibles)

		NYChunk * Voisins[6];
		
		NYChunk()
		{
			_NbVertices = 0;
			_BufWorld = 0;
			memset(Voisins,0x00,sizeof(void*) * 6);
		}

		void setVoisins(NYChunk * xprev, NYChunk * xnext,NYChunk * yprev,NYChunk * ynext,NYChunk * zprev,NYChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		/**
		  * Raz de l'�tat des cubes (a draw = false)
		  */
		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = false;
						_Cubes[x][y][z]._Type = CUBE_AIR;
					}
		}


		//On met le chunk dans son VBO
		void toVbo(void)
		{
			GLenum error;
			_NbVertices = 0;

			for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 3 * 3 * 12; i++)
			{
				_WorldVert[i] = 0;
				_WorldCols[i] = 0;
				_WorldNorm[i] = 0;
			}

			int bufferCount = 0;

			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				for (int j = 0; j < CHUNK_SIZE; j++)
				{
					for (int k = 0; k < CHUNK_SIZE; k++)
					{
						NYCube c = _Cubes[i][j][k];

						if (c._Draw)
						{
							  //12 triangles dans un cube (sisi)
							c.FillVertAndNormBuffer(i, j, k, bufferCount, _WorldVert, _WorldNorm);
							c.FillColorBuffer(bufferCount, _WorldCols);
							_NbVertices += 36;	//On a du �crire 12 triangles avec 3 vertices (et c'�tait pas drole :'( )
							bufferCount += 108; //(*3 float -,-)
						}
					}
				}
			}

			//printf("Vertices : %d\n", _NbVertices);
			//printf("Max float : %d\n", CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 3 * 3 * 12);

			//On le detruit si il existe deja
			if (_BufWorld != 0)
				glDeleteBuffers(1, &_BufWorld);

			//Genere un identifiant
			glGenBuffers(1, &_BufWorld);

			//On attache le VBO pour pouvoir le modifier
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);

			//On reserve la quantite totale de datas (creation de la zone memoire, mais sans passer les donn�es)
			//Les tailles g_size* sont en octets, � vous de les calculer
			glBufferData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE +
				_NbVertices * SIZE_COLOR +
				_NbVertices * SIZE_VERTICE,
				NULL,
				GL_STREAM_DRAW);

			//Check error (la tester ensuite...)
			error = glGetError();

			//On copie les vertices
			glBufferSubData(GL_ARRAY_BUFFER,
				0, //Offset 0, on part du debut                        
				_NbVertices * SIZE_VERTICE, //Taille en octets des datas copi�s
				_WorldVert);  //Datas          

			//Check error (la tester ensuite...)
			error = glGetError();

			//On copie les couleurs
			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE, //Offset : on se place apr�s les vertices
				_NbVertices * SIZE_COLOR, //On copie tout le buffer couleur : on donne donc sa taille
				_WorldCols);  //Pt sur le buffer couleur       

			 //Check error (la tester ensuite...)
			error = glGetError();

			//On copie les normales (a vous de d�duire les params)
			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE + _NbVertices * SIZE_COLOR,
				_NbVertices * SIZE_VERTICE,
				_WorldNorm );

			//On debind le buffer pour eviter une modif accidentelle par le reste du code
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void render(void)
		{
			glPushMatrix();
			glScaled(NYCube::CUBE_SIZE, NYCube::CUBE_SIZE, NYCube::CUBE_SIZE);

			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_LIGHTING);

			//On bind le buffer
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);
			NYRenderer::checkGlError("glBindBuffer");

			//On active les datas que contiennent le VBO
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			//On place les pointeurs sur les datas, aux bons offsets
			glVertexPointer(3, GL_FLOAT, 0, (void*)(0));
			glColorPointer(3, GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE));
			glNormalPointer(GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE + _NbVertices*SIZE_COLOR));

			//On demande le dessin
			glDrawArrays(GL_TRIANGLES, 0, _NbVertices);

			//On cleane
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			glPopMatrix();
		}

		/**
		  * On verifie si le cube peut �tre vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			NYCube * cubeXPrev = NULL; 
			NYCube * cubeXNext = NULL; 
			NYCube * cubeYPrev = NULL; 
			NYCube * cubeYNext = NULL; 
			NYCube * cubeZPrev = NULL; 
			NYCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if( cubeXPrev->isSolid() == true && //droite
				cubeXNext->isSolid() == true && //gauche
				cubeYPrev->isSolid() == true && //haut
				cubeYNext->isSolid() == true && //bas
				cubeZPrev->isSolid() == true && //devant
				cubeZNext->isSolid() == true )  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						if (!_Cubes[x][y][z]._Draw) //Je ne test pas la desactivation d'un cube si celui-ci est d�j� d�sactiv� ^^
							continue;

						_Cubes[x][y][z]._Draw = true;
						if(test_hidden(x,y,z))
							_Cubes[x][y][z]._Draw = false;
					}
		}


};