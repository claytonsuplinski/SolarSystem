/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates a shark object out of basic shapes.
*/

#pragma once
#include "object.h"
#include "shader.h"
#include "soldier.h"
#include "sphere.h"

class Team : public Object
{
public:
	Team();
	~Team();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
	void StepShader();
	void hpBounds();
	void firedBullet(int time, int index);
	void moveCpus();
	void ifHitByBullet();
	Shader shader;

	static const int NUM_SOLDIERS = 10;
	Soldier soldiers[NUM_SOLDIERS];

	struct soldierLoc{
		float x; float y; float z; float hRot; float vRot;
	};

	soldierLoc points[NUM_SOLDIERS];
	soldierLoc bulletLoc[NUM_SOLDIERS];
	int bulletAnimationTime[NUM_SOLDIERS];

	Sphere * bullet;

	bool userTeam;bool drawUser;
	bool drawBullets[NUM_SOLDIERS];
	bool drawBullet;

private:
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};