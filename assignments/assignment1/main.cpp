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
#include <wm/framebuffer.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

//UI for post processing 
static int effect_index = 0;
static std::vector<std::string> post_processing_effects = {
	"None",
	"Grayscale",
	"Kernel Blur",
	"Inverse",
	"Chromatic Aberration",
	"Edge Detection",
	"CRT",
};

//camera
ew::Camera camera;
ew::CameraController controller;

//transfrom
ew::Transform modelTrans;

//texutres
GLuint brickTexture;
GLuint normalMapping;


struct FullscreenQuad
{
	GLuint vao;
	GLuint vbo;

}fullscreenQuad;


struct Material 
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

float exposure = 5.0f;

struct Framebuffer
{
	GLuint fbo;
	GLuint color0;
	GLuint color1;
	GLuint color2;
	GLuint depth;

};

wm::FrameBuffer libBuffer;
wm::FrameBuffer HDRbuffer;

wm::FrameBuffer testBuffer;

Framebuffer buffer;

static float quad_vertices[] = {
	// pos (x, y) texcoord (u, v)
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f,
};

void renderMonekey(ew::Shader& shader, ew::Model& model, GLFWwindow* window)
{
	//bind new buffer? - this is causeing the crash
	glBindFramebuffer(GL_FRAMEBUFFER, libBuffer.fbo);

	//pipeline definition
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);

	//create a gfx pass
	//glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	modelTrans.rotation = glm::rotate(modelTrans.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("_Model", modelTrans.modelMatrix());

	//material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	//textures
	shader.setInt("_MainTex", 0);
	shader.setInt("_NormalMap", 1);

	model.draw();

	controller.move(window, &camera, deltaTime);

	//unbind buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void pingpongRender(ew::Shader& shader, wm::FrameBuffer buffers[2])
{
	glBindVertexArray(fullscreenQuad.vao);
	for (int i = 0; i <= 5; i++)
	{
		int drawIndex= 0;
		int sampleIndex = 0;

		//is even so draw odd
		if (i % 2 == 0) 
		{
			drawIndex = 1;
			sampleIndex = 0;
			
		}

		glBindFramebuffer(GL_FRAMEBUFFER, buffers[drawIndex].fbo);


		//texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffers[sampleIndex].colorBuffer[0]);

		shader.use();

		shader.setInt("tex", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		

	}

	//unbind buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawPostEffect(wm::FrameBuffer buffer, std::vector<ew::Shader> postList)
{
	glBindVertexArray(fullscreenQuad.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);

	switch (effect_index)
	{
	case 1:
		postList[1].use();
		postList[1].setInt("tex", 0);
		
		break;
	case 2:
		postList[2].use();
		postList[2].setInt("tex", 0);
		break;
	case 3:
		postList[3].use();
		postList[3].setInt("tex", 0);
		break;
	case 4:
		postList[4].use();
		postList[4].setInt("tex", 0);
		break;
	case 5:
		postList[5].use();
		postList[5].setInt("tex", 0);
		break;
	case 6:
		postList[6].use();
		postList[6].setInt("tex", 0);
		postList[6].setInt("depthTex", 1);
		break;
	default:
		postList[0].use();
		postList[0].setInt("tex", 0);
		postList[0].setFloat("exposure", exposure);
		break;
	}
	//shader.use();
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
};

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0.0, 0.0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;

}

int main() {

	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//chache 
	wm::FrameBuffer pingpong[2] = { wm::createFrameBuffer(800, 600, GL_RGB, wm::TEXTURE), wm::createFrameBuffer(800, 600, GL_RGB, wm::TEXTURE) };
	
	
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader full = ew::Shader("assets/blin.vert", "assets/blin.frag");
	ew::Shader inverse = ew::Shader("assets/inverse.vert", "assets/inverse.frag");
	ew::Shader grayScale = ew::Shader("assets/inverse.vert", "assets/grayScale.frag");
	ew::Shader blur = ew::Shader("assets/blur.vert", "assets/blur.frag");
	ew::Shader edge = ew::Shader("assets/Edge.vert", "assets/Edge.frag");
	ew::Shader chrom = ew::Shader("assets/chromatic.vert", "assets/chromatic.frag");
	ew::Shader crt = ew::Shader("assets/Fog.vert", "assets/Fog.frag");
	ew::Shader HDR = ew::Shader("assets/hdr.vert", "assets/hdr.frag");
	ew::Model model = ew::Model("assets/Suzanne.fbx");

	std::vector<ew::Shader> postEffects =
	{
		HDR, grayScale, blur, inverse, chrom, edge, crt
	};
	

	//init camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//inint full screen quad
	glGenVertexArrays(1, &fullscreenQuad.vao);
	glGenBuffers(1, &fullscreenQuad.vbo);

	glBindVertexArray(fullscreenQuad.vao);
	glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuad.vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //text coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (sizeof(float)*2));

	glBindVertexArray(0);
	//glVertexAttribPointer()

	
	//texture
	brickTexture = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_Color.jpg");
	normalMapping = ew::loadTexture("assets/bricks/Bricks075A_1K-JPG_NormalDX.jpg");
	//brickTexture = ew::loadTexture("assets/brick_color.jpg");
	
	//lib buffer
	//libBuffer = wm::createFrameBuffer(800, 600, GL_RGB, wm::TEXTURE);
	//HDRbuffer = wm::createHDR_FramBuffer(800, 600);
	libBuffer = wm::createHDR_FramBuffer(800, 600);

	//buffer code
	glGenFramebuffers(1, &buffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

	glGenTextures(1, &buffer.color0);

	glBindTexture(GL_TEXTURE_2D, buffer.color0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.color0, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("frame buffer is not complete!");
		return 0;

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;


		renderMonekey(shader, model, window);

		pingpong[0].colorBuffer[0] = libBuffer.colorBuffer[1];
		
		//pingpong[1].colorBuffer[0] = libBuffer.colorBuffer[1];
		



		glDisable(GL_DEPTH_TEST);
		pingpongRender(blur, pingpong);


		testBuffer.colorBuffer[0] = pingpong[0].colorBuffer[0];


		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawPostEffect(pingpong[0], postEffects);

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
	ImGui::Image((ImTextureID)(intptr_t)libBuffer.colorBuffer[0], ImVec2(800, 600));
	ImGui::Image((ImTextureID)(intptr_t)libBuffer.colorBuffer[1], ImVec2(800, 600));
	ImGui::Image((ImTextureID)(intptr_t)testBuffer.colorBuffer[0], ImVec2(800, 600));
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
	ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.f);

	if (ImGui::BeginCombo("Effect", post_processing_effects[effect_index].c_str()))
	{
		for (auto n = 0; n < post_processing_effects.size(); ++n)
		{
			auto is_selected = (post_processing_effects[effect_index] == post_processing_effects[n]);
			if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
			{
				effect_index = n;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	
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

