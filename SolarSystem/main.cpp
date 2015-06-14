/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Moshball - This video game has several modes.
See the README for the complete write-up about the 
game.
*/

#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <conio.h>
//#include <AL/al.h>
//#include <AL/alc.h>
//#include <AL/alut.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shader_utils.h"

#include "disc.h"
#include "sphere.h"
#include "cylinder.h"
#include "torus.h"
#include "square.h"
#include "tiger.h"
#include "squareStadium.h"
#include "discStadium.h"
#include "sphereStadium.h"
#include "coin.h"
#include "solarSystem.h"
#include "shark.h"
#include "triangle.h"
#include "bird.h"
#include "speedBoost.h"
#include "gengar.h"

#include "cube2.h"
#include "sphere2.h"
#include "square2.h"
#include "square4.h"

#include "vertexattributes2.h"
#include "object2.h"
#include "square2.h"
#include "shader2.h"
#include "globals.h"
#include "fbo.h"
#include "soldier.h"
#include "team.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

using namespace std;using namespace glm;

//x (0=Normal, 1=night, 2=twilight), y(0=normal, 1=underwater, 2=fog)
ivec2 tod = ivec2(0, 0);

//Creates a class for maintaining the window information for easy access.
class Window
{
public:
	Window()
	{
		this->time_last_pause_began = this->total_time_paused = 0;
		this->normals = this->wireframe = this->paused = false;
		this->slices = 20;this->interval = 1000 / 120;this->window_handle = -1;
	}
	float time_last_pause_began;float total_time_paused;bool paused , wireframe, normals;
	int window_handle;int interval;int slices;ivec2 size;float window_aspect;
	vector<string> instructions, countdown, scores;
} window;

/////////////////////////////////////VARIABLE DECLARATIONS/////////////////////////////
//Menu decision variables
bool menuOn = true;int menuSelect = 0;bool singlePlayer = false;
bool twoPlayer = false;bool secondPlayerTurn = false;int stadiumSelect = 0;
bool weatherSelect = false;

float moveVelocity = 0.01f;

const int numPlanets = 10;
int planetSelect = 0;
float planetX[numPlanets] = {0, 579.100, 1082.000, 1496.000, 2279.000, 7785.000, 14330.000, 28770.000, 45030.000, 59000.000};

//Values defining the position/rotation of the camera.
double RotatedX = 8;double RotatedY = 0;float transX = 0;
float transY = 0;float transZ = 0;

float velocity = 1; //Define the travel of the user
int score = 0; //The user's score for the game
bool caught = false; //If the enemy caught the user
float firstPersonTime, firstPersonScore; //Variables used for the 2 player game
bool startupTime = true; //Account for delay in loading the music
int gameSelect = 0; //Select amongst the game types
int viewPerspective = 0; //Select amongst 1st and 3rd person views
int weatherType = 0; //Select amongst the weather types

//Used for adjusting the y value for the lookAt function for the first and third person views.
float perspectiveOffset = 0.0f;
//Used for adjusting the z value for the lookAt function for the first and third person views.
float perspectiveStepBack = 1.0f;

bool countdownOn = false; //If the menu is off and the countdown is on
bool won = false; //If the user won
float* partX, *partY, *partOffset;int numParts; //Variables controlling the precipitation.

//Time control variables.
float current_timeDisplay, menuTime, countdownTime; 
string timeString;int first = 1;int menuFirst = 1;

//The projection and modelview matrices. 
mat4 projection, modelview;

//Mouse value/identification variables.
float mouseX = 0;float mouseY = 0;
float prevMouseY = (float)(window.size.y/2);

float maxSpeed = 1.6f; //Maximum velocity for user

//////////////////////////////////////TEXT RENDERING/////////////////////////////////////////////
////////Font variables//////////
GLuint program;GLint attribute_coord;GLint uniform_tex;GLint uniform_color;
struct point {GLfloat x;GLfloat y;GLfloat s;GLfloat t;};
GLuint vbo;FT_Library ft;FT_Face face;const char *fontfilename;
int init_resources()
{        /* Initialize the FreeType2 library */
        if(FT_Init_FreeType(&ft)){fprintf(stderr, "Could not init freetype library\n");return 0;}
        /* Load a font */
        if(FT_New_Face(ft, fontfilename, 0, &face)){fprintf(stderr, "Could not open font %s\n", fontfilename);return 0;}
        /* Compile and link the shader program */
        GLint link_ok = GL_FALSE;GLuint vs, fs;
        if ((vs = create_shader("text.vert", GL_VERTEX_SHADER))        == 0) return 0;
        if ((fs = create_shader("text.frag", GL_FRAGMENT_SHADER)) == 0) return 0;

        program = glCreateProgram();glAttachShader(program, vs);glAttachShader(program, fs);
        glLinkProgram(program);glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {fprintf(stderr, "glLinkProgram:");print_log(program);return 0;}

        const char* attribute_name;attribute_name = "coord";
        attribute_coord = glGetAttribLocation(program, attribute_name);
        if (attribute_coord == -1) {fprintf(stderr, "Could not bind attribute %s\n", attribute_name);return 0;}

        const char* uniform_name;uniform_name = "tex";
        uniform_tex = glGetUniformLocation(program, uniform_name);
        if (uniform_tex == -1){fprintf(stderr, "Could not bind uniform %s\n", uniform_name);return 0;}

        uniform_name = "color";uniform_color = glGetUniformLocation(program, uniform_name);
        if (uniform_color == -1){fprintf(stderr, "Could not bind uniform %s\n", uniform_name);return 0;}

        // Create the vertex buffer object
        glGenBuffers(1, &vbo);
        return 1;
}

