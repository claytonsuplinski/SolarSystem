/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates a shark object out of basic shapes.
*/

#include <iostream>
#include "team.h"

using namespace std;
using namespace glm;

//Create a Shark object
Team::Team() : Object(){
	userTeam = false;userTeam = false;drawBullet = false;
	for(int i=0; i<NUM_SOLDIERS; i++){drawBullets[i] = false;bulletAnimationTime[i] = 0;}
}

Team::~Team(){this->TakeDown();}

//Initialize all the Shark elements.
bool Team::Initialize()
{
	if (this->GLReturnedError("Team::Initialize - on entry"))
		return false;

	this->bullet = new Sphere();

	this->soldiers[0].userTeam = this->userTeam;
	if(userTeam){
		this->bullet->color = vec3(3.0f, 3.0f, 6.0f);
		this->bullet->Initialize(8, 1.0f, "phong.vert", "phong.frag");

		this->soldiers[0].user = true;this->soldiers[0].Initialize();
	}
	else {
		this->bullet->color = vec3(6.0f, 3.0f, 0.0f);
		this->bullet->Initialize(8, 1.0f, "phong.vert", "phong.frag");

		this->soldiers[0].Initialize();
	}

	this->points[0].x = 0;this->points[0].y = 0;this->points[0].z = 0;
		this->points[0].hRot = 0;this->points[0].vRot = 0;

		this->bulletLoc[0].x = 0;this->bulletLoc[0].y = 1.5;this->bulletLoc[0].z = 0;
		this->bulletLoc[0].hRot = 0;this->bulletLoc[0].vRot = 0;

	for(int i=1; i<NUM_SOLDIERS; i++){
		this->soldiers[i].userTeam = this->userTeam;
		this->soldiers[i].Initialize();
		this->points[i].x = 1000*((float) rand() / (RAND_MAX))-500;this->points[i].y = 0;this->points[i].z = 1000*((float) rand() / (RAND_MAX))-500;
		this->points[i].hRot = 360*((float) rand() / (RAND_MAX));this->points[i].vRot = i;

		this->bulletLoc[i].x = 1000*((float) rand() / (RAND_MAX))-500;this->bulletLoc[i].y = 1.5;this->bulletLoc[i].z = 1000*((float) rand() / (RAND_MAX))-500;
		this->bulletLoc[i].hRot = 360*((float) rand() / (RAND_MAX));this->bulletLoc[i].vRot = i;
	}

	if (this->GLReturnedError("Team::Initialize - on exit"))
		return false;

	return true;
}


void Team::StepShader(){
}

//Delete the Shark elements.
void Team::TakeDown()
{
	this->vertices.clear();
	this->shader.TakeDown();
	super::TakeDown();	
}

void Team::hpBounds(){
	for(int i=0; i<NUM_SOLDIERS; i++){
		if(soldiers[i].health > 100){soldiers[i].health = 100;}
		else if(soldiers[i].health < 0){soldiers[i].health = 0;soldiers[i].dead = true;}
	}
}

void Team::firedBullet(int time, int index){
	if(index < 0){
		for(int i=0; i<NUM_SOLDIERS; i++){
			if(this->soldiers[i].ammo > 0){
				this->bulletLoc[i].x = this->points[i].x;
				this->bulletLoc[i].y = this->points[i].y+1.5;
				this->bulletLoc[i].z = this->points[i].z;
				this->bulletLoc[i].hRot = this->points[i].hRot;
				this->bulletLoc[i].vRot = this->points[i].vRot;
				this->bulletAnimationTime[i] = time;
				this->soldiers[i].ammo--;
			}
		}
	}
	else{
		if(this->soldiers[index].ammo){
			this->bulletLoc[index].x = this->points[index].x;
			this->bulletLoc[index].y = this->points[index].y+1.5;
			this->bulletLoc[index].z = this->points[index].z;
			this->bulletLoc[index].hRot = this->points[index].hRot;
			this->bulletLoc[index].vRot = this->points[index].vRot;
			this->bulletAnimationTime[index] = time;
			this->soldiers[index].ammo--;
		}
	}

}

