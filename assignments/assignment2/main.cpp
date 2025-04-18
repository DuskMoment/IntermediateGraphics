#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/transform.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include<wm/framebuffer.h>

#include "glm/gtx/transform.hpp"


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

const int SHADOW_SIZE = 500;

//camera
ew::Camera camera;

ew::CameraController controller;

//transfrom
ew::Transform modelTrans;

//framebuffer
wm::FrameBuffer shadowMap;

//texutres
GLuint brickTexture;
GLuint normalMapping;
GLuint snow;

bool isSnow = false;

struct Material 
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

struct Light
{
	glm::vec3 lightDirection = glm::vec3(-2.0f, 4.0f, -1.0f);
	glm::vec3 lightColor = glm::vec3(1.0);
	float bias = 0.01;
	float maxBias = 0.01;
	bool pcf = false;
	int pcfAmmount = 1;

}light;

void renderMonekey(ew::Shader& shader, ew::Shader& deform, ew::Shader& shadowMapShdr, glm::mat4 lightMat, ew::Model& model, ew::Mesh plane, GLFWwindow* window)
{
	modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	//draw to the shadow buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.fbo);
	{	
		glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_TEST);
		if (!isSnow)
		{
			glClear(GL_DEPTH_BUFFER_BIT);

		}
		shadowMapShdr.use();
		shadowMapShdr.setMat4("lightSpaceMatrix", lightMat);
		shadowMapShdr.setMat4("model", modelTrans.modelMatrix());

		model.draw();
	}
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//fix view port
	glViewport(0, 0, screenWidth, screenHeight);

	//pipeline definition
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	//create a gfx pass
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapping);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowMap.depthBuffer);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, snow);

	shader.use();
	
	//camera uniforms
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setMat4("_LightSpaceMatrix", lightMat);
	shader.setVec3("_EyePos", camera.position);

	//shadows
	shader.setFloat("_Bias", light.bias);
	shader.setFloat("_BiasMax", light.bias);
	shader.setInt("_PCF", light.pcf);
	shader.setInt("_PCFAmmount", light.pcfAmmount);

	//model uniforms
	shader.setMat4("_Model", modelTrans.modelMatrix());

	//light
	shader.setVec3("_LightDirection", light.lightDirection);

	//material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	//textures
	shader.setInt("_MainTex", 0);
	shader.setInt("_NormalMap", 1);
	shader.setInt("_ShadowMap", 2);

	model.draw();

	if (isSnow)
	{
		deform.use();

		//draw plane

		//camera uniforms
		deform.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
		deform.setMat4("_LightSpaceMatrix", lightMat);
		deform.setVec3("_EyePos", camera.position);

		//shadows
		deform.setFloat("_Bias", light.bias);

		//model uniforms

		deform.setMat4("_Model", glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)));

		//material
		deform.setFloat("_Material.Ka", material.Ka);
		deform.setFloat("_Material.Kd", material.Kd);
		deform.setFloat("_Material.Ks", material.Ks);
		deform.setFloat("_Material.Shininess", material.Shininess);

		//textures
		deform.setInt("_MainTex", 3);
		deform.setInt("_NormalMap", 1);
		deform.setInt("_ShadowMap", 2);
		deform.setInt("_ImprintMap", 2);
	}
	else
	{
		shader.setMat4("_Model", glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)));
	}

	plane.draw();

	controller.move(window, &camera, deltaTime);

}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0.0, 0.0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;

}

int main() {

	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//chache 
	
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader blin = ew::Shader("assets/blin.vert", "assets/blin.frag");
	ew::Shader deform = ew::Shader("assets/Deform.vert", "assets/Deform.frag");
	ew::Shader shadowMapShdr = ew::Shader("assets/shadowMap.vert", "assets/shadowMap.frag");

	ew::Mesh plane = ew::createPlane(10, 10, 100);

	ew::Model model = ew::Model("assets/Suzanne.fbx");

	//init camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//texture
	brickTexture = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_Color.jpg");
	normalMapping = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_NormalDX.jpg");
	snow = ew::loadTexture("assets/snow.png");
	//brickTexture = ew::loadTexture("assets/brick_color.jpg");
	
	shadowMap = wm::createShadowBuffer(SHADOW_SIZE, SHADOW_SIZE);

	//shadow mapping light
	float near_plane = 0.01f, far_plane = 100;

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	//-2.0f, 4.0f, -1.0f
	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		glm::mat4 lightView = glm::lookAt(light.lightDirection,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		const glm::mat4 lightSpaceMat = lightProjection * lightView;
		//const glm::mat4 lightSpaceMat = lightCam.projectionMatrix() * lightCam.viewMatrix();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;
		
		//RENDER
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderMonekey(blin, deform, shadowMapShdr,lightSpaceMat, model, plane, window);

		drawUI();
		

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");

}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	ImGui::Text("Add Controls Here!");
	
	if (ImGui::CollapsingHeader("Model"))
	{

		ImGui::DragFloat3("model position", &modelTrans.position.x);
		//ImGui::SliderFloat("Bias", &light.bias, 0.001f, 0.1f);

	}
	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &controller);
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SepcularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);

	}
	if (ImGui::CollapsingHeader("Shadow"))
	{
		ImGui::SliderFloat("Min Bias", &light.bias, 0.0f, 0.01f);
		ImGui::SliderFloat("Max Bias", &light.maxBias, 0.0f, 0.5f);
		ImGui::Checkbox("PCF", &light.pcf);
		ImGui::SliderInt("PCF Filter Ammount", &light.pcfAmmount, 1, 10);

	}
	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::DragFloat3("Light Position", &light.lightDirection.x);
	}
	if (ImGui::CollapsingHeader("SUPER SECRET SETTINGS!!!!"))
	{
		ImGui::Checkbox("SnowMode", &isSnow);
	}
	ImGui::Image((ImTextureID)(intptr_t)shadowMap.depthBuffer, ImVec2(400, 300));

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

