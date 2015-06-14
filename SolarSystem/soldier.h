/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates a shark object out of basic shapes.
*/

#pragma once
#include "object.h"
#include "shader.h"
#include "gengar.h"

class Soldier : public Object
{
public:
	Soldier();
	~Soldier();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
	void StepShader();
	Shader shader;

	Gengar * soldier;

	int health;
	bool user;
	bool dead; //Change to int if multiple lives
	bool userTeam;
	int ammo;

private:
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};