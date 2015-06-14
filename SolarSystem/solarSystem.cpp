/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates an alien object out of cylinders, discs, and spheres.
*/

#include <iostream>
#include "solarSystem.h"

using namespace std;
using namespace glm;

//Create a alien object
SolarSystem::SolarSystem() : Object(){}

SolarSystem::~SolarSystem(){
	this->TakeDown();
}

//Initialize the elements of the Alien object
bool SolarSystem::Initialize()
{
	if (this->GLReturnedError("SolarSystem::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	this->ring = new Disc();

	this->planet = new Sphere();this->eye = new Sphere();
	this->mouth = new Sphere();	this->head = new Sphere2();
	this->hand = new Sphere2();	this->foot = new Sphere2();
	

	this->ring->color = vec3(0.35f, 0.3f, 0.05f);
	this->ring->Initialize(20,1.0f, "phong.vert", "phong.frag");

	this->planet->color = vec3(1.0f, 1.0f, 0.0f);
	this->planet->Initialize(18,0.5f, "phong3.vert", "phong3.frag");

	if (this->GLReturnedError("SolarSystem::Initialize - on exit"))
		return false;

	return true;
}

void SolarSystem::StepShader(){
	
}

//Delete the elements of the Alien
void SolarSystem::TakeDown(){
	this->ring = NULL;this->hand = NULL;
	this->planet = NULL;this->eye = NULL;this->mouth = NULL;
	this->head = NULL;this->foot = NULL;
	this->vertices.clear();
	this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

//Draw a SolarSystem
void SolarSystem::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float rotY)
{
	if (this->GLReturnedError("SolarSystem::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);
	float t = float(glutGet(GLUT_ELAPSED_TIME)) / 150.0f;
	mat4 another, sun, mercury, venus, earth,
		mars, jupiter, saturn, uranus, neptune, pluto;

	another = modelview;
	
	//Draw the Sun -- w/o corona, diameter is 139.1  -- w/ corona, diameter is about 159.1
	sun = scale(another, vec3(35.91, 35.91, 35.91));
	this->planet->color = vec3(1,1,0);
	this->planet->Draw(projection, sun, size, rotY);

	//Draw Mercury
	mercury = translate(another, vec3(579.100,0.0,0.0));
	mercury = scale(mercury, vec3(.049, .049, .049));
	this->planet->color = vec3(0.75, 0.45, 0.1);
	this->planet->Draw(projection, mercury, size, 0);

	//Draw Venus
	venus = translate(another, vec3(1082.000,0.0,0.0));
	venus = scale(venus, vec3(.121, .121, .121));
	this->planet->color = vec3(0.85, 0.85, 0.0);
	this->planet->Draw(projection, venus, size, 0);

	//Draw Earth
	earth = translate(another, vec3(1496.000,0.0,0.0));
	earth = scale(earth, vec3(.127, .127, .127));
	this->planet->color = vec3(0.0, 1.0, 0.5);
	this->planet->Draw(projection, earth, size, 0);

	//Draw Mars
	mars = translate(another, vec3(2279.000,0.0,0.0));
	mars = scale(mars, vec3(.068, .068, .068));
	this->planet->color = vec3(1.0, 0.0, 0.0);
	this->planet->Draw(projection, mars, size, 0);

	//Draw Jupiter
	jupiter = translate(another, vec3(7785.000,0.0,0.0));
	jupiter = scale(jupiter, vec3(1.398, 1.398, 1.398));
	this->planet->color = vec3(1.0, 0.5, 0.0);
	this->planet->Draw(projection, jupiter, size, 0);

	//Draw Saturn
	saturn = translate(another, vec3(14330.000,0.0,0.0));
	saturn = scale(saturn, vec3(1.16, 1.16, 1.16));
	this->planet->color = vec3(0.85, 0.5, 0.0);
	this->planet->Draw(projection, saturn, size, 0);
	this->ring->Draw(projection, saturn, size, 0);

	//Draw Uranus
	uranus = translate(another, vec3(28770.000,0.0,0.0));
	uranus = scale(uranus, vec3(.507, .507, .507));
	this->planet->color = vec3(0.25, 0.25, 1.0);
	this->planet->Draw(projection, uranus, size, 0);

	//Draw Neptune
	neptune = translate(another, vec3(45030.000,0.0,0.0));
	neptune = scale(neptune, vec3(.492, .492, .492));
	this->planet->color = vec3(0.15, 0.15, 0.5);
	this->planet->Draw(projection, neptune, size, 0);

	//Draw Pluto
	pluto = translate(another, vec3(59000.000,0.0,0.0));
	pluto = scale(pluto, vec3(.0236, .0236, .0236));
	this->planet->color = vec3(0.35, 0.15, 0.45);
	this->planet->Draw(projection, pluto, size, 0);
	

	if (this->GLReturnedError("SolarSystem::Draw - on exit"))
		return;
}
