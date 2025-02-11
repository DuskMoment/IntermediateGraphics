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
#include <iostream>

#include <tuple>


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

//camera
ew::Camera camera;
ew::CameraController controller;

//transfrom
ew::Transform modelTrans;

//texutres
GLuint brickTexture;
GLuint ZAtoon;
GLuint normalMapping;

//for debug
struct
{
	glm::vec3 color = glm::vec3(0, 0.31f, 0.81f);
	float tile = 1.0f;
	float speed = 1.0f;
	float b1 = 1.0f;
	float b2 = 1.0f;
	float strenght = 1.0f;

}debug;

typedef struct 
{

	glm::vec3 highlight;
	glm::vec3 shadow;

}Palette;

static int palette_index = 0;
static std::vector<std::tuple<std::string, Palette>> palette{
	{"Sunny Day", {{1.00f, 1.00f, 1.00f}, {0.60f, 0.54f, 0.52f}}},
	{"Bright Night", {{0.47f, 0.58f, 0.68f}, {0.32f, 0.39f, 0.57f}}},
	{"Rainy Day", {{0.62f, 0.69f, 0.67f},{0.50f, 0.55f, 0.50f}}},
	{"Rainy Night", {{0.24f, 0.36f, 0.54f},{0.25f, 0.31f, 0.31f}}},
};



struct Material 
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;


ew::Mesh plane;
void renderMonekey(ew::Shader& shader, ew::Model& model, GLFWwindow* window)
{
	
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
	glBindTexture(GL_TEXTURE_2D, ZAtoon);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normalMapping);


	shader.use();
	
	//camera uniforms
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setVec3("_EyePos", camera.position);

	//model uniforms
	modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("_Model", modelTrans.modelMatrix());

	//material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	//pallet uniforms
	shader.setVec3("_Pallet.highlight", std::get<Palette>(palette[palette_index]).highlight);
	shader.setVec3("_Pallet.shadow", std::get<Palette>(palette[palette_index]).shadow);

	
	//textures
	shader.setInt("_MainTex", 0);
	shader.setInt("zatoon", 1);
	//toonShader.setInt("_NormalMap", 1);

	model.draw();

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
	ew::Shader toonShader = ew::Shader("assets/toon.vert", "assets/toon.frag");
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader planeShader = ew::Shader("assets/windwaker/water.vert", "assets/windwaker/water.frag");
	ew::Model model = ew::Model("assets/skull.obj");

	plane = ew::createPlane(50.0f, 50.0f, 100);
	

	modelTrans = ew::Transform();

	modelTrans.scale = glm::vec3(0.5);


	//init camera
	camera.position = glm::vec3(0.0f, 10.0f, 10.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//texture
	brickTexture = ew::loadTexture("assets/Txo_dokuo.png");
	ZAtoon = ew::loadTexture("assets/ZAtoon.png");
	GLuint waterTex = ew::loadTexture("assets/windwaker/water.png");

	//normalMapping = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_NormalDX.jpg");
	//brickTexture = ew::loadTexture("assets/brick_color.jpg");
	


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		const auto view_proj = camera.projectionMatrix() * camera.viewMatrix();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTex);

		//renderMonekey(toonShader, model, window);
		planeShader.use();

		planeShader.setMat4("model", glm::mat4(1.0f));
		planeShader.setMat4("view_proj", view_proj);

		planeShader.setVec3("water_color", debug.color);
		planeShader.setVec3("camera_pos", camera.position);

		planeShader.setInt("waterTex", 0);
		planeShader.setFloat("tile", debug.tile);
		planeShader.setFloat("time", time);
		planeShader.setFloat("b1", debug.b1);
		planeShader.setFloat("b2", debug.b2);
		planeShader.setFloat("strenght", debug.strenght);
		

		plane.draw();

		controller.move(window, &camera, deltaTime);

		glCheckError();
		
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
	if (ImGui::BeginCombo("Palette", std::get<std::string>(palette[palette_index]).c_str()))
	{
		for (auto n = 0; n < palette.size(); ++n)
		{
			auto is_selected = (std::get<0>(palette[palette_index]) == std::get<0>(palette[n]));
			if (ImGui::Selectable(std::get<std::string>(palette[n]).c_str(), is_selected))
			{
				palette_index = n;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::ColorEdit3("Highlight", &std::get<Palette>(palette[palette_index]).highlight[0]);
	ImGui::ColorEdit3("Shadow", &std::get<Palette>(palette[palette_index]).shadow[0]);

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

	ImGui::ColorEdit3("debug", &debug.color.x, 0);
	ImGui::SliderFloat("tilling", &debug.tile, 1.0f, 100.0f);
	ImGui::SliderFloat("speed", &debug.speed, 1.0f, 100.0f);
	ImGui::SliderFloat("b1", &debug.b1, 0.0f, 1.0f);
	ImGui::SliderFloat("b2", &debug.b2, 0.0f, 1.0f);
	ImGui::SliderFloat("strenght", &debug.strenght, 1.0f, 100.0f);
	
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

