/*
Animaci�n por keyframes
La textura del skybox fue conseguida desde la p�gina https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edit� en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
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

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Voltorb_M;
Model Tablero_M;
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

//funci�n para teclado de keyframes 
void inputKeyframes(bool* keys);

//c�lculo del promedio de las normales para sombreado de Phong
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
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXModelo = 60.0, posYModelo = 2.70, posZModelo = -30.0;
float	moxModelo_X = 0.0f, moxModelo_Y = 0.0f;
float giroModelo = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float moxModelo_X;		//Variable para PosicionX
	float moxModelo_Y;		//Variable para PosicionY
	float moxModelo_XInc;		//Variable para IncrementoX
	float moxModelo_YInc;		//Variable para IncrementoY
	float giroModelo;
	float giroModeloInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
int contadorRalentizador = 0;
bool play = false;
int playIndex = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funciones para el guardado y lectura de keyframes con archivos de texto
void ReadKeyFramesFile() {
	FILE* ReadFile;
	fopen_s(&ReadFile, "ValoresKeyFrameVoltorb.txt", "r"); //Apertura del archivo en modo de solo lectura
	if (ReadFile == NULL) {
		printf("Error: No fue posible abrir el archivo en modo lectura\n");
		return;
	}
	char line[256]; //Buffer para almacenar lineas leidas
	FrameIndex = 0; //Reinicia el indice al comienzo de la lectura
	//Lectura del archivo linea por linea
	while (fgets(line, sizeof(line), ReadFile)) {
		int index;
		float mov_x, mov_y, giro;

		//Lectura de los datos de moxModelo_X
		if (sscanf_s(line, "KeyFrame[%d].moxModelo_X = %f;", &index, &mov_x) == 2) {
			KeyFrame[FrameIndex].moxModelo_X = mov_x;
		}


		//Lectura de los datos de moxModelo_Y
		if (fgets(line, sizeof(line), ReadFile) && sscanf_s(line, "KeyFrame[%d].moxModelo_Y = %f;", &index, &mov_y) == 2) {
			KeyFrame[FrameIndex].moxModelo_Y = mov_y;
		}


		//Lectura de los datos de giroModelo
		if (fgets(line, sizeof(line), ReadFile) && sscanf_s(line, "KeyFrame[%d].giroModelo = %f;", &index, &giro) == 2) {
			KeyFrame[FrameIndex].giroModelo = giro;
			/*printf("-------------------------------------------------------------------------------------------------------------------\n");
			printf("Indice KeyFrame %d leido: moxModelo_X = %.2f, moxModelo_Y = %.2f, giroModelo = %.2f\n", FrameIndex, mov_x, mov_y, giro);
			printf("-------------------------------------------------------------------------------------------------------------------\n");*/
			FrameIndex++; // Incrementa el indice para el proximo frame
		}

	}
	fclose(ReadFile);
	/*printf("--------------------------------------------------------------------------------------------------------------------------- ");
	printf("Se termino la lectura del archivo ValoresKeyFrameVoltorb.txt\n");*/
}

void resetElements(void)
{
	moxModelo_X = KeyFrame[0].moxModelo_X;
	moxModelo_Y = KeyFrame[0].moxModelo_Y;
	giroModelo = KeyFrame[0].giroModelo;
}

void interpolation(void)
{
	KeyFrame[playIndex].moxModelo_XInc = (KeyFrame[playIndex + 1].moxModelo_X - KeyFrame[playIndex].moxModelo_X) / i_max_steps;
	KeyFrame[playIndex].moxModelo_YInc = (KeyFrame[playIndex + 1].moxModelo_Y - KeyFrame[playIndex].moxModelo_Y) / i_max_steps;
	KeyFrame[playIndex].giroModeloInc = (KeyFrame[playIndex + 1].giroModelo - KeyFrame[playIndex].giroModelo) / i_max_steps;

}
void iniciarAnimacion() {
	if (reproduciranimacion < 1) {
		if (!play && (FrameIndex > 1)) {
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
			reproduciranimacion++;
			habilitaranimacion = 0;
			//printf("\n Iniciando animacion en ciclo.\n");
		}
	}
}

