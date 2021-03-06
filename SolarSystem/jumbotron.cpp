/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates a jumbotron object out of basic shapes.
*/

#include <iostream>
#include "jumbotron.h"

using namespace std;
using namespace glm;

//Create a Jumbotron object
Jumbotron::Jumbotron() : Object()
{
}

Jumbotron::~Jumbotron()
{
	this->TakeDown();
}

//Initialize all the Jumbotron elements.
bool Jumbotron::Initialize()
{
	if (this->GLReturnedError("Jumbotron::Initialize - on entry"))
		return false;

	this->base = new Cylinder();this->screen = new Square4();

	this->base->color = vec3(0.35f, 0.35f, 0.35f);
	this->base->Initialize(8,10.0f,1.0f,1.0f, "phong.vert", "phong.frag");

	this->screen->Initialize(1,5.0f, "jumbotronShader.vert", "jumbotronShader.frag");

	if (this->GLReturnedError("Jumbotron::Initialize - on exit"))
		return false;

	return true;
}


void Jumbotron::StepShader(){}

//Delete the Jumbotron elements.
void Jumbotron::TakeDown(){
	this->base = NULL;this->screen = NULL;this->vertices.clear();
	this->shader.TakeDown();
	super::TakeDown();	
}

//Draw and position the Jumbotron elements.
void Jumbotron::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float rotY)
{
	if (this->GLReturnedError("Jumbotron::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);
	mat4 another, scaler;

	another = modelview;
	another = rotate(another, -90.0f, vec3(0,1,0));
	this->base->Draw(projection, another, size, 0);

	another = translate(another, vec3(0.0,0.0,-5.0));
	this->base->Draw(projection, another, size, 0);
	another = translate(another, vec3(0.0,0.0,5.0));

	another = translate(another, vec3(1.1,10.0,-4.0));
	another = rotate(another, 90.0f, vec3(0,1,0));
	another = rotate(another, -90.0f, vec3(1,0,0));
	this->screen->fboID = this->fboID;
	this->screen->Draw(projection, another, size, 0);
	another = rotate(another, -90.0f, vec3(0,0,1));
	another = translate(another, vec3(-1.0,-15.0,0.0));
	
	if (this->GLReturnedError("Jumbotron::Draw - on exit"))
		return;
}
