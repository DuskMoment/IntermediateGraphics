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


// weenis peenis
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
// fuck u willison
ew::CameraController controller;

//transfrom
ew::Transform modelTrans;

//framebuffer
wm::FrameBuffer shadowMap;

//texutres
GLuint brickTexture;
GLuint normalMapping;

struct Material 
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

struct Light
{
	glm::vec3 lightDirection = glm::vec3(0.0, -1.0, 0.0);
	glm::vec3 lightColor = glm::vec3(1.0);
	float bias = 0.01;

}light;

void renderMonekey(ew::Shader& shader, ew::Shader& shadowMapShdr, glm::mat4 lightMat, ew::Model& model, ew::Mesh plane, GLFWwindow* window)
{
	modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	//draw to the shadow buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.fbo);
	{	
		glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);

		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
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

	shader.use();
	
	//camera uniforms
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setMat4("_LightSpaceMatrix", lightMat);
	shader.setVec3("_EyePos", camera.position);

	//shadows
	shader.setFloat("_Bias", light.bias);

	//model uniforms
	
	shader.setMat4("_Model", modelTrans.modelMatrix());

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


	shader.setMat4("_Model", glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)));
	//draw plane
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
	ew::Shader shadowMapShdr = ew::Shader("assets/shadowMap.vert", "assets/shadowMap.frag");

	ew::Mesh plane = ew::createPlane(100, 100, 10);

	ew::Model model = ew::Model("assets/Suzanne.fbx");

	//init camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//texture
	brickTexture = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_Color.jpg");
	normalMapping = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_NormalDX.jpg");
	//brickTexture = ew::loadTexture("assets/brick_color.jpg");
	
	shadowMap = wm::createShadowBuffer(SHADOW_SIZE, SHADOW_SIZE);

	//shadow mapping light
	float near_plane = 1.0f, far_plane = 7.5f;

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(glm::vec3(2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	ew::Camera lightCam;

	lightCam.target = glm::vec3();
	lightCam.position = light.lightDirection;
	lightCam.orthographic = true;
	lightCam.orthoHeight = 10.0f;
	lightCam.aspectRatio = 1.0f;
	lightCam.fov = 60.0f;


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		const glm::mat4 lightSpaceMat = lightProjection * lightView;
		//const glm::mat4 lightSpaceMat = lightCam.projectionMatrix() * lightCam.viewMatrix();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;
		
		//RENDER
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderMonekey(blin, shadowMapShdr, lightSpaceMat, model, plane, window);

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
		ImGui::SliderFloat("Bias", &light.bias, 0.001f, 0.1f);

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

