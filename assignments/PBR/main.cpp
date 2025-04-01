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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <wm/framebuffer.h>
#include <random>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

const int SUZAN_X = 1;
const int SUZAN_Y = 1;

//camera
ew::Camera camera;
ew::CameraController controller;

//transfrom
ew::Transform modelTrans;
ew::Transform lightTrans;

//texutres
GLuint brickTexture;
GLuint normalMapping;

wm::FrameBuffer framebuffer;
wm::FrameBuffer lightBuffer;
wm::FrameBuffer testBuffer;

struct Material 
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

struct FullscreenQuad
{
	GLuint vao;
	GLuint vbo;

}fullscreenQuad;

static float quad_vertices[] = {
	// pos (x, y) texcoord (u, v)
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f,
};

struct Light
{
	glm::vec3 pos;
	glm::vec3 color;
	float radius =3;
};
Light lights[SUZAN_X * SUZAN_Y];

//random nuber gen



void calcualteLightRange(float x, float y, int lightIndex)
{
	//fix me :3
	//static std::random_device rd;
	//static std::mt19937 gen(rd());
	//static std::uniform_real_distribution<>dis(x, y + 3);
	float randX = x;//dis(gen);
	float randZ = y; //dis(gen);

	lights[lightIndex].pos = glm::vec3(randX, 2, randZ);

	int r = rand() % 2;
	int g = rand() % 2;
	int b = rand() % 2;

	lights[lightIndex].color = glm::vec3(r,g,b);
	
}

