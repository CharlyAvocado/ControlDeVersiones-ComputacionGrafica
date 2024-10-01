#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;
	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;
	articulacion7 = 0.0f;
	articulacion8 = 0.0f;
	articulacion9 = 0.0f;
	articulacion10= 0.0f;
	articulacion11 = 0.0f;
	articulacion12 = 0.0f;
	articulacion13 = 0.0f;
	articulacion14 = 0.0f;
	articulacion15 = 0.0f;
	articulacion16 = 0.0f;
	articulacion17 = 0.0f;
	articulacion18 = 0.0f;
	articulacion19 = 0.0f;
	articulacion20 = 0.0f;
	traslacion = 0.0f;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	//Para segundo monitor (Helios)
	//int monitorCount;
	//GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

	//// Si hay más de un monitor, selecciona el segundo
	//GLFWmonitor* secondMonitor = (monitorCount > 1) ? monitors[1] : monitors[0];  // Si solo hay un monitor, usar el principal.

	//// Obtener el modo de video del monitor
	//const GLFWvidmode* mode = glfwGetVideoMode(secondMonitor);
	//mainWindow = glfwCreateWindow(mode->width, mode->height, "Practica 04: Modelado Jerarquico", secondMonitor, NULL);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica 07: Iluminacion 1", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	
	// Traslacion 
	if (key == GLFW_KEY_UP)
	{
		theWindow->traslacion -= 0.5f;  // Mover hacia adelante
	}

	if (key == GLFW_KEY_DOWN)
	{
		theWindow->traslacion += 0.5f;  // Mover hacia atrás
	}

	if (key == GLFW_KEY_UP)
	{
		theWindow->articulacion13 += 75.0;
	}
	if (key == GLFW_KEY_DOWN)
	{
		theWindow->articulacion13 -= 50.0;
	}


	if (key == GLFW_KEY_E)
	{
		theWindow->rotax += 10.0;
	}
	if (key == GLFW_KEY_R)
	{
		theWindow->rotay += 10.0; //rotar sobre el eje y 10 grados
	}
	if (key == GLFW_KEY_T)
	{
		theWindow->rotaz += 10.0;
	}
	if (key == GLFW_KEY_F)
	{
		if (theWindow->articulacion1 > -1.0)
		{
		}
		else
		{
			theWindow->articulacion1 += 10.0;
		}
		
		}
	if (key == GLFW_KEY_G)
	{
		if (theWindow->articulacion1 < -45.0)
		{
		}
		else
		{
			theWindow->articulacion1 -= 10.0;
		}
	}

		if (key == GLFW_KEY_H)
	{
		if (theWindow->articulacion3 > 45.0)
		{
		}
		else
		{
			theWindow->articulacion3 += 10.0;
		}

	}
	if (key == GLFW_KEY_J)
	{
		if (theWindow->articulacion3 < -45.0)
		{
		}
		else
		{
			theWindow->articulacion3 -= 10.0;
		}
	}
	if (key == GLFW_KEY_K)
	{
		if (theWindow->articulacion5 > 9.9)
		{
		}
		else
		{
			theWindow->articulacion5 += 10.0;
		}

	}
	if (key == GLFW_KEY_L)
	{
		if (theWindow->articulacion5 < -35.0)
		{
		}
		else
		{
			theWindow->articulacion5 -= 10.0;
		}
	}
	if (key == GLFW_KEY_C)
	{
		if(theWindow->articulacion7 > 45.0)
		{
		}
	else
	{
		theWindow->articulacion7 += 10.0;
	}

	}
	if (key == GLFW_KEY_V)
	{
		if (theWindow->articulacion7 < -45.0)
		{
		}
		else
		{
			theWindow->articulacion7 -= 10.0;
		}
	}
	if (key == GLFW_KEY_B)
	{
		if (theWindow->articulacion9 > 45.0)
		{
		}
		else
		{
			theWindow->articulacion9 += 10.0;
		}

	}
	if (key == GLFW_KEY_N)
	{
		if (theWindow->articulacion9 < -45.0)
		{
		}
		else
		{
			theWindow->articulacion9 -= 10.0;
		}
	}

	if (key == GLFW_KEY_Y)
	{
		if (theWindow->articulacion11 > 45.0)
		{
		}
		else
		{
			theWindow->articulacion11 += 10.0;
		}

	}
	if (key == GLFW_KEY_U)
	{
		if (theWindow->articulacion11 < -45.0)
		{
		}
		else
		{
			theWindow->articulacion11 -= 10.0;
		}
	}
	if (key == GLFW_KEY_I)
	{
		theWindow->articulacion13 += 10.0;
	}
	if (key == GLFW_KEY_O)
	{
		theWindow->articulacion13 -= 10.0;
	}
	if (key == GLFW_KEY_1)
	{
		theWindow->articulacion15 += 10.0;
	}
	if (key == GLFW_KEY_2)
	{
		theWindow->articulacion16 += 10.0;
	}
	if (key == GLFW_KEY_3)
	{
		theWindow->articulacion17 += 10.0;
	}
	if (key == GLFW_KEY_4)
	{
		theWindow->articulacion18 += 10.0;
	}
	if (key == GLFW_KEY_5)
	{
		theWindow->articulacion19 += 10.0;
	}
	if (key == GLFW_KEY_6)
	{
		theWindow->articulacion20 += 10.0;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		const char* key_name = glfwGetKeyName(GLFW_KEY_D, 0);
		//printf("se presiono la tecla: %s\n",key_name);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
