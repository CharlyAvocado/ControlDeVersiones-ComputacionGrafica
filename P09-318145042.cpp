/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
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

//variables para animación
//letrero
bool enEspera;
bool moviendoHaciaAbajo;
float tiempoTotal;
float tiempoEspera;
float letreroPosY;
float anguloRotacion;
float duracionFase;
float alturaMaxima;
float alturaMinima;
float velocidadRotacion;
float velocidadMovimiento;

//falcon
float movFalconX;
float movFalconZ;
float rotacion;
float movOffset;
float rotHeli;
float rotHeliOffset;
bool avanza;
bool go_carcel;
bool carcel_parada;
bool parada_arresto;
bool arresto_go;

//Dado
float movDado;
float randRotX; 
float randRotY;
float randRotZ;
float movOffsetDado; 
bool DadoRot; 
bool dadoEnSuelo;      

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Postes_M;
Model Letrero_M;
Model Monopoly_M;
Model Falcon_M;
Model HeliceI_M;
Model HeliceD_M;
Model D10_M;

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
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
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
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


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

	
	Postes_M = Model();
	Postes_M.LoadModel("Models/postesv2.obj");
	Letrero_M = Model();
	Letrero_M.LoadModel("Models/letrerov3.obj");
	Monopoly_M = Model();
	Monopoly_M.LoadModel("Models/monopolyv4.obj");
	Falcon_M = Model();
	Falcon_M.LoadModel("Models/falconv2.obj");
	HeliceI_M = Model();
	HeliceI_M.LoadModel("Models/HeliceIzq.obj");
	HeliceD_M = Model();
	HeliceD_M.LoadModel("Models/HeliceDer.obj");
	D10_M = Model();
	D10_M.LoadModel("Models/d10r.obj");

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
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		100.0f, 10.0f, 95.0f,
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

	//luz fija
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.5f, 1.0f,
		5.0f, 100.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.001f, 0.0002f,
		50.0f);
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	///////////////////////////////////////////////////////////////////////////////////////
	//Letrero
	enEspera = false;  // Control para saber si está en espera
	moviendoHaciaAbajo = true;  // Dirección inicial
	letreroPosY = 0.0f;  // Posición inicial del letrero
	tiempoEspera = 0.0f;  // Tiempo de espera
	anguloRotacion = 0.0f;  // Ángulo de rotación del letrero
	duracionFase = 2.0f;  // Duración de cada etapa (2 segundos)
	alturaMaxima = 2.0f;  // Posición Y más alta
	alturaMinima = -24.0f;  // Posición Y más baja
	velocidadRotacion = 180.0f;  // Grados por segundo
	velocidadMovimiento = 0.35f;  // Velocidad de desplazamiento vertical

	//////////////////////////////////////////////////////////////////////////////////////
	//Falcon
	movFalconX = 0.0f; //Movimiento en X del modelo
	movOffset = 0.5f; //Velocidad del desplazamiento
	rotHeli = 0.0f; //rotación de las hélices
	rotHeliOffset = 20.0f; //Velocidad de la rotación de las helices
	go_carcel = true; //Bandera de la casilla GO a Carcel
	carcel_parada = false; //Bandera de la casilla Carcel a Parada Libre
	parada_arresto = false;//Bandera de la casilla Parada Libre a Vaya a la carcel
	arresto_go = false; //Bandera de la casilla Vaya a la carcel a GO
	rotacion=0.0f; //Variable para la rotación en cada esquina
	movFalconZ = 0.0f;//Variable para el desplazamiento en Z del modelo
	////////////////////////////////////////////////////////////////////////////////////////
	//Dado
	movDado = 0.0f;
	randRotX = 0.0f; //Variable para guardar la rotación en X
	randRotY = 0.0f;//Variable para guardar la rotación en Y
	randRotZ = 0.0f;//Variable para guardar la rotación en Z
	movOffsetDado = 0.5f; //Velocidad del dado
	DadoRot = true; //Bandera para la rotacion del dado
	dadoEnSuelo = false;      // Bandera para detectar cuando el dado toca el suelo

	// Arreglo con las rotaciones posibles (X, Y, Z) para cada cara del decaedro
	struct Rotation {
		float x, y, z;
	};

	Rotation orientacionesDecaedro[10] = {
		{-150.0f, 0.0f, 45.0f},   // Orientación para la cara 1
		{-50.0f, 0.0f, 45.0f},  // Orientación para la cara 2
		{-130.0f, 0.0f, -15.0f}, // Orientación para la cara 3
		{-45.0f, 0.0f, 0.0f}, // Orientación para la cara 4
		{180.0f, 0.0f, -50.0f}, // Orientación para la cara 5
		{0.0f, 0.0f, -50.0f}, // Orientación para la cara 6
		{130.0f, 0.0f, -15.0f}, // Orientación para la cara 7
		{50.0f, 0.0f, -15.0f}, // Orientación para la cara 8
		{140.0f, 0.0f, 40.0f},  // Orientación para la cara 9
		{35.0f, 0.0f, 40.0f}  // Orientación para la cara 10
	};
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
	
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		////////////////////////////////////////////////////////////////////////////////////
		//Dado
		if (mainWindow.getcaida() == 1 && DadoRot == true) {
			movDado -= movOffsetDado * deltaTime;
			// Generar rotaciones aleatorias acotadas
			int indiceAleatorio = rand() % 10;
			Rotation rotSelect = orientacionesDecaedro[indiceAleatorio];

			// Asignar la rotación predefinida
			randRotX = rotSelect.x;
			randRotY = rotSelect.y;
			randRotZ = rotSelect.z;
			if (movDado < -59.5f) {
				DadoRot = false;    // Deja de rotar
				dadoEnSuelo = true;
	
			}
		}
		if (mainWindow.getFw() == true && dadoEnSuelo) {
			// Reiniciar las variables para una nueva tirada
			movDado = 0.0f;  // Reiniciar la posición del dado
			DadoRot = true;  // Habilitar nuevamente la rotación
			dadoEnSuelo = false;  // El dado ya no está en el suelo			
		}
		////////////////////////////////////////////////////////////////////////////////////
		//Letrero
		if (mainWindow.getArranque() == true) {
			if (enEspera==false) {
				if (moviendoHaciaAbajo) {
					// Mover hacia abajo
					if (letreroPosY > alturaMinima) {
						letreroPosY -= velocidadMovimiento * deltaTime;
					}
					else {
						//Cuando llega a la posición más baja
						moviendoHaciaAbajo = false;
						enEspera = true;
						tiempoEspera = now;
					}
				}
				else {
					// Mover hacia arriba
					if (letreroPosY < alturaMaxima) {
						letreroPosY += velocidadMovimiento * deltaTime;
					}
					else {
						// Cuando llega a la posición más alta
						moviendoHaciaAbajo = true;
						enEspera = true;
						tiempoEspera = now;
					}
				}
			}
			else {
				// En espera
				if (now - tiempoEspera > duracionFase) {
					enEspera = false;
				}
			}

			// Rotación del letrero cuando está en la posición superior o inferior
			if (letreroPosY > alturaMaxima || letreroPosY < alturaMinima) {
				anguloRotacion += velocidadRotacion+3.0* deltaTime;
				if (anguloRotacion >= 360.0f) {
					anguloRotacion -= 360.0f;  // Mantener el ángulo entre 0 y 360
				}
			}
		}

	
		////////////////////////////////////////////////////////////////////////////////////
		// Movimiento Falcon
		rotHeli += rotHeliOffset * deltaTime;
		if (mainWindow.getArranque() == true) {
			
			if (go_carcel == true) {
				if (movFalconX > -195.0f) {
					movFalconX -= movOffset * deltaTime;  // Avanza de GO a carcel
				}
				else {
					go_carcel = false;
					carcel_parada = true;
					rotacion += 270.0f;
				}
			}
			else if (carcel_parada == true) {
				if (movFalconZ > -195.0f) {
					movFalconZ -= movOffset * deltaTime;  // Avanza de carcel a parada libre
				}
				else {
					carcel_parada = false;
					parada_arresto = true;
					rotacion += 270.0f;
				}
			}
			else if (parada_arresto == true) {
				if (movFalconX < -3.0f) {
					movFalconX += movOffset * deltaTime;  // Avanza de parada libre a vaya a la carcel
				}
				else{
					parada_arresto = false;
					arresto_go = true;
					rotacion += 270.0f;
				}
			}
			else if (arresto_go == true) {
				if (movFalconZ < 3.0f) {

					movFalconZ += movOffset * deltaTime;  //Avanza de vaya a la carcel a GO
				}
				else{
					arresto_go = false;
					go_carcel = false;
					rotacion += 270.0f;
				}
			}
			if (mainWindow.getArranque2()==true) {
				// Reiniciar el estado de las banderas y variables cuando se presiona 'Q'
				go_carcel = true;
				carcel_parada = false;
				parada_arresto = false;
				arresto_go = false;
				movFalconX = 0.0f;  // Reset de la posición
				movFalconZ = 0.0f;
				rotacion = 0.0f;   // Reset de la rotación
			}
		}
	
		
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
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, movDado + 65.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		if (mainWindow.getcaida() == true) {
			// Aplicar rotaciones aleatorias
			model = glm::rotate(model, glm::radians(randRotX), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(randRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(randRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		D10_M.RenderModel();

		//Tablero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Monopoly_M.RenderModel();

		//Falcon
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(100.0f, 10.0f, 95.0f));
		model = glm::translate(model, glm::vec3(movFalconX, 0.0f, movFalconZ));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Falcon_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(12.4f, 11.25f, -5.1f));
		model = glm::rotate(model,rotHeli * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceD_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-10.0f, 11.25f, -5.1f));
		model = glm::rotate(model, rotHeli * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceI_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -1.0f, 90.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Postes_M.RenderModel();
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Postes_M.RenderModel();
		// Renderizar el letrero
		model = modelaux;
		model = glm::translate(model, glm::vec3(-5.0f, letreroPosY + 28.0f, 0.0f));
		if (letreroPosY > alturaMaxima || letreroPosY < alturaMinima) {
			model = glm::rotate(model, glm::radians(anguloRotacion), glm::vec3(-1.0f, 0.0f, 0.0f));  // Rotar sobre el eje X
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero_M.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
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
