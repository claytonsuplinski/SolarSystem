/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Creates an alien object out of cylinders, discs, and spheres.
*/

#pragma once
#include "object.h"
#include "shader.h"
#include "disc.h"
#include "sphere.h"
#include "sphere2.h"

class SolarSystem : public Object
{
public:
	SolarSystem();
	~SolarSystem();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float rotY);
	void TakeDown();
	void StepShader();
	Shader shader;
	Shader solid_color;
	Disc * ring;
	Sphere * planet;
	Sphere2 * foot;
	Sphere * eye;
	Sphere2 * head;
	Sphere2 * hand;
	Sphere * mouth;
private:
	glm::vec4 colors[2];
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};