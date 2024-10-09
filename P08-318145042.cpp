/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;


Model Blackhawk_M;

Model Carroceria_M;
Model Cofre_M;
Model LlantaFD_M;
Model LlantaFI_M;
Model LlantaTD_M;
Model LlantaTI_M;
Model Dado3D_M;
Model GuiltySpark_M;
Model Lamp_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight arreglo2puntual[MAX_SPOT_LIGHTS];


SpotLight spotLights[MAX_SPOT_LIGHTS];


static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	

	
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();



	GuiltySpark_M = Model();
	GuiltySpark_M.LoadModel("Models/343 GuiltySpark.obj");
	

	Lamp_M = Model();
	Lamp_M.LoadModel("Models/lamp.obj");


	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	
	Carroceria_M = Model();
	Carroceria_M.LoadModel("Models/Carroceria.obj");

	Cofre_M = Model();
	Cofre_M.LoadModel("Models/Cofre.obj");

	LlantaFD_M = Model();
	LlantaFD_M.LoadModel("Models/LlantaFD.obj");

	LlantaFI_M = Model();
	LlantaFI_M.LoadModel("Models/LlantaFI.obj");


	LlantaTD_M = Model();
	LlantaTD_M.LoadModel("Models/LlantaTD.obj");


	LlantaTI_M = Model();
	LlantaTI_M.LoadModel("Models/LlantaTI.obj");

	Dado3D_M = Model();
	Dado3D_M.LoadModel("Models/dado_animales3ds.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	

	//Declaración de primer luz puntual
	//Luz Guilty Spark
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.5f, 1.0f,
		0.0f, 1.5f, -40.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//Luz lámpara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.75f, -20.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(
		1.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		10.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(
		1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		15.0f);
	spotLightCount++;

	spotLights[3] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		15.0f);
	spotLightCount++;



	
	



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//POINTLIGHTS
		unsigned int activePointLightCount = 0;
		// Reset del contador para cada frame
		activePointLightCount = 0;
		// Control de lámpara (tecla 7)
		if (mainWindow.getLamp() == true)
		{
			// Mantener la primera luz encendida y contarla
			shaderList[0].SetPointLights(&pointLights[0], 1);
			activePointLightCount++;
		}
		else if (mainWindow.getLamp() == false)
		{
			// Si está apagada, no se envía nada al shader
			shaderList[0].SetPointLights(pointLights, activePointLightCount);
		}

		// Control de Guilty Spark (tecla 8)
		if (mainWindow.getLamp2() == true)
		{
			shaderList[0].SetPointLights(&pointLights[1], activePointLightCount + 1);
		}
		else if (mainWindow.getLamp2() == false)

		{
			shaderList[0].SetPointLights(pointLights, activePointLightCount);
		}
		if (mainWindow.getLamp2() == true && mainWindow.getLamp() == true)
		{
			shaderList[0].SetPointLights(pointLights, activePointLightCount=2);
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//SPOTLIGHTS
		// Inicializar el contador de SpotLights activas
			unsigned int activeSpotLightCount = 0;

		// Arreglo temporal de luces activas para enviarlas al shader
		SpotLight activeSpotLights[4];  // Máximo 4 luces activas

		// SpotLights 0 y 1 están siempre activas porque corresponden
		//a la linterna y a la luz del cofre
		activeSpotLights[activeSpotLightCount] = spotLights[0];  
		activeSpotLightCount++;
		activeSpotLights[activeSpotLightCount] = spotLights[1];
		activeSpotLightCount++;

		// Control de la luz delantera
		if (mainWindow.getFw()) {
			activeSpotLights[activeSpotLightCount] = spotLights[2];  
			activeSpotLightCount++;
		}

		// Control de la luz trasera
		if (mainWindow.getBw()) {
			activeSpotLights[activeSpotLightCount] = spotLights[3]; 
			activeSpotLightCount++;
		}

		// Enviar todas las luces activas al shader
		shaderList[0].SetSpotLights(activeSpotLights, activeSpotLightCount);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Guilty Spark
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, -40.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GuiltySpark_M.RenderModel();

		//Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lamp_M.RenderModel();

		//dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-18.0f, 1.2f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(1.25f, 1.25f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado3D_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, 1.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado3D_M.RenderModel();
		//Instancia del coche 

		model = glm::mat4(1.0);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -25.0f+ mainWindow.getTraslacion()));
		modelaux = model;
		//Luz delantera
		glm::vec4 faroFLocal = glm::vec4(0.0f, 0.5f, 5.0f, 1.0f); // Coordenadas del faro
		glm::vec3 faroFWorld = glm::vec3(model * glm::vec4(faroFLocal));// Calcula la posición del faro  del coche
		spotLights[2].SetPos(faroFWorld);// Establece la posición del faro izquierdo
		shaderList[0].SetSpotLights(spotLights, spotLightCount); // Actualiza las luces en el shader
		
		//Luz trasera
		glm::vec4 faroTLocal = glm::vec4(1.0f, 0.5f, -3.5f, 1.0f); // Coordenadas del faro
		glm::vec3 faroTWorld = glm::vec3(model * glm::vec4(faroTLocal));// Calcula la posición del faro  del coche
		spotLights[3].SetPos(faroTWorld);// Establece la posición del faro izquierdo
		shaderList[0].SetSpotLights(spotLights, spotLightCount); // Actualiza las luces en el shader



		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec4 faroLocal = glm::vec4(0.0f, 2.0f, 4.15f, 1.0f); // Coordenadas del faro
		glm::vec3 faroWorld = glm::vec3(model * glm::vec4(faroLocal));// Calcula la posición del faro  del coche
		spotLights[1].SetPos(faroWorld);// Establece la posición del faro izquierdo
		shaderList[0].SetSpotLights(spotLights, spotLightCount); // Actualiza las luces en el shader

		model = modelaux;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carroceria_M.RenderModel();

		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.56f, 1.09f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();
		//Llanta frontal derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.9f, 0.59f, 2.025f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFD_M.RenderModel();

		//Llanta frontal izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.6f, 2.025f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaFI_M.RenderModel();


		//Llanta trasera derecha
		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.9f, 0.59f, -1.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTD_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.6f, -1.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(1.0f, 0.0f, 0.0f));	
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTI_M.RenderModel();

		//Blackhawk

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 12.0f, 6.5));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 1.0f, -15.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