void Team::ifHitByBullet(){
	for(int i=0; i<NUM_SOLDIERS; i++){
		for(int j=0; j<NUM_SOLDIERS; j++){
			if(i != j){
				if(sqrt(dot(vec3(points[i].x-bulletLoc[j].x, points[i].y+1.5-bulletLoc[j].y, points[i].z-bulletLoc[j].z), 
					vec3(points[i].x-bulletLoc[j].x, points[i].y+1.5-bulletLoc[j].y, points[i].z-bulletLoc[j].z))) < 2){
					soldiers[i].health -= 2;
					if(soldiers[i].health <= 0){soldiers[i].health = 0;soldiers[i].dead = true;}
				}
			}
		}
	}
}

void Team::moveCpus(){
	float cpuSpeed = 0.05;
	if(userTeam){
		for(int i=1; i<NUM_SOLDIERS; i++){
			if(!soldiers[i].dead){
				this->points[i].hRot += ((float) rand() / (RAND_MAX))-0.5;
				this->points[i].x += cpuSpeed*cos(-this->points[i].hRot*3.14/180);
				this->points[i].z += cpuSpeed*sin(-this->points[i].hRot*3.14/180);
				
				if(abs(this->points[i].x) > 500.0f){
					this->points[i].x -= cpuSpeed*cos(-this->points[i].hRot*3.14/180);
					this->points[i].hRot += 10.f;
				}
				if(abs(this->points[i].z) > 500.0f){
					this->points[i].z -= cpuSpeed*sin(-this->points[i].hRot*3.14/180);
					this->points[i].hRot += 10.f;
				}
			}
		}
	}
	else{
		for(int i=0; i<NUM_SOLDIERS; i++){
			if(!soldiers[i].dead){
				this->points[i].hRot += ((float) rand() / (RAND_MAX))-0.5;
				this->points[i].x += cpuSpeed*cos(-this->points[i].hRot*3.14/180);
				this->points[i].z += cpuSpeed*sin(-this->points[i].hRot*3.14/180);
				
				if(abs(this->points[i].x) > 500.0f){
					this->points[i].x -= cpuSpeed*cos(-this->points[i].hRot*3.14/180);
					this->points[i].hRot += 10.f;
				}
				if(abs(this->points[i].z) > 500.0f){
					this->points[i].z -= cpuSpeed*sin(-this->points[i].hRot*3.14/180);
					this->points[i].hRot += 10.f;
				}
			}
		}
	}
}

