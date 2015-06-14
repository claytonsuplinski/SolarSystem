/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates the square arena
*/

#include <iostream>
#include "squareStadium.h"

using namespace std;
using namespace glm;

//Create the SquareStadium object
SquareStadium::SquareStadium() : Object()
{
}

SquareStadium::~SquareStadium(){
	this->TakeDown();
}

//Initialize all the elements of the SquareStadium
bool SquareStadium::Initialize(int weatherIndex)
{
	if (this->GLReturnedError("SquareStadium::Initialize - on entry"))
		return false;

	if (!super::Initialize())
		return false;

	this->wall = new Square3();this->floor = new Square3();this->bleacher = new Bleacher();
	this->grass = new Cylinder();this->bird = new Bird();this->bird2 = new Bird();
	this->bird3 = new Bird();this->bird4 = new Bird();this->jb = new Jumbotron();
	this->zubat = new Gengar();this->charizard = new Gengar();this->blades = new Gengar();
	this->mtns = new Gengar();this->trees = new Gengar();

	this->jb->Initialize();
	
	
	this->floor->texID = 28;
	this->floor->Initialize(200, 1000.0f, "./textures/grass.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	

	this->wall->texID = 10;
	this->wall->Initialize(1,105.6f, "./textures/wood.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");
	
	this->blades->texID = 29;
	this->blades->Initialize("./models/Grass2.obj", "./models/Grass1.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->mtns->texID = 30;
	this->mtns->Initialize("./models/mtnPass.obj", "./models/mtnPass.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	this->grass->color = vec3(0.05f, 0.65f, 0.2f);
	this->grass->Initialize(1,0.75f, 0.03f, 0.03f, "phong.vert", "phong.frag");

	this->trees->texID = 49;
	this->trees->Initialize("./models/GumBough.obj", "./models/GumBough.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag");

	if (this->GLReturnedError("SquareStadium::Initialize - on exit"))
		return false;

	return true;
}

void SquareStadium::StepShader(){
	this->floor->StepShader();this->bleacher->StepShader();this->grass->StepShader();
	this->bird->StepShader();this->bird2->StepShader();this->bird3->StepShader();
	this->bird4->StepShader();this->jb->StepShader();
}

//Delete the SquareStadium
void SquareStadium::TakeDown()
{
	this->wall = NULL;this->floor = NULL;this->bleacher = NULL;this->grass = NULL;
	this->bird = NULL;this->bird2 = NULL;this->bird3 = NULL;this->bird4 = NULL;
	this->jb = NULL;this->zubat = NULL;this->charizard = NULL;this->blades = NULL;
	this->mtns = NULL;this->trees = NULL;this->vertices.clear();this->shader.TakeDown();
	this->solid_color.TakeDown();
	super::TakeDown();
}

//Draw and position the elements of the SquareStadium
void SquareStadium::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("SquareStadium::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);

	mat4 another, scaler;

	float t = float(glutGet(GLUT_ELAPSED_TIME) / 5000.0f);
	float tOffset = float(glutGet(GLUT_ELAPSED_TIME) / 1000.0f) - 1000;

	

	//Elements of the arena.
	another = modelview;
	mat4 grassPlacement = another;
	//another = translate(modelview, vec3(-500.0f, 0.0f, -500.0f));
	//another = rotate(another, 90.0f, vec3(1.0f, 0.0f, 0.0f));	
	//another = rotate(another, 90.0f, vec3(0.0f, 1.0f, 0.0f));
	this->floor->Draw(projection, another, size, 0);

	//grassPlacement = translate(grassPlacement, vec3(-52.8f, 0.0f, -52.8f));

	mat4 mtnPlacement = translate(grassPlacement, vec3(0.0f, -20.0f, 0.0f));

	mat4 grassPlacementCopy = grassPlacement;
	mat4 mtnPlacementCopy = mtnPlacement;



	grassPlacement = scale(grassPlacement, vec3(25,25,25));
	this->blades->Draw(projection, grassPlacement, size, 0);
	grassPlacement = grassPlacementCopy;

	mtnPlacement = translate(mtnPlacement, vec3(-1000.0f, 0.0f, 0.0f));
	mtnPlacement = rotate(mtnPlacement, 45.0f, vec3(0,1,0));
	mtnPlacement = scale(mtnPlacement, vec3(25,15,25));
	this->mtns->Draw(projection, mtnPlacement, size, 0);
	mtnPlacement = mtnPlacementCopy;

	if (this->GLReturnedError("SquareStadium::Draw - on exit"))
		return;
}
