//
//  main.cpp
//  OpenGL_Shader_Example_step1
//
//  Created by CGIS on 02/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
#else
	#define GLEW_STATIC
	#include <GLEW/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" 
#include "glm/gtc/type_ptr.hpp"

#include <string>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream> 
#include <fstream> 

#include <irrKlang.h> 
using namespace irrklang; 

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 viewPos;
GLuint viewPosLoc;

//gps::Camera myCamera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 5.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 5.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)); 

// daca vrem ca obiectul sa se miste mai repede modificam asta (era 0.1f initial)
float cameraSpeed = 0.05f;

bool pressedKeys[1024];
float angle = 0.0f;

int fog = 0; 

int dirLight = 1; 

int bulbEn = 0;
int bulbMonumentEn = 0; 

float thunderTimer = 4.1f;  // timpul pentru a urmari pauza
float thunderInterval = 6.0f; 
int thunderEn = 0; 

gps::Model3D myModel;
gps::Shader myCustomShader;

// rain
gps::Model3D rainDropModel;
const int nrRaindrops = 10000;
std::vector<glm::vec3> raindropPositions;  
int rainEn = 0; 


// sound
irrklang::ISoundEngine* engine;   
irrklang::ISound* thunderSound; 
irrklang::ISound* rainSound;
irrklang::ISound* presentationSound; 
irrklang::ISound* generalSound; 

// automated tour
int autoTour = 0; 
std::ifstream f("pos.txt"); 
std::ifstream g("target.txt"); 

gps::Model3D boatModel; 
glm::vec3 boatPosition(5.34f, -0.5f, 21.9f);    
float boatSpeed = 0.02f; 


void initEngine() {
	engine = irrklang::createIrrKlangDevice();
	if (!engine) {
		return; //eroare daca nu functioneaza  
	}
}

void playThunderSound() {
	thunderSound = engine -> play2D("audio/loud-thunder.ogg", GL_FALSE); 
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {

	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// fog
	if (pressedKeys[GLFW_KEY_F]) {
		fog = 1 - fog;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
	} 

	if (pressedKeys[GLFW_KEY_L]) {
		dirLight = 1 - dirLight;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight"), dirLight);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_I))
	{  
		bulbEn = 1 - bulbEn; 
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "bulbEn"), bulbEn);
	} 


	if (glfwGetKey(glWindow, GLFW_KEY_O))
	{
		bulbMonumentEn = 1 - bulbMonumentEn; 
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "bulbMonumentEn"), bulbMonumentEn); 
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		thunderEn = 1 - thunderEn;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "thunderEn"), thunderEn); 
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "thunderTimer"), thunderTimer);  
	}  
 
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		rainEn = 1 - rainEn;
		if (rainEn) {
			rainSound = engine->play2D("audio/calming-rain-257596.ogg", GL_TRUE, GL_FALSE, GL_TRUE); 
		}
		else {
			rainSound->stop(); 
			rainSound->drop(); 
			rainSound = nullptr; 
		}
	}

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		autoTour = 1;
		if (!f.is_open())
			f.open("pos.txt");
		if (!g.is_open())
			g.open("target.txt");

		if (!presentationSound) { 
			presentationSound = engine->play2D("audio/10. Fun Festival-[AudioTrimmer.com].ogg", GL_FALSE, GL_FALSE, GL_TRUE);
		}
	}

	if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
		if (generalSound) {
			generalSound->stop();
			generalSound->drop(); 
			generalSound = nullptr;
		}
		else {
			generalSound = engine->play2D("audio/01. Stardew Valley Overture.ogg", GL_TRUE, GL_FALSE, GL_TRUE);
		}
	}


	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		autoTour = 0;
		if (f.is_open())
			f.close();
		if (g.is_open())
			g.close();

		if (presentationSound) {
			presentationSound->stop();
			presentationSound->drop();
			presentationSound = nullptr;
		}
	}
	 
	if (key >= 0 && key < 1024)	{

		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

bool mouseEn = true;
float yaw = 0.0f; 
float pitch = 0.0f; // privire orizontala
float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
float zoom = 45.0f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	// prima miscare mouse
	if (mouseEn)
	{
		// salvam coord mouse
		lastX = xpos;
		lastY = ypos;
		mouseEn = false;
	}

	//offset mouse - cat de mult s-a deplasat mouse-ul pe x si y
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos; 
	lastX = xpos; 
	lastY = ypos;

	// sensibilitate mouse 
	float sensibility = 0.1f; 
	offsetX *= sensibility; 
	offsetY *= sensibility; 

	// calculam unghiul de rotatie
	yaw += offsetX;
	pitch += offsetY;

	// previne flip camera
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw); 
}

gps::MOVE_DIRECTION scrollDirection;  
void scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	zoom -= (float)offsetY; // amount scrolled vertically

	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;

	if (offsetX < offsetY)
		scrollDirection = gps::MOVE_BACKWARD;
	else
		scrollDirection = gps::MOVE_FORWARD;

	myCamera.move(scrollDirection, 0.1f); 
} 

