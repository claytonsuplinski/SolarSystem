/*
Name: Clayton Suplinski

Creates a local textured shader object
-Taken from demo code
*/

/*	Perry Kivolowitz - University of Wisconsin - Madison 
	Computer Sciences Department
*/

#include "texturedshader.h"

TexturedShader::TexturedShader() : super()
{
	this->texture_handle = GLuint(-1);
}

bool TexturedShader::Initialize(char * vertex_shader_file, char * fragment_shader_file)
{
	if (super::Initialize(vertex_shader_file, fragment_shader_file) == false)
		return false;

	this->Use();
	this->texture_handle = glGetUniformLocation(this->program_id, (const GLchar *) "picture");
	glUseProgram(0);
	return true;
}

void TexturedShader::CustomSetup()
{
	glUniform1i(this->texture_handle, 0);
}
