/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates a shark object out of basic shapes.
*/

#include <iostream>
#include "soldier.h"

using namespace std;
using namespace glm;

//Create a Shark object
Soldier::Soldier() : Object(){
	this->health = 100;
	this->dead = false;
	this->user = false;
	this->userTeam = false;
	this->ammo = 10024;
}

Soldier::~Soldier(){this->TakeDown();}

//Initialize all the Shark elements.
bool Soldier::Initialize()
{
	if (this->GLReturnedError("Soldier::Initialize - on entry"))
		return false;

	this->soldier = new Gengar();

	if(userTeam){
		this->soldier->texID = 11;
		this->soldier->order = 1;
		this->soldier->Initialize("./models/swSoldier.obj", "./models/swSoldier.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}
	else{
		this->soldier->texID = 11111;
		this->soldier->order = 1;
		this->soldier->Initialize("./models/battleDroid.obj", "./models/battleDroid.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	}
	//this->bullet->color = vec3(1,1,0);
	
	//this->bullet->Initialize(8, 1.0f, "phong3.vert", "phong3.frag");

	if (this->GLReturnedError("Soldier::Initialize - on exit"))
		return false;

	return true;
}

//Delete the Shark elements.
void Soldier::TakeDown()
{
	this->soldier = NULL;
	this->vertices.clear();
	this->shader.TakeDown();
	super::TakeDown();	
}

//Draw and position the Shark elements.
void Soldier::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float rotY)
{
	if (this->GLReturnedError("Soldier::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);
	mat4 another, scaler;
	another = modelview;

	float t = float(glutGet(GLUT_ELAPSED_TIME)) / 200.0f;

	this->soldier->Draw(projection, another, size, t);
	
	if (this->GLReturnedError("Soldier::Draw - on exit"))
		return;
}