//Draw and position the Shark elements.
void Team::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float rotY)
{
	if (this->GLReturnedError("Team::Draw - on entry"))
		return;

	hpBounds();

	glEnable(GL_DEPTH_TEST);
	mat4 another, refresh;
	another = modelview;refresh = modelview;

	int t = glutGet(GLUT_ELAPSED_TIME);
	int bulletTravelTime = 1000;

	ifHitByBullet();


	if(t%bulletTravelTime < 20){
		//cout << "t = " << t << endl;
		if(!userTeam && !soldiers[0].dead){firedBullet(t, 0);}

		for(int i=1; i<NUM_SOLDIERS; i++){
			if(!soldiers[i].dead){
				firedBullet(t, i);
			}
		}
		
	}

	moveCpus();

	for(int i=0; i<NUM_SOLDIERS; i++){
		if(!soldiers[i].dead){drawBullets[i] = true;}
		else{drawBullets[i] = false;}
	}

	//Draw the soldiers
	if(!drawBullet){
		if(userTeam && drawUser){
			//another = translate(another, vec3(points[0].x, points[0].y, points[0].z));
			//another = rotate(another, points[0].hRot, vec3(0,1,0));
			if(soldiers[0].dead){another = rotate(another, 90.f, vec3(0,0,1));}
			soldiers[0].Draw(projection, another, size, t);
			another = refresh;
		}
		else if(userTeam && !drawUser){
			for(int i=1; i<NUM_SOLDIERS; i++){
				another = translate(another, vec3(points[i].x, points[i].y, points[i].z));
				another = rotate(another, points[i].hRot, vec3(0,1,0));
				if(soldiers[i].dead){another = rotate(another, 90.f, vec3(0,0,1));}
				soldiers[i].Draw(projection, another, size, t);
				another = refresh;
			}
		}
		else{
			for(int i=0; i<NUM_SOLDIERS; i++){
				another = translate(another, vec3(points[i].x, points[i].y, points[i].z));
				another = rotate(another, points[i].hRot+90, vec3(0,1,0));
				if(soldiers[i].dead){another = rotate(another, -90.f, vec3(1,0,0));}
				another = scale(another, vec3(0.6f, 0.6f, 0.6f));
				soldiers[i].Draw(projection, another, size, t);
				another = refresh;
			}
		}
	}


	////Draw the bullets when necessary
	if(drawBullet){
		if(userTeam && drawUser){
			if(drawBullets[0]){
				if(t-bulletAnimationTime[0] < bulletTravelTime){
					bulletLoc[0].x += ((t-bulletAnimationTime[0])/3)*cos(-this->bulletLoc[0].hRot*3.14/180);
					bulletLoc[0].z += ((t-bulletAnimationTime[0])/3)*sin(-this->bulletLoc[0].hRot*3.14/180);
					another = translate(another, vec3(bulletLoc[0].x, bulletLoc[0].y, bulletLoc[0].z));
					another = rotate(another, bulletLoc[0].hRot, vec3(0,1,0));
					//another = translate(another, vec3((t-bulletAnimationTime[0])/3, 0.f, 0.f));
					another = scale(another, vec3(3.0f, 0.02f, 0.02f));
					bullet->Draw(projection, another, size, t);
					another = refresh;
				}
			}
		}
		else if(userTeam && !drawUser){
			for(int i=1; i<NUM_SOLDIERS; i++){
				if(drawBullets[i]){
					bulletLoc[i].x += ((t-bulletAnimationTime[i])/3)*cos(-this->bulletLoc[i].hRot*3.14/180);
					bulletLoc[i].z += ((t-bulletAnimationTime[i])/3)*sin(-this->bulletLoc[i].hRot*3.14/180);
					another = translate(another, vec3(bulletLoc[i].x, bulletLoc[i].y, bulletLoc[i].z));
					another = rotate(another, bulletLoc[i].hRot, vec3(0,1,0));
					//another = translate(another, vec3((t-bulletAnimationTime[i])/3, 0.f, 0.f));
					another = scale(another, vec3(3.0f, 0.02f, 0.02f));
					bullet->Draw(projection, another, size, t);
					another = refresh;
				}
			}
		}
		else{
			for(int i=0; i<NUM_SOLDIERS; i++){
				if(drawBullets[i]){
					bulletLoc[i].x += ((t-bulletAnimationTime[i])/3)*cos(-this->bulletLoc[i].hRot*3.14/180);
					bulletLoc[i].z += ((t-bulletAnimationTime[i])/3)*sin(-this->bulletLoc[i].hRot*3.14/180);
					another = translate(another, vec3(bulletLoc[i].x, bulletLoc[i].y, bulletLoc[i].z));
					another = rotate(another, bulletLoc[i].hRot, vec3(0,1,0));
					//another = translate(another, vec3((t-bulletAnimationTime[i])/3, 0.f, 0.f));
					another = scale(another, vec3(3.0f, 0.02f, 0.02f));
					bullet->Draw(projection, another, size, t);
					another = refresh;
				}
			}
		}
	}

	

	if (this->GLReturnedError("Team::Draw - on exit"))
		return;
}