/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void render_text(const char *text, float x, float y, float sx, float sy) {
        const char *p;FT_GlyphSlot g = face->glyph;

        /* Create a texture that will be used to hold one "glyph" */
        GLuint tex;glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(uniform_tex, 0);
        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        /* Clamping to edges is important to prevent artifacts when scaling */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        /* Linear filtering usually looks best for text */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /* Set up the VBO for our vertex data */
        glEnableVertexAttribArray(attribute_coord);glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
        /* Loop through all characters */
        for(p = text; *p; p++) {
                /* Try to load and render the character */
                if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
                        continue;

                /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
                glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
                /* Calculate the vertex and texture coordinates */
                float x2 = x + g->bitmap_left * sx;float y2 = -y - g->bitmap_top * sy;
                float w = g->bitmap.width * sx;float h = g->bitmap.rows * sy;

                point box[4] = {
                        {x2,     -y2    , 0, 0}, {x2 + w, -y2    , 1, 0},
                        {x2,     -y2 - h, 0, 1}, {x2 + w, -y2 - h, 1, 1},
                };

                /* Draw the character on the screen */
                glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                /* Advance the cursor to the start of the next character */
                x += (g->advance.x >> 6) * sx;y += (g->advance.y >> 6) * sy;
        }
        glDisableVertexAttribArray(attribute_coord);glDeleteTextures(1, &tex);
}

