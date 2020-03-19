#include "pch.h"
#include "Test.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include "entityx\entityx.h"

#include "irrKlang.h"
#include "filesystem.h"

//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

struct DirLight {
	glm::vec3 direction;
	glm::vec3 color;
};

struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float cutOff;
	float outerCutOff;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void renderFloor();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
bool alt = false;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int Test::work()
{
	// start the sound engine with default parameters
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engine

	// play some sound stream, looped
	engine->play2D(FileSystem::getResPath("sounds/ophelia.mp3").c_str(), true);

	// --------------------------------------------------
	// --------------------------------------------------

	entityx::Entity e;
	std::cout << "entity valid: " << e.valid() << std::endl;

	// --------------------------------------------------
	// --------------------------------------------------


	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
	#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Zad4 - PBR", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// IMGUI
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsClassic();
	//------

	// build and compile shaders
	// -------------------------
	Shader           shader(FileSystem::getResPath(R"(shaders\vertex_shader.glsl)").c_str(), FileSystem::getResPath(R"(shaders\fragment_shader.glsl)").c_str());
	Shader   lightingShader(FileSystem::getResPath(R"(shaders\lighting_vs.glsl)").c_str(),   FileSystem::getResPath(R"(shaders\lighting_fs.glsl)").c_str());
	Shader instancingShader(FileSystem::getResPath(R"(shaders\instancing_vs.glsl)").c_str(), FileSystem::getResPath(R"(shaders\lighting_fs.glsl)").c_str());
	
	// load models
	// -----------
	Model sphere(instancingShader, FileSystem::getResPath("models/sphere/sphere.obj").c_str());

	Model pointL(shader, FileSystem::getResPath("models/sphere/sphere.obj").c_str());
	Model spot1L(shader, FileSystem::getResPath("models/cone/cone.obj").c_str());
	Model spot2L(shader, FileSystem::getResPath("models/cone/cone.obj").c_str());
	

	// load PBR material textures
	// --------------------------
	unsigned int albedo = loadTexture(FileSystem::getResPath("textures/rusted_iron/albedo.png").c_str());
	unsigned int normal = loadTexture(FileSystem::getResPath("textures/rusted_iron/normal.png").c_str());
	unsigned int metallic = loadTexture(FileSystem::getResPath("textures/rusted_iron/metallic.png").c_str());
	unsigned int roughness = loadTexture(FileSystem::getResPath("textures/rusted_iron/roughness.png").c_str());
	unsigned int ao = loadTexture(FileSystem::getResPath("textures/rusted_iron/ao.png").c_str());

	unsigned int floor_a = loadTexture(FileSystem::getResPath("textures/floor/albedo.png").c_str());
	unsigned int floor_n = loadTexture(FileSystem::getResPath("textures/floor/normal-ogl.png").c_str());
	unsigned int floor_m = loadTexture(FileSystem::getResPath("textures/floor/metallic.png").c_str());
	unsigned int floor_r = loadTexture(FileSystem::getResPath("textures/floor/roughness.png").c_str());
	unsigned int floor_ao = loadTexture(FileSystem::getResPath("textures/floor/ao.png").c_str());

	// lights
	// ------
	bool p = true;
	PointLight pl;
	pl.position = glm::vec3(0.0f, 4.0f, 0.0f);
	pl.color = glm::vec3(1.0f, 1.0f, 1.0f);

	bool d = true;
	DirLight dl;
	dl.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	dl.color = glm::vec3(0.0f, 1.0f, 0.0f);

	bool s_1 = true;
	SpotLight sl_1;
	sl_1.position = glm::vec3(-5.0f, 3.0f, -8.0f);
	sl_1.direction = glm::vec3(0.0f, 0.0f, 1.0f);
	sl_1.color = glm::vec3(1.0f, 0.0f, 0.0f);
	sl_1.cutOff = glm::cos(glm::radians(10.0f));
	sl_1.outerCutOff = glm::cos(glm::radians(14.0f));

	bool s_2 = true;
	SpotLight sl_2;
	sl_2.position = glm::vec3(-5.0f, 3.0f, 8.0f);
	sl_2.direction = glm::vec3(0.0f, 0.0f, -1.0f);
	sl_2.color = glm::vec3(0.0f, 0.0f, 1.0f);
	sl_2.cutOff = glm::cos(glm::radians(10.0f));
	sl_2.outerCutOff = glm::cos(glm::radians(14.0f));

	bool lightSpheres = true;

	// instancing positions for spheres
	// --------------------------------
	int nrRows = 20;
	int nrColumns = 20;
	float spacing = 2.5;
	glm::mat4* modelMatrices = new glm::mat4[nrRows * nrColumns];

	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < nrRows; ++row)
	{
		for (int col = 0; col < nrColumns; ++col)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				1.0f,
				(float)(row - (nrRows / 2)) * -spacing
			));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

			modelMatrices[row * nrColumns + col] = model;
		}
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, nrRows * nrColumns * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// ----------------------------------------------------------------------------
	for (unsigned int i = 0; i < sphere.meshes.size(); i++)
	{
		unsigned int VAO = sphere.meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	// const projection matrix
	// -----------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// render loop
	// -----------
	int frameCount = 0;
	double previousTime = glfwGetTime();
	float fps = 0;
	bool open = true;

	float m_FrameTimeGraph[100];
	unsigned int values_offset = 0;

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameCount++;
		if (currentFrame - previousTime >= 1.0)
		{
			fps = frameCount;

			frameCount = 0;
			previousTime = currentFrame;
		}

		// input
		// -----
		processInput(window);
		glm::mat4 view = camera.GetViewMatrix();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Options");
		ImGui::Text("FPS: %f", fps);
		ImGui::Checkbox("Point Light", &p);
		ImGui::ColorEdit3("Point Light", pl.color.data.data);
		ImGui::Checkbox("Dir Light", &d);
		ImGui::ColorEdit3("Dir Light", dl.color.data.data);
		ImGui::SliderFloat3("Direction", dl.direction.data.data, -1.0f, 1.0f);
		ImGui::Checkbox("Spot 1 Light", &s_1);
		ImGui::ColorEdit3("Spot 1 Light", sl_1.color.data.data);
		ImGui::Checkbox("Spot 2 Light", &s_2);
		ImGui::ColorEdit3("Spot 2 Light", sl_2.color.data.data);
		ImGui::End();

		ImGui::Begin("Performance");
		m_FrameTimeGraph[values_offset] = 1000.0f / io.Framerate;
		values_offset = (values_offset + 1) % 100;
		ImGui::PlotLines("##Frametime", m_FrameTimeGraph, 100, values_offset, "Frametime (ms)", 0.0f, 66.6f, ImVec2(0, 100));
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		//ImGui::ShowMetricsWindow();

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setMat4("pv", projection * view);

		// render objects by instancing
		// --------------------------
		instancingShader.use();
		instancingShader.setMat4("pv", projection * view);
		instancingShader.setVec3("camPos", camera.Position);
		instancingShader.setInt("albedoMap", 0);
		instancingShader.setInt("normalMap", 1);
		instancingShader.setInt("metallicMap", 2);
		instancingShader.setInt("roughnessMap", 3);
		instancingShader.setInt("aoMap", 4);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);

		//rock.DrawInstanced(nrRows * nrColumns);
		for (unsigned int i = 0; i < sphere.meshes.size(); i++)
		{
			glBindVertexArray(sphere.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, sphere.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, nrRows * nrColumns);
			glBindVertexArray(0);
		}

		// render floor
		// ------------
		lightingShader.use();
		lightingShader.setMat4("pv", projection * view);
		lightingShader.setVec3("camPos", camera.Position);
		lightingShader.setInt("albedoMap", 0);
		lightingShader.setInt("normalMap", 1);
		lightingShader.setInt("metallicMap", 2);
		lightingShader.setInt("roughnessMap", 3);
		lightingShader.setInt("aoMap", 4);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floor_a);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, floor_n);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floor_m);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, floor_r);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, floor_ao);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(4.0f));
		lightingShader.setMat4("model", model);
		renderFloor();


		glm::vec3
			newPos = pl.position + glm::vec3(sin(glfwGetTime() * 2.0) * 10.0f, 0.0f, cos(glfwGetTime() * 2.0f) * 10.0f);
		// newPos = lightPositions[i];

		instancingShader.use();
		// point
		instancingShader.setInt("p", p);
		if (p)
		{
			instancingShader.setVec3("pl.position", newPos);
			instancingShader.setVec3("pl.color", pl.color);
		}
		// directional
		instancingShader.setInt("d", d);
		if (d)
		{
			instancingShader.setVec3("dl.direction", dl.direction);
			instancingShader.setVec3("dl.color", dl.color);
		}
		// spotlight 1
		instancingShader.setInt("s_1", s_1);
		if (s_1)
		{
			instancingShader.setVec3("sl_1.position", sl_1.position);
			instancingShader.setVec3("sl_1.direction", sl_1.direction);
			instancingShader.setVec3("sl_1.color", sl_1.color);
			instancingShader.setFloat("sl_1.cutOff", sl_1.cutOff);
			instancingShader.setFloat("sl_1.outerCutOff", sl_1.outerCutOff);
		}
		// spotlight 2
		instancingShader.setInt("s_2", s_2);
		if (s_2)
		{
			instancingShader.setVec3("sl_2.position", sl_2.position);
			instancingShader.setVec3("sl_2.direction", sl_2.direction);
			instancingShader.setVec3("sl_2.color", sl_2.color);
			instancingShader.setFloat("sl_2.cutOff", sl_2.cutOff);
			instancingShader.setFloat("sl_2.outerCutOff", sl_2.outerCutOff);
		}

		lightingShader.use();
		// point
		lightingShader.setInt("p", p);
		if (p)
		{
			lightingShader.setVec3("pl.position", newPos);
			lightingShader.setVec3("pl.color", pl.color);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			lightingShader.setMat4("model", model);

			shader.use();
			shader.setMat4("model", glm::scale(model, glm::vec3(0.1f)));
			shader.setVec3("color", pl.color);
			pointL.Draw();
		}

		lightingShader.use();
		// directional
		lightingShader.setInt("d", d);
		if (d)
		{
			lightingShader.setVec3("dl.direction", dl.direction);
			lightingShader.setVec3("dl.color", dl.color);
		}

		// spotlight 1
		lightingShader.setInt("s_1", s_1);
		if (s_1)
		{
			lightingShader.setVec3("sl_1.position", sl_1.position);
			lightingShader.setVec3("sl_1.direction", sl_1.direction);
			lightingShader.setVec3("sl_1.color", sl_1.color);
			lightingShader.setFloat("sl_1.cutOff", sl_1.cutOff);
			lightingShader.setFloat("sl_1.outerCutOff", sl_1.outerCutOff);

			model = glm::mat4(1.0f);
			model = glm::translate(model, sl_1.position);
			model = glm::scale(model, glm::vec3(0.5f));
			lightingShader.setMat4("model", model);

			shader.use();
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			shader.setMat4("model", model);
			shader.setVec3("color", sl_1.color);
			spot1L.Draw();
		}

		lightingShader.use();
		// spotlight 2
		lightingShader.setInt("s_2", s_2);
		if (s_2)
		{
			lightingShader.setVec3("sl_2.position", sl_2.position);
			lightingShader.setVec3("sl_2.direction", sl_2.direction);
			lightingShader.setVec3("sl_2.color", sl_2.color);
			lightingShader.setFloat("sl_2.cutOff", sl_2.cutOff);
			lightingShader.setFloat("sl_2.outerCutOff", sl_2.outerCutOff);

			model = glm::mat4(1.0f);
			model = glm::translate(model, sl_2.position);
			model = glm::scale(model, glm::vec3(0.5f));
			lightingShader.setMat4("model", model);

			shader.use();
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			shader.setMat4("model", model);
			shader.setVec3("color", sl_2.color);
			spot2L.Draw();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	// irrKlang: 
	engine->drop(); // delete engine

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		if (alt)
		{
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetCursorPosCallback(window, nullptr);
			glfwSetScrollCallback(window, nullptr);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
		alt = false;
	else
		alt = true;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// renders (and builds at first invocation) a floor
// -------------------------------------------------
unsigned int floorVAO = 0;
void renderFloor()
{
	if (floorVAO == 0)
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float floorVertices[] = {
			// positions             // texcoords	 // normals
			 10.0f, 0.0f,  10.0f,   10.0f,  0.0f,	 0.0f, 1.0f, 0.0f,
			-10.0f, 0.0f,  10.0f,   0.0f,   0.0f,	 0.0f, 1.0f, 0.0f,
			-10.0f, 0.0f, -10.0f,   0.0f,  10.0f,	 0.0f, 1.0f, 0.0f,

			 10.0f, 0.0f,  10.0f,   10.0f,  0.0f,	 0.0f, 1.0f, 0.0f,
			-10.0f, 0.0f, -10.0f,   0.0f,  10.0f,	 0.0f, 1.0f, 0.0f,
			 10.0f, 0.0f, -10.0f,   10.0f, 10.0f,	 0.0f, 1.0f, 0.0f
		};

		// floor VAO
		unsigned int floorVBO;

		glGenVertexArrays(1, &floorVAO);
		glGenBuffers(1, &floorVBO);

		glBindVertexArray(floorVAO);

		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