void processMovement()
{

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		boatPosition.z += boatSpeed;
	}
	if (pressedKeys[GLFW_KEY_DOWN]) { 
		boatPosition.z -= boatSpeed;
	}
	if (pressedKeys[GLFW_KEY_LEFT]) { 
		boatPosition.x += boatSpeed; 
	}
	if (pressedKeys[GLFW_KEY_RIGHT]) {
		boatPosition.x -= boatSpeed;
	}

	//  wireframe
	if (glfwGetKey(glWindow, GLFW_KEY_1)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//solid
	if (glfwGetKey(glWindow, GLFW_KEY_2)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// punctiform
	if (glfwGetKey(glWindow, GLFW_KEY_3)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

}

bool initOpenGLWindow() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for multisampling/antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "Mountain Village", NULL, NULL);

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetScrollCallback(glWindow, scrollCallback); 
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState() {
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

}

void initObjects() {
	myModel.LoadModel("objects/mainScene/mainScene.obj");
	rainDropModel.LoadModel("objects/rainDrop2/rainDrop2.obj"); 
	boatModel.LoadModel("objects/boat3/boat3.obj");

}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); 

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	lightDir = glm::vec3(0.0f, 1.0f, 1.0f); // 45grade intre y si z
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //alb
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//initialize the projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); 

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "dirLight"), dirLight); 
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
} 

void initPointLight() {

	myCustomShader.useShaderProgram(); 

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].position"), -1.5f, 2.25f, -8.0f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].diffuseP"), 1.0f, 1.0f, 1.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].specularP"), 10.0f, 4.0f, 2.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].linear"), 3.7f); 
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].quadratic"), 3.5f);


	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].position"), -7.0f, 1.5f, -1.0f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].diffuseP"), 1.0f, 1.0f, 1.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].specularP"), 10.0f, 4.0f, 2.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].linear"), 7.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].quadratic"), 7.5f);

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].position"), 4.5f, 1.5f, -0.5f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].diffuseP"), 1.0f, 1.0f, 1.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].specularP"), 10.0f, 4.0f, 2.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].linear"), 7.7f); 
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].quadratic"), 7.5f); 

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].position"), 4.5f, 1.5f, -7.0f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].diffuseP"), 1.0f, 1.0f, 1.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].specularP"), 10.0f, 4.0f, 2.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].linear"), 7.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[3].quadratic"), 7.5f);

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].position"), -7.0f, 1.5f, -7.5f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].diffuseP"), 1.0f, 1.0f, 1.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].specularP"), 10.0f, 4.0f, 2.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].linear"), 7.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[4].quadratic"), 7.5f);
}



void initRainDrops() {
	for (int i = 0; i < nrRaindrops; i++) {
		float x = static_cast<float>(rand()) / RAND_MAX * 40.0f - 20.0f; // Între -20 și 20 pentru x
		float y = static_cast<float>(rand()) / RAND_MAX * 20.0f + 20.0f; // Între 20 și 40 pentru y
		float z = static_cast<float>(rand()) / RAND_MAX * 80.0f - 40.0f; // Între -40 și 40 pentru z
		raindropPositions.push_back(glm::vec3(x, y, z));
	} 
}

void updateRaindrops(float deltaTime) {
	for (auto& pos : raindropPositions) {
		pos.y -= 5.0f * deltaTime; // viteza de cadere
		if (pos.y < -5.0f) {
			pos.y = 5.0f; // reseteaza deasupra scenei
		}
	}
} 

double lastTime = glfwGetTime();  

void renderScene()
{
	double currentTime = glfwGetTime();  // Timpul curent
	float deltaTime = float(currentTime - lastTime);  // calculam deltaTime
	lastTime = currentTime;  // actualizam lastTime pentru urmatorul cadru 

	if (thunderEn == 1) {
		if (thunderTimer >= thunderInterval) {  
			thunderTimer = 0.0f;  
			playThunderSound();  
		}
		else {

			thunderTimer+= deltaTime;
		} 
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "thunderTimer"), thunderTimer); 
	}

	if (rainEn) {
		updateRaindrops(deltaTime);
	} 

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model"); 

	myCustomShader.useShaderProgram();

	//initialize the view matrix
	glm::mat4 view = myCamera.getViewMatrix();
	//send matrix data to shader
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	
	// pentru incarcare obj 3D
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	myModel.Draw(myCustomShader);

	if (rainEn) {
		for (const auto& pos : raindropPositions) {
			glm::mat4 rainModel = glm::mat4(1.0f);
			rainModel = glm::translate(rainModel, pos); // translatam picatura la pozitia curenta
			rainModel = glm::scale(rainModel, glm::vec3(0.02f)); // scalam picatura 

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rainModel));
			rainDropModel.Draw(myCustomShader);
		}
	} 

	glm::mat4 boatModelMatrix = glm::translate(glm::mat4(1.0f), boatPosition); 
	float angle = glm::radians(25.0f);
	boatModelMatrix = glm::rotate(boatModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f)); 
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boatModelMatrix));  
	boatModel.Draw(myCustomShader);  

}

void cleanup() {
    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
	    glfwTerminate();
	    return 1;
	}

	initOpenGLState();
	initShaders(); 
	initUniforms(); 
	initObjects(); 

	initPointLight();
	initRainDrops(); 

	initEngine(); 


	while (!glfwWindowShouldClose(glWindow)) {
        processMovement();

		if (autoTour == 1) {

			glm::vec3 position; 
			f >> position.x; 
			f >> position.y;
			f >> position.z; 

			myCamera.setCameraPosition(position); 

			glm::vec3 frontDir;
			g >> frontDir.x;
			g >> frontDir.y;
			g >> frontDir.z;

			myCamera.setCameraFrontDirection(frontDir); 
		}

		renderScene();


		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