void reiniciarAnimacion() {
	if (habilitaranimacion < 1 && reproduciranimacion > 0) {
		//printf("Reiniciando animacion.\n");
		reproduciranimacion = 0;
		habilitaranimacion++;
	}
}
void animate() {
	if (play) {
		if (i_curr_steps >= i_max_steps) { // fin de interpolaci?n entre frames
			playIndex++;
			if (playIndex > FrameIndex - 2) { // fin de toda la animacion
				playIndex = 0;
				play = false;
				reiniciarAnimacion();
				iniciarAnimacion();  // Inicia de nuevo la animacion
			}
			else { // interpolacion al siguiente cuadro
				i_curr_steps = 0;
				interpolation();
			}
		}
		else {
			// Dibujar Animaci?n
			moxModelo_X += KeyFrame[playIndex].moxModelo_XInc;
			moxModelo_Y += KeyFrame[playIndex].moxModelo_YInc;
			giroModelo += KeyFrame[playIndex].giroModeloInc;
			i_curr_steps++;
		}
	}
}

	void Weathley() { //Funcion para ralentizar el programa ya que fue la unica solucion rapida, nombre Weathley en inspiracion al juego Portal 2
					  //En el que Weathley es un nucleo de estupidez creado para ralentizar la inteligencia de una IA
		FILE* WeathleyCore;
		fopen_s(&WeathleyCore, "Weathley.txt", "r"); //Apertura del archivo en modo de solo lectura, el archivo contiene datos basura
		if (WeathleyCore == NULL) {
			printf("Error: No fue posible abrir el archivo en modo lectura\n");
			return;
		}

		char line[256]; //Buffer para almacenar l�neas le�das
		contadorRalentizador = 0; //Reinicia el indice al comienzo de la lectura

		//Lectura del archivo l�nea por l�nea
		while (fgets(line, sizeof(line), WeathleyCore)) {
			int index;
			float mov_x, mov_y, giro;

			//Lectura de los datos de moxModelo_X
			if (sscanf_s(line, "KeyFrame[%d].moxModelo_X = %f;", &index, &mov_x) == 2) {
			}


			//Lectura de los datos de moxModelo_Y
			if (fgets(line, sizeof(line), WeathleyCore) && sscanf_s(line, "KeyFrame[%d].moxModelo_Y = %f;", &index, &mov_y) == 2) {
			}


			//Lectura de los datos de giroModelo
			if (fgets(line, sizeof(line), WeathleyCore) && sscanf_s(line, "KeyFrame[%d].giroModelo = %f;", &index, &giro) == 2) {
				//printf("-------------------------------------------------------------------------------------------------------------------\n");
				//printf("-------------------------------------------------------------------------------------------------------------------\n");
				contadorRalentizador++; // Incrementa el �ndice para el pr�ximo frame
			}

		}

		fclose(WeathleyCore);
		//printf("--------------------------------------------------------------------------------------------------------------------------- ");
	}





///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	ReadKeyFramesFile();
	iniciarAnimacion();
	CreateObjects();
	CreateShaders();
	

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


	Voltorb_M = Model();
	Voltorb_M.LoadModel("Models/Voltorb.obj");
	Tablero_M = Model();
	Tablero_M.LoadModel("Models/TableroNoche.obj");
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
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
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);


	//KEYFRAMES CON TXT

	//Se agregan nuevos frames 
		
		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X\n");
		printf("7.-Presione 3 para mover en X (Hacia adelante)\n");
		printf("8.-Presione 4 para habilitar mover en X\n");
		printf("9.-Presione 5 para mover en Y (Hacia abajo)\n");
		printf("10.-Presione 6 para habilitar mover en Y\n");
		printf("11.-Presione 7 para mover en Y (Hacia arriba)\n");
		printf("12.-Presione 8 para habilitar mover en Y\n");
		printf("13.-Presione 9 para rotar \n");
		printf("14.-Presione G para habilitar rotar \n");
		printf("15.-Presione T para rotar \n");
		printf("16.-Presione Y para habilitar rotar \n");
		printf("17.-Presione R para leer el archivo con los frames guardados\n");

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		animate();

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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		
		Weathley();
		
		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXModelo + moxModelo_X, posYModelo + moxModelo_Y, posZModelo);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, giroModelo * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Voltorb_M.RenderModel();

		model = glm::mat4(1.0);
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tablero_M.RenderModel();

		//Agave �qu� sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav += 0.0;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();


		glDisable(GL_BLEND);
		
		



		glUseProgram(0);
		mainWindow.swapBuffers();

	}

	return 0;
}