void renderMonekey(ew::Shader& shader, ew::Model& model, ew::Mesh& light, GLFWwindow* window)
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
	//pipeline definition
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	//create a gfx pass
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapping);


	shader.use();
	
	//camera uniforms
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setVec3("_EyePos", camera.position);

	//model uniforms
	//modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("_Model", modelTrans.modelMatrix());

	//material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	shader.setVec3("_Color", glm::vec3(1.0,1.0,1.0));

	//textures
	shader.setInt("_Texture", 0);

	for (int i = -1; i < SUZAN_X; i++)
	{
		for (int j = -1; j < SUZAN_Y; j++)
		{
			model.draw();

			shader.setMat4("_Model", glm::translate(glm::vec3(2.0f * i, 0 , 2.0f * j)));

			//calcualteLightRange(2.0f * i, 2.0f * j, i+j);
		}
		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	controller.move(window, &camera, deltaTime);

}
void postProcess(ew::Shader& shader, wm::FrameBuffer& buffer, wm::FrameBuffer& bufferLight)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferLight.colorBuffer[0]);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, bufferLight.colorBuffer[0]);

	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, bufferLight.colorBuffer[1]);

	shader.use();
	shader.setInt("_coords", 1);
	shader.setInt("_Normals", 2);
	shader.setInt("_Albito", 0);
	shader.setInt("_Volume", 3);

	//these need to be looped through? - mov to after the quad
	int max = SUZAN_X * SUZAN_Y;
	
	for (int i = 0; i < max; i++)
	{
		shader.setVec3("_lights[" + std::to_string(i) + "].pos", lights[i].pos);
		shader.setVec3("_lights[" + std::to_string(i) + "].color", lights[i].color);
		shader.setFloat("_lights[" + std::to_string(i) + "].radius", lights[i].radius);
	}

	shader.setVec3("_EyePos", camera.position);

	//material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);


	glBindVertexArray(fullscreenQuad.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
void RenderVolume(wm::FrameBuffer& lightBuffer, ew::Shader shader, wm::FrameBuffer& gBuffer, ew::Mesh& sphere)
{

	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer.fbo);
	
	//create a gfx pass
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE); //Additive blending
	glCullFace(GL_FRONT); //Front face culling - we want to render back faces so that the light volumes don't disappear when we enter them.
	glDepthMask(GL_FALSE); //Disable writing to depth buffer
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffer[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffer[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffer[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffer[3]);

	shader.use();

	shader.setInt("_albito", 0);
	shader.setInt("_normals", 2);
	shader.setInt("_positions", 1);
	shader.setInt("_MaterialTex", 3);

	lightTrans.scale = glm::vec3(5);
	shader.setMat4("_Model", lightTrans.modelMatrix());
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());

	shader.setVec3("_EyePos", camera.position);

	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	//change from testing
	shader.setFloat("metalic", material.Ka);
	shader.setFloat("roughness", material.Kd);

	for (int i = 0; i < SUZAN_X * SUZAN_Y; i++)
	{
		shader.setVec3("_lights.pos", lights[i].pos);
		shader.setVec3("_lights.color", lights[i].color);
		shader.setFloat("_lights.radius",lights[i].radius);

		shader.setMat4("_Model", glm::translate(lights[i].pos));//glm::translate(lights[i].pos)
		//shader.setMat4("_Model", glm::scale(glm::vec3(5)));
		sphere.draw();
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}
void renderSphere(ew::Mesh& sphere, ew::Shader& shader, wm::FrameBuffer &buffer)
{
	glEnable(GL_DEPTH_TEST);
	glBlitNamedFramebuffer(framebuffer.fbo, 0, 0, 0,
		screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	
	
	shader.use();
	
	//camera uniforms
	shader.setMat4("_VeiwProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setVec3("_EyePos", camera.position);

	//model uniforms
	//modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("_Model", modelTrans.modelMatrix());

	//draw the spheres
	for (int i = 0; i < SUZAN_X * SUZAN_Y; i++)
	{
		shader.setMat4("_Model", glm::translate(lights[i].pos));
		shader.setVec3("_Color", lights[i].color);
		sphere.draw();

	}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	ew::Shader _default = ew::Shader("assets/default.vert", "assets/default.frag"); // geo
	ew::Shader geoShader = ew::Shader("assets/geoShader.vert", "assets/geoShader.frag");	// lighting eq.
	ew::Shader renderVolume = ew::Shader("assets/volume.vert", "assets/volume.frag");	// volume.S
	ew::Shader debugLight = ew::Shader("assets/DebugLight.vert", "assets/DebugLight.frag");
	ew::Shader PBR = ew::Shader("assets/PBR.vert", "assets/PBR.frag");
	ew::Model model = ew::Model("assets/Suzanne.fbx");

	ew::Mesh light = ew::createSphere(0.5f, 4);
	ew::Mesh lightVolumeMesh = ew::createSphere(lights[0].radius, 100);
	
	//init camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//texture
	brickTexture = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_Color.jpg");
	normalMapping = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_NormalDX.jpg");
	//brickTexture = ew::loadTexture("assets/brick_color.jpg");
	
	framebuffer = wm::createHDR_FramBuffer(screenWidth, screenHeight);
	lightBuffer = wm::createHDR_FramBuffer(screenWidth, screenHeight);

	testBuffer = wm::createHDR_FramBuffer(screenWidth, screenHeight);
	//inint full screen quad
	glGenVertexArrays(1, &fullscreenQuad.vao);
	glGenBuffers(1, &fullscreenQuad.vbo);

	glBindVertexArray(fullscreenQuad.vao);
	glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuad.vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //text coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

	glBindVertexArray(0);

	// instancing

	// calculagte all suzannes
	// all lights

	
	for (int i = 0; i < SUZAN_X; i++)
	{
		for (int j = 0; j < SUZAN_Y; j++)
		{
			calcualteLightRange(2.0f * i, 2.0f * j, i * SUZAN_X + j);
		}

	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderMonekey(_default, model, light, window);

		RenderVolume(testBuffer, PBR, framebuffer, lightVolumeMesh);
		
		//moving this function ends up breaking it

		postProcess(geoShader, framebuffer, testBuffer);
		
		// redner lights
		renderSphere(light, debugLight, lightBuffer);
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

	ImGui::Image((ImTextureID)(intptr_t)framebuffer.colorBuffer[0], ImVec2(400, 300));
	ImGui::Image((ImTextureID)(intptr_t)framebuffer.colorBuffer[1], ImVec2(400, 300));
	ImGui::Image((ImTextureID)(intptr_t)framebuffer.colorBuffer[2], ImVec2(400, 300));
	ImGui::Image((ImTextureID)(intptr_t)framebuffer.colorBuffer[3], ImVec2(400, 300));

	ImGui::Image((ImTextureID)(intptr_t)testBuffer.colorBuffer[0], ImVec2(400, 300));

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

