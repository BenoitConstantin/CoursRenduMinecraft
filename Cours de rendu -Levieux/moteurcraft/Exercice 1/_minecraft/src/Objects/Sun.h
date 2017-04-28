#pragma once

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"

class Sun
{
public:

	NYVert3Df m_position;
	NYVert3Df m_offset;

	float g_tweak_time = 1; //De base on se fie au temps réel.

	float diffuse[4];
	float specular[4];
	float ambient[4];


	NYColor sunColor;
	NYColor blueSkyColor = NYColor(0, 75.f / 255.f, 230.f / 255.f, 1);
	NYColor orangeSkyColor = NYColor(200.f / 255.f, 120.f / 255.f, 0, 1);

	time_t currentTime;

	float deltaTime = 0.f;

	Sun(time_t time);
	~Sun();

	void Draw();
	void EmitLight(NYRenderer* g_renderer);
	void Update(NYCamera camera);
};