void textDisplay(int textIndex, float windowX, float windowY, int timeOfDay, bool pokemon)
{
		glDisable(GL_CULL_FACE);
        float sx = (float)(2.0/windowX);
        float sy = (float)(2.0/windowY);

        glUseProgram(program);

        glClearColor(1.0,1.0,0.25,1);
       // glClear(GL_COLOR_BUFFER_BIT);

        /* Enable blending, necessary for our alpha texture */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLfloat realRed[4] = {1, 0, 0, 1.0};
        GLfloat red[4] = {1, 1, 0, 1};
        GLfloat transparent_green[4] = {0, 1, 0, 0.5};

        /* Set font size to 48 pixels, color to black */
        FT_Set_Pixel_Sizes(face, 0, 48);
		
		if(textIndex == 0){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Main Menu",            -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Tiger Echo Plateau",            -1 + 8 * sx,   1 - 150 * sy,    sx, sy);
			render_text("(2) The Intergalactic Dome",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(3) Marine Layer Arena",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(4) Weather Select",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 350 * sy,   sx, sy);
		}
		else if(textIndex == 1){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Weather Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) No weather",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Fog",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(3) Rain",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(4) Snow",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 350 * sy,   sx, sy);
		}
		else if(textIndex == 2){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Player Options",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Single Player",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Two Players (Not implemented)",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 3 || textIndex == 4 || textIndex == 5){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Game Type Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Moshball",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);	
			if(textIndex == 3){
				if(pokemon){render_text("(2) Avoid the Gengar",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Tiger",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			else if(textIndex == 4){
				if(pokemon){render_text("(2) Avoid the Magneton",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Alien",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			else if(textIndex == 5){
				if(pokemon){render_text("(2) Avoid the Tentacool",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Shark",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			render_text("(3) Coin Time Attack",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
		}
		else if(textIndex == 6){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - View Perspective Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) First person view",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Third person view",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 7){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Game Type Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Moshball (Timed)",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Coin Collection",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 8){glUniform4fv(uniform_color, 1, realRed);render_text("Get Ready!",            -0.2f,  0,   sx, sy);}
		glUniform4fv(uniform_color, 1, red);
		if(timeOfDay == 0){render_text("(5) Daytime",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}
		else if(timeOfDay == 1){render_text("(5) Night",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}
		else if(timeOfDay == 2){render_text("(5) Twilight",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}

		if(pokemon){render_text("(6) vs Pokemon",      -1 + 8 * sx,   1 - 450 * sy,   sx, sy);}
		else{render_text("(6) No Pokemon",      -1 + 8 * sx,   1 - 450 * sy,   sx, sy);}
		
		glDisable(GL_BLEND);
		glUseProgram(0);
		glEnable(GL_CULL_FACE);
		
		
}

void activeTextDisplay(const char* words, float x, float y, FT_UInt size)
{
		glDisable(GL_CULL_FACE);
        float sx = (float)(2.0/glutGet(GLUT_WINDOW_WIDTH));
        float sy = (float)(2.0/glutGet(GLUT_WINDOW_HEIGHT));

        glUseProgram(program);

        /* Enable blending, necessary for our alpha texture */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/* Set font size to 48 pixels, color to black */
        FT_Set_Pixel_Sizes(face, 0, size);

		if(weatherType != 3 || tod.x == 1){GLfloat black[4] = {1, 1, 1, 1};glUniform4fv(uniform_color, 1, black);}
		else{GLfloat black[4] = {0,0,0,0.75};glUniform4fv(uniform_color, 1, black);}
		
		render_text(words,x,y,sx,sy); 
		
		glDisable(GL_BLEND);
		glUseProgram(0);
        //glFinish();
		glEnable(GL_CULL_FACE);
}

/*
/////////////////////////////////////AUDIO CONTROLS/////////////////////////////////////////////////////
 // Buffers to hold sound data.
ALuint Buffer;ALuint origBuffer;
// Sources are points of emitting sound.
ALuint Source;ALuint origSource;
/*
 * These are 3D cartesian vector coordinates. A structure or class would be
 * a more flexible of handling these, but for the sake of simplicity we will
 * just leave it as is.
 */
/*
// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
// Position of the Listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
// Velocity of the Listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

/*
 * ALboolean LoadALData()
 *
 *         This function will load our sample data from the disk using the Alut
 *         utility and send the data into OpenAL as a buffer. A source is then
 *         also created to play that buffer.
 */
/*
ALboolean LoadALData(ALbyte *song, bool looping){
            // Variables to load into.
	        ALenum format;ALsizei size;ALvoid* data;ALsizei freq;
            ALboolean loop = true; 

            // Load wav data into a buffer.
            alGenBuffers(1, &Buffer);
			if(alGetError() != AL_NO_ERROR)
                        return AL_FALSE;

    // Load any of your favourite wav song here
            alutLoadWAVFile(song, &format, &data, &size, &freq, &loop);
            alBufferData(Buffer, format, data, size, freq);
            alutUnloadWAV(format, data, size, freq);
			
            // Bind the buffer with the source.
            alGenSources(1, &Source);
            if(alGetError() != AL_NO_ERROR)
                        return AL_FALSE;
			
            alSourcei (Source, AL_BUFFER,   Buffer   );
            alSourcef (Source, AL_PITCH,    1.0      );
            alSourcef (Source, AL_GAIN,     1.0      );
            alSourcefv(Source, AL_POSITION, SourcePos);
            alSourcefv(Source, AL_VELOCITY, SourceVel);
			if(looping){
            alSourcei (Source, AL_LOOPING,  AL_TRUE     );
			}
			else{
			alSourcei (Source, AL_LOOPING,  AL_FALSE     );
			}

            // Do another error check and return.
            if(alGetError() == AL_NO_ERROR)
                        return AL_TRUE;
			
            return AL_FALSE;
}

/*
 * void SetListenerValues()
 *
 *         We already defined certain values for the Listener, but we need
 *         to tell OpenAL to use that data. This function does just that.
 */
/*
void SetListenerValues(){
            alListenerfv(AL_POSITION,    ListenerPos);alListenerfv(AL_VELOCITY,    ListenerVel);
            alListenerfv(AL_ORIENTATION, ListenerOri);
}

/*
 * void KillALData()
 *
 *         We have allocated memory for our buffers and sources which needs
 *         to be returned to the system. This function frees that memory.
 */
/*
void KillALData(){/*alDeleteBuffers(1, &Buffer);alDeleteSources(1, &Source);alDeleteBuffers(1, &origBuffer);alDeleteSources(1, &origSource);*///alutExit();}
/*
void init(ALbyte *song, bool loop){
            // Initialize OpenAL and clear the error bit. 
            alutInit(NULL, 0);alGetError();
			
            // Load the wav data. 
            if(LoadALData(song, loop) == AL_FALSE) //method 1: LoadALData()
            {
                printf("Error loading data.");
				return;
            }
			
            SetListenerValues(); //method 2: void SetListenerValues()
			
            // Setup an exit procedure. 
            atexit(KillALData); //method 3: void KillALData()
			if(loop){origSource = Source;}
			alSourcePlay(Source);

}*/

////////////////////////////////////OBJECT CREATION////////////////////////////////////
Sphere sphere, sph1, enm, usr;Sphere sphere2;Cylinder cylinder;Disc disc;Cylinder healthBar;
Torus torus;Square square;Square square2;Tiger tiger;Sphere rain2;Sphere snow2;
SquareStadium stadium;Coin coin;SolarSystem ss;Shark shark;Bird bird;SpeedBoost sb;Square4 sq4;
Gengar arena, magneton, haunter, star, frog, goldeen;Square2 tri2;FrameBufferObject fbo;
Cube2 skybox, skybox2, skybox3, skyboxUW;Sphere2 egg;

Soldier soldier;Team userTeam, cpuTeam;

////////////////////////////////////MOUSE DETECTION & MANAGEMENT/////////////////////////
void mouseMovement(int x, int y) {
    mouseX=(float)x; //set lastx to the current x position
    mouseY=(float)y; //set lasty to the current y position
}

void mouseRotations(int stadium, int person){
	if(person == 1 || person == 3){
		//Turning controls
		if(mouseX < window.size.x/2){RotatedY -= abs((mouseX - window.size.x/2)/75);}
		else if(mouseX > window.size.x/2){RotatedY += abs((mouseX - window.size.x/2)/75);}
		//Forward & Backward Velocity controls
		if(mouseY < window.size.y/2 && velocity < maxSpeed*(2*abs(mouseY-(window.size.y/2))/window.size.y)){
			velocity += abs((mouseY - window.size.y/2)/1000);
		}
		else if(mouseY < window.size.y/2 && mouseY > prevMouseY){velocity += -abs((mouseY - window.size.y/2)/1000);}
		if(mouseY > window.size.y/2 && velocity > -maxSpeed*(2*abs(mouseY-(window.size.y/2))/window.size.y)){
			velocity += -abs((mouseY - window.size.y/2)/1000);
		}
		else if(mouseY > window.size.y/2 && mouseY < prevMouseY){
			velocity += abs((mouseY - window.size.y/2)/1000);
		}
	}
	if(stadium > 2){   //Up/Down Turning Controls
		if(mouseY < window.size.y/2 && RotatedX >= -89){
			RotatedX -= abs((mouseY - window.size.y/2)/75);
		}
		else if(mouseY > window.size.y/2 && RotatedX <= 89){
			RotatedX += abs((mouseY - window.size.y/2)/75);
		}
	}
	prevMouseY = mouseY;
}

/////////////////////////////////////CAMERA CONTROLS//////////////////////////////////////////
//Moves the camera according to user inputs
glm::mat4 render(glm::mat4 Model){		
	Model = glm::rotate(Model, (GLfloat) RotatedX, glm::vec3(1,0,0));
	Model = glm::rotate(Model, (GLfloat) RotatedY, glm::vec3(0,1,0));
	Model = glm::translate(Model, glm::vec3(transX,0,0));    
	Model = glm::translate(Model, glm::vec3(0,0,transZ));    
	Model = glm::translate(Model, glm::vec3(0,transY,0));	
	glLoadMatrixf(glm::value_ptr(Model));
    return Model;
}

//Undoes the render function. 
glm::mat4 endRender(glm::mat4 Model){	
	Model = glm::translate(Model, glm::vec3(0,-transY,0));	
	Model = glm::translate(Model, glm::vec3(0,0,-transZ));
	Model = glm::translate(Model, glm::vec3(-transX,0,0));
	Model = glm::rotate(Model, (GLfloat) -RotatedY, glm::vec3(0,1,0));
	Model = glm::rotate(Model, (GLfloat) -RotatedX, glm::vec3(1,0,0));			
	glLoadMatrixf(glm::value_ptr(Model));
    return Model;
}

//Changes the value of the angle in which the camera should be rotated vertically.
void rotateX(double angle){
    if(!(RotatedX>=89 && angle>0) && !(RotatedX<=-89 && angle<0)){
        RotatedX += angle;
    }
}

//Changes the value of the angle in which the camera should be rotated horizontally.
void rotateY(double angle){
    RotatedY += angle;
}

//Deletes all of the created objects here.
void CloseFunc(){
	window.window_handle = -1;
	cylinder.TakeDown();torus.TakeDown();square.TakeDown();square2.TakeDown();
	disc.TakeDown();sphere.TakeDown();sphere2.TakeDown();tiger.TakeDown();goldeen.TakeDown();
	stadium.TakeDown();coin.TakeDown();ss.TakeDown();healthBar.TakeDown();
	shark.TakeDown();bird.TakeDown();sb.TakeDown();arena.TakeDown();skyboxUW.TakeDown();fbo.TakeDown();rain2.TakeDown();
	tri2.TakeDown();skybox.TakeDown();skybox2.TakeDown();skybox3.TakeDown();egg.TakeDown();snow2.TakeDown();
	star.TakeDown();magneton.TakeDown();haunter.TakeDown();frog.TakeDown();
	sph1.TakeDown(); enm.TakeDown(); usr.TakeDown();sq4.TakeDown();soldier.TakeDown();userTeam.TakeDown();cpuTeam.TakeDown();
}

//Maintains aspect ratio when window is resized.
void ReshapeFunc(int w, int h){
	if (h > 0){window.size = ivec2(w, h);window.window_aspect = float(w) / float(h);}
}

void KeyboardFunc(unsigned char c, int x, int y){
	float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	switch (c){
	case 'b':  //Shifts backwards through menu
		if(menuOn){
			if(stadiumSelect > 0){
				if(singlePlayer || twoPlayer){
					if(gameSelect > 0){gameSelect = 0;}
					else{
						singlePlayer = false;
						twoPlayer = false;
					}
				}
				else{
					stadiumSelect = 0;
					menuSelect = 0;
				}
			}
		}
		//Goes back out of the weather menu
		if(weatherSelect){
			weatherSelect = false;
		}
		break;

	case 'e':
		transX = -planetX[planetSelect];transY = 0;transZ = 0;
		planetSelect = (planetSelect + 1) % numPlanets;		
		break;

	case 's':
		transX = 0;transY = 0;transZ = 0;
		break;

	case 'n':  //Moves user down one unit
		transY = transY + moveVelocity;
		break;

	case 'u':  //Moves user up one unit
		transY = transY - moveVelocity;
		break;

	case 'i':  //Moves user forward one unit
		//cout << "transX = " << transX << "; transZ = " << transZ << endl;
		if(stadiumSelect == 1 || stadiumSelect == 2){
			transZ = (float)(transZ + moveVelocity*cos(-RotatedY*3.14/180));
			transX = (float)(transX + moveVelocity*sin(-RotatedY*3.14/180));
		}
		
		break;

	case 'k':  //Moves user backward one unit
		transZ = (float)(transZ - moveVelocity*cos(-RotatedY*3.14/180));
		transX = (float)(transX - moveVelocity*sin(-RotatedY*3.14/180));
		break;

	case 'j':  //Moves user left one unit
		transX = (float)(transX + moveVelocity*cos(RotatedY*3.14/180));
		transZ = (float)(transZ + moveVelocity*sin(RotatedY*3.14/180));
		break;

	case 'l':  //Moves user right one unit
		transX = (float)(transX - moveVelocity*cos(RotatedY*3.14/180));
		transZ = (float)(transZ - moveVelocity*sin(RotatedY*3.14/180));
		break;

	case 'p':
		if(!menuOn && !countdownOn){
			if (window.paused == true)
			{
				// Will be leaving paused state
				window.total_time_paused += (current_time - window.time_last_pause_began);
			}
			else
			{
				// Will be entering paused state
				window.time_last_pause_began = current_time;
			}
			window.paused = !window.paused;
		}
		break;

	case 'w':  //Toggles wireframe mode
		window.wireframe = !window.wireframe;
		break;

	case 'x':  //Exit the program
	case 27:
		glutLeaveMainLoop();
		return;
	case '1': 	
		//Select no weather and go back to the main menu
		if(weatherSelect){
			weatherSelect = false;weatherType = 0;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
		}
		//Select 1st person perspective in certain single player modes
		else if(menuOn && menuSelect > 0 && singlePlayer && gameSelect>1){
			viewPerspective = 1;
			perspectiveOffset = 0.0f;
			perspectiveStepBack = 1.0f;
			countdownOn = true;
			menuOn = false;
			transY = -1;
			RotatedX = 0;
		}
		//Select Moshball for the game in either mode (always 1st person)
		else if(menuOn && menuSelect > 0 && (singlePlayer || twoPlayer)){
			gameSelect = 1;
			viewPerspective = 1;perspectiveOffset = 0.0f;perspectiveStepBack = 1.0f;
			countdownOn = true;menuOn = false;transY = -1;RotatedX = 0;
		}
		//Select single player
		else if(menuOn && menuSelect > 0){singlePlayer = true;}
		//Select jungle stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;stadiumSelect = 1;
			if(tod.y != 2){tod.y = 0;}
		}
		return;
	case '2':  
		//Select fog and go back to the main menu
		if(weatherSelect){
			weatherType = 1;
			tod.y = 2;
			weatherSelect = false;			
		}
		//Select 3rd person view for single player games
		else if(menuOn && menuSelect > 0 && singlePlayer && gameSelect > 1){
			viewPerspective = 3;
			perspectiveOffset = 0.0f;
			perspectiveStepBack = 10.0f;
			countdownOn = true;
			menuOn = false;
			transY = 0;
		}
		//Select the avoid game for single player/ coin collection for 2 player
		else if(menuOn && menuSelect > 0 && (singlePlayer || twoPlayer)){
			gameSelect = 2;
		}
		//Select two player
		else if(menuOn && menuSelect > 0){
			twoPlayer = true;
		}
		//Select ufo stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;
			stadiumSelect = 2;
			if(tod.y != 2){tod.y = 0;}
		}
		return;
	case '3':
		//Select rain and go back to the main menu
		if(weatherSelect){
			weatherType = 2;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
			weatherSelect = false;			
		}
		//Select Coin Time Attack for single player mode
		else if(menuOn && menuSelect > 0 && singlePlayer){
			gameSelect = 3;
		}
		//Select underwater stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;
			stadiumSelect = 3;
			if(tod.y != 2){tod.y = 1;}
		}
		return;
	case '4':
		//Select snow and go back to the main menu
		if(weatherSelect){
			weatherType = 3;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
			weatherSelect = false;
			menuOn = true;			
		}
		//Select weather menu
		else if(menuOn && menuSelect == 0){
			weatherSelect = true;
		}
		else if(!menuOn){
			weatherType = (weatherType + 1)%4;
			if(weatherType == 1){tod.y = 2;}
			else{if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}}

			if(weatherType == 2){
				//Initialize the precipitation
				numParts = 500;
				partX = new float[numParts];
				partY = new float[numParts];
				partOffset = new float[numParts];
				for(int i=0; i<numParts; i++){
					partX[i] = (float)(rand() % 800);
					partX[i] = (partX[i] - 400)/100;

					partY[i] = (float)(rand() % 800);
					partY[i] = (partY[i] - 400)/100;
					partOffset[i] = (float)(rand() % 360 + 1);
				}
			}

			if(weatherType == 3){
				//Initialize the precipitation
				numParts = 250;
				partX = new float[numParts];
				partY = new float[numParts];
				partOffset = new float[numParts];
				for(int i=0; i<numParts; i++){
					partX[i] = (float)(rand() % 800);
					partX[i] = (partX[i] - 400)/100;

					partY[i] = (float)(rand() % 800);
					partY[i] = (partY[i] - 400)/100;
					partOffset[i] = (float)(rand() % 360 + 1);
				}
			}
		}
		return;
	case '5':
		//Cycle through time of day
		//if(menuOn){
			tod.x  = (tod.x + 1)%3;
		//}
		return;
	case '6':
		if(userTeam.soldiers[0].health > 0){
			userTeam.soldiers[0].health--;
		}
		return;
	case '7':
		if(userTeam.soldiers[0].health < 100){
			userTeam.soldiers[0].health++;
		}
		return;
	case 32:   //space bar
		return;
	}	
}

void SpecialFunc(int c, int x, int y)
{
	switch(c){
        case GLUT_KEY_UP:rotateX(1);return;
        case GLUT_KEY_DOWN:rotateX(-1);return;
        case GLUT_KEY_RIGHT:rotateY(-1);return;
        case GLUT_KEY_LEFT:rotateY(1);return;
		case GLUT_KEY_F1:	
			weatherType = (weatherType + 1)%4;
			if(weatherType == 1){tod.y = 2;}
			else{if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}}
			return;
		case GLUT_KEY_F2:
			tod.x  = (tod.x + 1)%3;
			return;
    }
}

void drawRadar(){
		//Set up orthographic projection for radar
		mat4 projection1 = perspective(45.0f, window.window_aspect, 0.01f, 10.0f);
		glm::mat4 Projection2 = glm::ortho( -10.0f, 10.0f, -10.0f, 00.0f,1.0f, 10.0f);
		glm::mat4 View       = glm::lookAt(
			glm::vec3(0,0,5), // Camera is at (0,0,5), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);  
			
		mat4 another = View;
		mat4 healthBarMatrix = View;
		another = translate(another, vec3(8.5,-8.5,0));
		
		another = rotate(another, 90.0f, vec3(0,1,0));
		another = rotate(another, 90.0f, vec3(0,0,1));

		healthBarMatrix = translate(healthBarMatrix, vec3(-9.5,-0.5,0));
		
		//healthBarMatrix = rotate(healthBarMatrix, 90.0f, vec3(0,1,0));
		healthBarMatrix = rotate(healthBarMatrix, -90.0f, vec3(0,0,1));
		healthBarMatrix = scale(healthBarMatrix, vec3(0.05f, userTeam.soldiers[0].health/10.f, 0.05f));

		if(userTeam.soldiers[0].health < 50){
			healthBar.color = vec3(1.f, userTeam.soldiers[0].health/50.f, 0.f);
		}
		else{
			healthBar.color = vec3(1.f - (userTeam.soldiers[0].health-50.f)/50.f, 1.f, 0.f);
		}
		healthBar.Draw(Projection2, healthBarMatrix, tod, 0);

		mat4 radar = another;

		//Radar display
		
		int radarBlipTime = current_timeDisplay * 1000;
		float radarBlip = 0.0015 * (radarBlipTime%1000);
		radar = scale(radar, vec3(radarBlip, radarBlip, radarBlip));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		disc.color = vec3(0,1,0);
		disc.Draw(Projection2, radar, tod, 0);

		another = scale(another, vec3(1.5, 1.5, 1.5));
		disc.Draw(Projection2, another, tod, 0);

		

		glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
		disc.color = vec3(0,0.25,0);
		disc.Draw(Projection2, another, tod, 0);

		another = translate(View, vec3(8.5 - ((3/104.6)*transX),-8.6 + ((3/104.6)*transZ),0));
		usr.Draw(Projection2, another, tod, 0);

		stringstream ammo (stringstream::in | stringstream::out);
		ammo << userTeam.soldiers[0].ammo;
		activeTextDisplay(("Ammo: " + ammo.str()).c_str(), -0.95f, -0.85f, 16);
}

void drawPrecipitationWithRadar(){
	//Make the skybox based on weather//
		modelview = endRender(modelview);

		modelview = rotate(modelview, (GLfloat) RotatedX, vec3(1,0,0));
		modelview = rotate(modelview, (GLfloat) RotatedY, vec3(0,1,0));
		skybox.Draw(projection, modelview, tod, 0);
		
		//Make precipitation
		if(weatherType > 1){
			mat4 another;
			for(int i=0; i<numParts; i++){
				another = rotate(modelview, 90.0f, vec3(1,0,0));
				another = translate(another, vec3(partX[i],partY[i],3*tan(((weatherType == 2 ? 2 : 0.75) *current_timeDisplay)+partOffset[i])));
				another = rotate(another, 270.0f, vec3(1,0,0));
				another = rotate(another, 90.0f, vec3(0,1,0));
				if(weatherType == 2){
					mat4 rainScale = another;rainScale = scale(rainScale, vec3(0.35, 2, 0.35));
					rain2.Draw(projection, rainScale, tod, 0);
				}
				else{snow2.Draw(projection, another, tod, 0);}
			}
		}
			
		modelview = rotate(modelview, (GLfloat) -RotatedY, vec3(0,1,0));
		modelview = rotate(modelview, (GLfloat) -RotatedX, vec3(1,0,0));

		drawRadar();

		modelview = render(modelview);
}

void checkDamage(){
	//cout << "check damage" << endl;
	for(int i=0; i<userTeam.NUM_SOLDIERS; i++){
		for(int j=0; j<cpuTeam.NUM_SOLDIERS; j++){
			if(sqrt(dot(vec3(userTeam.points[i].x-cpuTeam.bulletLoc[j].x, userTeam.points[i].y+1.5-cpuTeam.bulletLoc[j].y, userTeam.points[i].z-cpuTeam.bulletLoc[j].z), 
				vec3(userTeam.points[i].x-cpuTeam.bulletLoc[j].x, userTeam.points[i].y+1.5-cpuTeam.bulletLoc[j].y, userTeam.points[i].z-cpuTeam.bulletLoc[j].z))) < 2){
				userTeam.soldiers[i].health -= 2;
				//cout << "user shot cpu" << endl;
				if(userTeam.soldiers[i].health <= 0){userTeam.soldiers[i].health = 0;userTeam.soldiers[i].dead = true;}
			}
		}
		//if(i == 0){cout << "sqrt: " << sqrt(dot(vec3(userTeam.points[i].x-cpuTeam.bulletLoc[i].x, userTeam.points[i].y+1.5-cpuTeam.bulletLoc[i].y, userTeam.points[i].z-cpuTeam.bulletLoc[i].z), 
			//		vec3(userTeam.points[i].x-cpuTeam.bulletLoc[i].x, userTeam.points[i].y+1.5-cpuTeam.bulletLoc[i].y, userTeam.points[i].z-cpuTeam.bulletLoc[i].z))) << endl;}
		//if(i == 0){cout << "soldier = (" << userTeam.points[i].x << ", " << userTeam.points[i].y + 1.5 << ", " << userTeam.points[i].z << ")" << endl;}
		//if(i == 0){cout << "bullet = (" << cpuTeam.bulletLoc[i].x << ", " << cpuTeam.bulletLoc[i].y << ", " << cpuTeam.bulletLoc[i].z << ")" << endl;}
	}

	for(int i=0; i<cpuTeam.NUM_SOLDIERS; i++){
		for(int j=0; j<userTeam.NUM_SOLDIERS; j++){
			if(sqrt(dot(vec3(cpuTeam.points[i].x-userTeam.bulletLoc[j].x, cpuTeam.points[i].y+1.5-userTeam.bulletLoc[j].y, cpuTeam.points[i].z-userTeam.bulletLoc[j].z), 
				vec3(cpuTeam.points[i].x-userTeam.bulletLoc[j].x, cpuTeam.points[i].y+1.5-userTeam.bulletLoc[j].y, cpuTeam.points[i].z-userTeam.bulletLoc[j].z))) < 2){
				cpuTeam.soldiers[i].health -= 2;
				//cout << "cpu shot user" << endl;
				if(cpuTeam.soldiers[i].health <= 0){cpuTeam.soldiers[i].health = 0;cpuTeam.soldiers[i].dead = true;}
			}
		}
	}
}

void drawStadium(){
	//Change the stadium to match the weather
	modelview = translate(modelview, vec3(499.f,-1,500.f));
	//mat4 scaler = scale(modelview, vec3(2.2,2.2, 2.2));
	stadium.Draw(projection, modelview, tod, 0.0f);
	modelview = translate(modelview, vec3(-499.f,0,-500.f));
}

//Orchestrates all the objects and variables into a playable game
void GameDisplay(){
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	perspectiveStepBack = 1.0f;
	projection = perspective(25.0f, window.window_aspect, 0.01f, 1200000000.0f);
	modelview = lookAt(vec3(0.0f, perspectiveOffset, perspectiveStepBack), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	modelview = render(modelview);
	glLoadMatrixf(value_ptr(modelview));
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);	
	/////////////////////////////Menu Display////////////////////////////////
	if(menuOn){

		menuSelect = 1;stadiumSelect = 1;
		singlePlayer = true;

		viewPerspective = 3;
		perspectiveOffset = 0.0f;
		perspectiveStepBack = 10.0f;
		countdownOn = true;
		menuOn = false;
		transY = 0;
		gameSelect = 2;

	}
	//Countdown Display
	else if(countdownOn){
			if(first == 1){
				//alSourceStop(origSource);
				//init((ALbyte *)"./music/havingAWildTime.wav", true);
				first = 2;
			}

			countdownTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;countdownTime -= menuTime;
			countdownOn = false;
			
	}

	///////////////////////Game Display///////////////////////////
	else{
		current_timeDisplay = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		current_timeDisplay = (window.paused ? window.time_last_pause_began : current_timeDisplay) - window.total_time_paused;
		
		//Mouse movement
		mouseRotations(stadiumSelect, viewPerspective);

		//Stop the clock, user, and score calculations when caught
		if(!caught){

			modelview = endRender(modelview);
			
			mat4 scaler;
			scaler = translate(modelview, vec3(0.0f, -1.f, 0.0f));
			scaler = rotate(scaler, 90.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			userTeam.drawUser = true;userTeam.drawBullet = false;
			//userTeam.Draw(projection, scaler, window.size, 0);
			userTeam.points[0].x=transX;userTeam.points[0].y=transY;userTeam.points[0].z=transZ;
			//userTeam.points[0].hRot = RotatedY;userTeam.points[0].vRot=RotatedX;

			modelview = render(modelview);

			

		}			

		//drawStadium();
		ss.Draw(projection, modelview, window.size, 0);

		
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFlush();
	
}

//Manage pausing and when to display the game / jumbotrons
void DisplayFunc(){
	glViewport(0, 0, window.size.x, window.size.y);
	if(!window.paused){
	GameDisplay();
	glutSwapBuffers();
	}
	
}

void TimerFunc(int value){
	if (window.window_handle != -1){glutTimerFunc(window.interval, TimerFunc, value);glutPostRedisplay();}
}

int main(int argc, char * argv[])
{
	glewInit();
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	//Initialize object's colors / values
	torus.color = vec3(1.0f, 0.1f, 0.0f);
	square.color1 = vec3(1.0, 1.0, 1.0);
	square2.color1 = vec3(0.95f, 0.1f, 0.7f);
	square2.color2 = vec3(1.0f, 1.0f, 0.0f);
	disc.color = vec3(0.1f, 0.5f, 1.0f);
	sphere.color = vec3(0.15f, 0.45f, 0.0f);
	sphere2.color = vec3(0.45f, 0.05f, 0.25f);
	cylinder.color = vec3(0.0f, 0.5f, 0.0f);
	arena.texID = 18;
	egg.texID = 20;
	star.texID = 22;
	magneton.texID = 25;
	haunter.texID = 26;
	frog.texID = 27;
	goldeen.texID = 76;
	rain2.color = vec3(0.4f, 0.4f, 1.0f);
	snow2.color = vec3(1.0f, 1.0f, 1.0f);
	fontfilename = "Pokemon.ttf";

	//Initialize the precipitation
	numParts = 300;
	partX = new float[numParts];
	partY = new float[numParts];
	partOffset = new float[numParts];
	for(int i=0; i<numParts; i++){
		partX[i] = (float)(rand() % 1200);
		partX[i] = (partX[i] - 600)/100;

		partY[i] = (float)(rand() % 1200);
		partY[i] = (partY[i] - 600)/100;
		partOffset[i] = (float)(rand() % 360 + 1);
	}
	sphere.lava = true;sphere2.lava = false; //Initialize sphere statuses

	window.window_handle = glutCreateWindow("Solar System");
	glutReshapeFunc(ReshapeFunc);
	glutCloseFunc(CloseFunc);
	
	glutMotionFunc(mouseMovement);
	glutPassiveMotionFunc(mouseMovement); //check for mouse movement
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutTimerFunc(window.interval, TimerFunc, 0);
	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	
	//Initialize everything

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	init_resources();
	glutDisplayFunc(DisplayFunc);

	if (!fbo.Initialize(glm::ivec2(512, 512), 1, true)){
		cerr << "Frame buffer failed to initialize." << endl;
		return 0;
	}

	sph1.color = vec3(1.0f, 0.85f, 0.0f);
	if (!sph1.Initialize(8,1.0f, "phong3.vert", "phong3.frag")){return 0;}
	enm.color = vec3(0.75f, 0.0f, 0.75f);
	if (!enm.Initialize(2,0.05f, "phong.vert", "phong.frag")){return 0;}
	usr.color = vec3(0.0f, 0.0f, 0.5f);
	if (!usr.Initialize(2,0.05f, "phong.vert", "phong.frag")){return 0;}
	//if (!sb.Initialize()){return 0;}
	//if (!coin.Initialize()){return 0;}
	//if (!stadium.Initialize(0)){return 0;}
	if (!skybox.Initialize(0, 5000, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skybox2.Initialize(1, 5025, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skybox3.Initialize(2, 5050, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skyboxUW.Initialize(3, 5075, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}


	arena.order = 1;
	
	if (!sphere.Initialize(15, 1, "sphereShader2.vert", "sphereShader2.frag")){return 0;}
	if (!sphere2.Initialize(15, 1, "sphereShader2.vert", "sphereShader2.frag")){return 0;}
	if (!rain2.Initialize(2, 0.02f, "phong.vert", "phong.frag")){return 0;}
	if (!snow2.Initialize(3, 0.02f, "phong.vert", "phong.frag")){return 0;}
	//if (!egg.Initialize(15, 1, "./textures/alienEgg.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag")){return 0;}
	if (!square.Initialize(1, 3.0f, "phong.vert", "phong.frag")) {return 0;}
	if (!sq4.Initialize(1, 5.0f, "basic_texture_shader.vert", "basic_texture_shader.frag")) {return 0;}
	if (!disc.Initialize(8, 1.0f, "phong3.vert", "phong3.frag")) {return 0;}
	if (!healthBar.Initialize(2, 1.0f, 1.f, 1.f, "phong3.vert", "phong3.frag")) {return 0;}
	if (!soldier.Initialize()) {return 0;}
	if (!ss.Initialize()) {return 0;}

	userTeam.userTeam = true;
	if (!userTeam.Initialize()) {return 0;}
	if (!cpuTeam.Initialize()) {return 0;}

	glutMainLoop();
}