void inputKeyframes(bool* keys)
{
//
//	if (keys[GLFW_KEY_SPACE])
//	{
//		if (reproduciranimacion < 1)
//		{
//			if (play == false && (FrameIndex > 1))
//			{
//				resetElements();
//				//First Interpolation				
//				interpolation();
//				play = true;
//				playIndex = 0;
//				i_curr_steps = 0;
//				reproduciranimacion++;
//				printf("\n presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
//				habilitaranimacion = 0;
//
//			}
//			else
//			{
//				play = false;
//
//			}
//		}
//	}
//	if (keys[GLFW_KEY_0])
//	{
//		if (habilitaranimacion < 1 && reproduciranimacion>0)
//		{
//			printf("Ya puedes reproducir de nuevo la animaci�n con la tecla de barra espaciadora'\n");
//			reproduciranimacion = 0;
//			habilitaranimacion++;
//			
//		}
//	}
//
//	if (keys[GLFW_KEY_L])
//	{
//		if (guardoFrame < 1)
//		{
//			saveFrame();
//			printf("moxModelo_X es: %f\n", moxModelo_X);
//			printf("moxModelo_Y es: %f\n", moxModelo_Y);
//			printf("presiona P para habilitar guardar otro frame'\n");
//			guardoFrame++;
//			reinicioFrame = 0;
//		}
//	}
//
//
//	
//	if (keys[GLFW_KEY_R]) //Leer archivo
//	{
//		ReadKeyFramesFile();
//	}
//
//
//	//if (keys[GLFW_KEY_M]) //Leer archivo de datos basura en caso de que el programa ignore los limites de cuadros por segundo
//	//{
//	//	Weathley();
//	//}
//
//
//
//	if (keys[GLFW_KEY_P])
//	{
//		if (reinicioFrame < 1)
//		{
//			guardoFrame = 0;
//			reinicioFrame++;
//			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
//		}
//	}
//
//
//	if (keys[GLFW_KEY_1])
//	{
//		if (ciclo < 1)
//		{
//			//printf("moxModelo_X es: %f\n", moxModelo_X);
//			moxModelo_X -= 1.0f;
//			printf("\n moxModelo_X es: %f\n", moxModelo_X);
//			ciclo++;
//			ciclo2 = 0;
//			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
//		}
//
//	}
//	if (keys[GLFW_KEY_2])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes modificar tu variable moxModelo_X presionando la tecla 1\n");
//		}
//	}
//
//	if (keys[GLFW_KEY_3])
//	{
//		if (ciclo < 1)
//		{
//			//printf("moxModelo_X es: %f\n", moxModelo_X);
//			moxModelo_X += 1.0f;
//			printf("\n moxModelo_X es: %f\n", moxModelo_X);
//			ciclo++;
//			ciclo2 = 0;
//			printf("\n Presiona la tecla 4 para poder habilitar la variable\n");
//		}
//
//	}
//	if (keys[GLFW_KEY_4])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes modificar tu variable moxModelo_X presionando la tecla 3\n");
//		}
//	}
//
//	if (keys[GLFW_KEY_5])
//	{
//		if (ciclo < 1)
//		{
//			//printf("moxModelo_X es: %f\n", moxModelo_X);
//			moxModelo_Y -= 1.0f;
//			printf("\n moxModelo_Y es: %f\n", moxModelo_Y);
//			ciclo++;
//			ciclo2 = 0;
//			printf("\n Presiona la tecla 6 para poder habilitar la variable\n");
//		}
//
//	}
//	if (keys[GLFW_KEY_6])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes modificar tu variable moxModelo_Y presionando la tecla 5\n");
//		}
//	}
//
//	if (keys[GLFW_KEY_7])
//	{
//		if (ciclo < 1)
//		{
//			//printf("moxModelo_X es: %f\n", moxModelo_X);
//			moxModelo_Y += 1.0f;
//			printf("\n moxModelo_Y es: %f\n", moxModelo_Y);
//			ciclo++;
//			ciclo2 = 0;
//			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
//		}
//
//	}
//	if (keys[GLFW_KEY_8])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes modificar tu variable moxModelo_Y presionando la tecla 7\n");
//		}
//	}
//
//	if (keys[GLFW_KEY_9])
//	{
//		if (ciclo < 1)
//		{
//			//printf("moxModelo_X es: %f\n", moxModelo_X);
//			giroModelo += 45.0f;
//			printf("\n giroModelo es: %f\n", giroModelo);
//			ciclo++;
//			ciclo2 = 0;
//			printf("\n Presiona la tecla G para poder habilitar la variable  giroModelo\n");
//		}
//
//	}
//	if (keys[GLFW_KEY_G])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes modificar tu variable de giroModelo presionando la tecla 9 \n");
//		}
//	}
//
//	if (keys[GLFW_KEY_T]) // Decrementa la rotacion
//	{
//		if (ciclo < 1)
//		{
//			if (giroModelo > 0.0f)
//			{
//				giroModelo -= 45.0f;
//				printf("\n giroModelo es: %f\n", giroModelo);
//				printf("\n Presiona la tecla Y para guardar el valor de giroModelo\n");
//				ciclo++;
//				ciclo2 = 0;
//			}
//		}
//	}
//
//	if (keys[GLFW_KEY_Y])
//	{
//		if (ciclo2 < 1)
//		{
//			ciclo = 0;
//			ciclo2++;
//			printf("\n Ya puedes decrementar la variable giroModelo con la tecla T\n");
//		}
//	}
//
}
