/*
*	COMP 371 - Assignment 1
*
*	Umaid Malik 27576110
*	Daniel Thibault-Shea 40073133
*	Noor Alali
*	Vishal Senewiratne
*	Dominic Hart
*
*	CONTROLS:

*		VIEW SELECTION CONTROLS
*		==============================================================
*
*       ESCAPE              : QUIT PROGRAM
*
*		==============================================================
*

*		X					: MOVE NEGATIVE Y-DIRECTION
*
*		SPACE				: MOVE POSITIVE Y-DIRECTION
*
*		Q					: PERSPECTIVE PROJECTION
*
*		E					: ORTHOGRAPHIC PROJECTION
*
*		T					: RENDER TRIANGLES
*
*		L					: RENDER LINES
*
*		P					: RENDER POINTS
*
*		C					: HOLD TO DISABLE BACKFACE CULLING
*
*		HOME				: RETURN TO INITIAL POSTION
*
*		MOUSE SCROLL UP		: ZOOM IN
*
*		MOUSE SCROLL DOWN	: ZOOM OUT
*
*
*		PAN AND TILT (MOUSE MOVEMENT) WHILE HOLDING RIGHT MOUSE BUTTON
*		==============================================================
*
*		W		: MOVE FORWARD
*
*		S		: MOVE BACKWARD
*
*		A		: MOVE LEFT
*
*		D		: MOVE RIGHT
*
*		SHIFT	: INCREASE MOVEMENT SPEED
*
*		==============================================================
*
*
*		MODEL CONTROL
*		ENTER EITHER 1, 2, 3, 4 OR 5 TO SELECT MODELS - 0 WILL SELECT ALL MODELS - LEFT MOUSE CLICK WILL DESELECT ALL MODELS
*		====================================================================================================================
*		
*		W						: MOVE MODEL IN +Z DIRECTION
*
*		S						: MOVE MODEL IN -Z DIRECTION
*
*		A						: MOVE MODEL IN -X DIRECTION
*
*		D						: MOVE MODEL IN +X DIRECTION
*
*		X						: MOVE MODEL IN -Y DIRECTION
*
*		SPACE					: MOVE MODEL IN +Y DIRECTION
*
*		R						: RESET MODEL TO INITIAL POSITION
*
*		SHIFT + R				: RESET MODEL TO INITIAL SIZE
*
*		LCTRL + SHIFT + R		: RESET MODEL ORIENTATION
*
*		U						: UPSCALE MODEL
*
*		J						: DOWNSCALE MODEL
*
*		T						: ROTATE MODEL +X DIRECTION
*
*		Y						: ROTATE MODEL -X DIRECTION
*
*		G						: ROTATE MODEL +Y DIRECTION
*
*		H						: ROTATE MODEL -Y DIRECTION
*
*		B						: ROTATE MODEL +Z DIRECTION
*
*		N						: ROTATE MODEL -Z DIRECTION
*
*		SHIFT + W/A/S/D/X/SPACE	: INCREASE MOVEMENT SPEED
*
*		SHIFT + U/J				: INCREASE SCALING SPEED
*
*
*		ARROW UP/DOWN/LEFT/RIGHT: ORIENTATION CONTROL
*/

#include <iostream>
#include <algorithm>

#define GLEW_STATIC 1 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


const int numbObjInScene = 5;

const int numGridLines = 100;                           // how many gridlines (going one way)

glm::vec3 initialCameraPosition(0.6f, 1.0f, 1.5f);
glm::vec3 initialcameraLookAt(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

glm::vec3 center(0.0f, 0.0f, 0.0f);

glm::vec3 cameraPosition = initialCameraPosition;
glm::vec3 cameraLookAt = initialcameraLookAt;

glm::mat4 identityMatrix = glm::mat4(1.0f);

glm::mat4 projectionMatrix = identityMatrix;
glm::mat4 viewMatrix = identityMatrix;
glm::mat4 worldMatrix = identityMatrix;

glm::mat4 rotationMatrix = identityMatrix;
glm::mat4 scalingMatrix = identityMatrix;
glm::mat4 shearingMatrix = identityMatrix;
glm::mat4 translationMatrix = identityMatrix;
glm::mat4 partMatrix;

glm::mat4 modelTranslationMatrix;
glm::mat4 modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;
float cameraSpeed = 1.0f;
float cameraSpeedFast = 4 * cameraSpeed;
float deltaTime;
float fov = 90.0f;

double lastMousePosX, lastMousePosY;

// model parameters
float modelSpeed = 1.0f;

bool ONE_KEY_PRESSED = false;
bool TWO_KEY_PRESSED = false;
bool THREE_KEY_PRESSED = false;
bool FOUR_KEY_PRESSED = false;
bool FIVE_KEY_PRESSED = false;

bool T_KEY = GLFW_RELEASE;
bool Y_KEY = GLFW_RELEASE;
bool G_KEY = GLFW_RELEASE;
bool H_KEY = GLFW_RELEASE;
bool B_KEY = GLFW_RELEASE;
bool N_KEY = GLFW_RELEASE;

GLuint worldMatrixLocation;

// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// models
void model_N7(int shaderProgram, float scalingFactor, float rotationAngle, glm::mat4 rotationMat, glm::vec3 worldPosition);
void model_A7();
void model_O9();
void model_S0();
void model_M6();

GLFWwindow* setupWindow();
// end of function protoypes


const char* getVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aColor;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 viewMatrix = mat4(1.0);" // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"out vec3 vertexColor;"
		"void main()"
		"{"
		"   vertexColor = aColor;"
		"	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
}

const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"out vec4 FragColor;"
		"in vec3 vertexColor;"
		"void main()"
		"{"
		" FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
		"}";
}

int compileAndLinkShaders()
{
	// compile and link shader program
	// return shader program id

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = getVertexShaderSource();
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//fragment shader
	unsigned int fragmentShader;
	const char* fragmentShaderSource = getFragmentShaderSource();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TURE);
	glfwWindowHint(GL_DEPTH_TEST, 24);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "COMP 371 - Team 6 - Part 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	return window;
}

GLuint VAO[numbObjInScene];	// vertexArrayObject
GLuint VBO[numbObjInScene];	// vertexBufferObject

int main()
{
	// Setup a window
	GLFWwindow* window = setupWindow();

	// Compile and link shaders
    unsigned int shaderProgram = compileAndLinkShaders();

	// defines vertices and color for the cube
	glm::vec3 vertexArray[] = {

		// cube made of 12 triangles

		// postion						// color
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),

		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),

		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),

		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.20f, 0.20f, 0.20f),
		glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),

		glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.0f, 0.1f, 0.1f), glm::vec3(0.4f, 0.4f, 0.4f),
	};

	const float axisLineLength = 5 * (10.0f / (float)numGridLines);  // axis lines are the length of n grid squares

	// red x axis line
	glm::vec3 redLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(axisLineLength, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	};

	// green y axis line
	glm::vec3 greenLine[] = {
		glm::vec3(0.0f,  0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, axisLineLength, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
	};

	// yellow z axis line
	glm::vec3 blueLine[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, axisLineLength), glm::vec3(1.0f, 1.0f,0.0f),
	};

	
	glGenVertexArrays(numbObjInScene, VAO);
	glGenBuffers(numbObjInScene, VBO);


	// cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));   // color attribute
	glEnableVertexAttribArray(1);


	// red line
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redLine), redLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// green line
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(greenLine), greenLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);


	// blue line
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blueLine), blueLine, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	// grid lines

	
	const float spacing = 10.0f / (float)numGridLines;       // divide the 2.0 world into the number of gridlines
	float increment = 0.0f;                                 // how much to move a line over
	const int numDataPoints = 8;                            // this is how many vec3's there are in one gridline (4 vertices with 1 color each)
	glm::vec3 zLineColor = glm::vec3(1.0f, 0.0f, 1.0f);     // set line color for lines running parallel to z-axis
	glm::vec3 xLineColor = glm::vec3(1.0f, 1.0f, 0.0f);     // set line color for lines running parallel to x-axis
	glm::vec3 gridLines[numDataPoints * numGridLines];
	

	for (int i = 0; i < numGridLines; ++i) {
		// lines parallel to z-axis
		gridLines[i * numDataPoints ] = glm::vec3(-5.0f + increment, 0.0f, -5.0f);
		gridLines[i * numDataPoints + 1] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		gridLines[i * numDataPoints + 2] = glm::vec3(-5.0f + increment, 0.0f, 5.0f);
		gridLines[i * numDataPoints + 3] = glm::vec3(zLineColor.x, zLineColor.y, zLineColor.z);
		// lines parallel to x-axis
		gridLines[i * numDataPoints + 4] = glm::vec3(-5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 5] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);
		gridLines[i * numDataPoints + 6] = glm::vec3(5.0f, 0.0f, -5.0f + increment);
		gridLines[i * numDataPoints + 7] = glm::vec3(xLineColor.x, xLineColor.y, xLineColor.z);

		increment += spacing;
	}
	
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridLines), gridLines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
	

	// bind to nothing so we don't inadvertantly modify something
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(shaderProgram);

	projectionMatrix = glm::perspective(90.0f, 1024.0f / 768.0f, 0.0005f, 500.0f);

	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	// Variables to be used later in tutorial
	float angle = 0.0f;
	float rotationSpeed = 90.0f;  // degrees per second
	float lastFrameTime = glfwGetTime();
	

	// read mouse position
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);     // clockwise polygons are considered as front-facing

	// z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	
	// initial model parameters
	
	// model A7											// model O9											// model S0											// model M6
	const float init_A7_Size = 1.0f;					const float init_O9_Size = 1.0f;					const float init_S0_Size = 1.0f;					const float init_M6_Size = 1.0f;
	float model_A7_Size = 0.0f;							float model_O9_Size = 0.0f;							float model_S0_Size = 0.0f;							float model_M6_Size = 0.0f;
	glm::vec3 initPos_A7(-4.0f, 0.5f, -4.0f);			glm::vec3 initPos_O9(4.0f, 0.5f, -4.0f);			glm::vec3 initPos_S0(-4.0f, 0.5f, 4.0f);			glm::vec3 initPos_M6(4.0f, 0.5f, 4.0f);
	glm::vec3 model_A7_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_O9_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_S0_Position(0.0f, 0.0f, 0.0f);		glm::vec3 model_M6_Position(0.0f, 0.0f, 0.0f);
	glm::vec3 A7_theta(0.0f, 0.0f, 0.0f);				glm::vec3 O9_theta(0.0f, 0.0f, 0.0f);				glm::vec3 S0_theta(0.0f, 0.0f, 0.0f);				glm::vec3 M6_theta(0.0f, 0.0f, 0.0f);
	
	glm::mat4 A7_rotation_X;							glm::mat4 O9_rotation_X;							glm::mat4 S0_rotation_X;							glm::mat4 M6_rotation_X;
	glm::mat4 A7_rotation_Y;							glm::mat4 O9_rotation_Y;							glm::mat4 S0_rotation_Y;							glm::mat4 M6_rotation_Y;
	glm::mat4 A7_rotation_Z;							glm::mat4 O9_rotation_Z;							glm::mat4 S0_rotation_Z;							glm::mat4 M6_rotation_Z;
	
	

	// These are for model N7 and need to live outside the loop (Dan was a bit different and changes position by altering parameters to the function)
	const glm::vec3 N7_init = glm::vec3(0.05f, 0.0f, -0.25f);     // { x, y, z, scaling }
	glm::vec3 N7_change = glm::vec3(0.0f, 0.0f, 0.0f);           // { x, y, z, scaling }

	const float N7_init_scaling = 2.0f;
	float N7_change_scaling = 0.0f;

	const float N7_init_angle = 0.0f;
	float N7_change_angle = 0.0f;
	glm::vec3 rotationAxes = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 N7_theta(0.0f, 0.0f, 0.0f);
	glm::mat4 N7_rotation_X;
	glm::mat4 N7_rotation_Y;
	glm::mat4 N7_rotation_Z;
	
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{

		//  background color
		//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	// sky blue color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPointSize(10);		// increased point size so that it is easily visible
		glLineWidth(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);


		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime += deltaTime;
		

		// beginning of model A7
		A7_theta = { A7_theta.x, A7_theta.y, A7_theta.z };
		rotationMatrix = A7_rotation_X * A7_rotation_Y * A7_rotation_Z;
		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_A7.x + model_A7_Position.x, initPos_A7.y + model_A7_Position.y, initPos_A7.z + model_A7_Position.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_A7_Size + init_A7_Size, model_A7_Size + init_A7_Size, model_A7_Size + init_A7_Size));
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		
		if (ONE_KEY_PRESSED)
		{
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) 
				model_A7_Size += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) 
				model_A7_Size -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				model_A7_Position.x -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				model_A7_Position.x += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				model_A7_Position.z -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				model_A7_Position.z += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				model_A7_Position.y += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				model_A7_Position.y -= deltaTime * modelSpeed;

			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)		
				model_A7_Position = { 0.0f, 0.0f, 0.0f };
			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
				model_A7_Size = 0.0f;
			
			if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T)))	// rotate in +y axis
				A7_theta.x = A7_theta.x + glm::radians(5.0f);
			T_KEY = glfwGetKey(window, GLFW_KEY_T);

			if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y)))	// rotate in -y axis
				A7_theta.x = A7_theta.x - glm::radians(5.0f);
			Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

			if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G))) 	// rotate in +x axis
				A7_theta.y = A7_theta.y + glm::radians(5.0f);
			G_KEY = glfwGetKey(window, GLFW_KEY_G);

			if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H))) 	// rotate in -x axis
				A7_theta.y = A7_theta.y - glm::radians(5.0f);
			H_KEY = glfwGetKey(window, GLFW_KEY_H);

			if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B)))	// rotate in +z axis
				A7_theta.z = A7_theta.z + glm::radians(5.0f);
			B_KEY = glfwGetKey(window, GLFW_KEY_B);


			if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
				A7_theta.z = A7_theta.z - glm::radians(5.0f);
			N_KEY = glfwGetKey(window, GLFW_KEY_N);

			if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS)	// reset model orientation
				A7_theta = { 0.0f, 0.0f, 0.0f };  // set theta back to zero

			A7_rotation_X = {
									  1,            0,             0, 0,
									  0, cos(A7_theta.x), -sin(A7_theta.x), 0,
									  0, sin(A7_theta.x),  cos(A7_theta.x), 0,
									  0,            0,             0, 1,
			};

			A7_rotation_Y = {
									  cos(A7_theta.y),  0, sin(A7_theta.y), 0,
												 0,  1,            0, 0,
									 -sin(A7_theta.y),  0, cos(A7_theta.y), 0,
												 0,  0,            0, 1,
			};

			A7_rotation_Z = {
						   cos(A7_theta.z), -sin(A7_theta.z), 0, 0,
						   sin(A7_theta.z),  cos(A7_theta.z), 0, 0,
									  0,             0, 1, 0,
									  0,             0, 0, 1,
			};

			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_A7_Size + init_A7_Size, model_A7_Size + init_A7_Size, model_A7_Size + init_A7_Size));
			modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_A7.x + model_A7_Position.x, initPos_A7.y + model_A7_Position.y, initPos_A7.z + model_A7_Position.z));
			rotationMatrix = A7_rotation_X * A7_rotation_Y * A7_rotation_Z;
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		}
		
		model_A7();
		// end of model_A7();

		// beginning of model O9
		O9_theta = { O9_theta.x, O9_theta.y, O9_theta.z };
		rotationMatrix = O9_rotation_X * O9_rotation_Y * O9_rotation_Z;
		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_O9.x + model_O9_Position.x, initPos_O9.y + model_O9_Position.y, initPos_O9.z + model_O9_Position.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_O9_Size + init_O9_Size, model_O9_Size + init_O9_Size, model_O9_Size + init_O9_Size));
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		if (TWO_KEY_PRESSED)
		{
			
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) 
				model_O9_Size += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
				model_O9_Size -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				model_O9_Position.x -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				model_O9_Position.x += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				model_O9_Position.z -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				model_O9_Position.z += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				model_O9_Position.y += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				model_O9_Position.y -= deltaTime * modelSpeed;

			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) 
				model_O9_Position = { 0.0f, 0.0f, 0.0f };

			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
				model_O9_Size = 0.0f;

			if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T))) 	// rotate in +y axis
				O9_theta.x = O9_theta.x + glm::radians(5.0f);
			T_KEY = glfwGetKey(window, GLFW_KEY_T);

			if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y))) 	// rotate in -y axis
				O9_theta.x = O9_theta.x - glm::radians(5.0f);
			Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

			if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G))) 	// rotate in +x axis
				O9_theta.y = O9_theta.y + glm::radians(5.0f);
			G_KEY = glfwGetKey(window, GLFW_KEY_G);

			if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H))) 	// rotate in -x axis
				O9_theta.y = O9_theta.y - glm::radians(5.0f);
			H_KEY = glfwGetKey(window, GLFW_KEY_H);

			if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B))) 	// rotate in +z axis
				O9_theta.z = O9_theta.z + glm::radians(5.0f);
			B_KEY = glfwGetKey(window, GLFW_KEY_B);

			if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
				O9_theta.z = O9_theta.z - glm::radians(5.0f);
			N_KEY = glfwGetKey(window, GLFW_KEY_N);

			if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS) 	// reset model orientation
				O9_theta = { 0.0f, 0.0f, 0.0f };  // set theta back to zero
		
			O9_rotation_X = {
									  1,            0,             0, 0,
									  0, cos(O9_theta.x), -sin(O9_theta.x), 0,
									  0, sin(O9_theta.x),  cos(O9_theta.x), 0,
									  0,            0,             0, 1,
			};

			O9_rotation_Y = {
						              cos(O9_theta.y),  0, sin(O9_theta.y), 0,
									             0,  1,            0, 0,
							         -sin(O9_theta.y),  0, cos(O9_theta.y), 0,
									             0,  0,            0, 1,
			};

			O9_rotation_Z = {
						   cos(O9_theta.z), -sin(O9_theta.z), 0, 0,
						   sin(O9_theta.z),  cos(O9_theta.z), 0, 0,
									  0,             0, 1, 0,
									  0,             0, 0, 1,
			};

			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_O9_Size + init_O9_Size, model_O9_Size + init_O9_Size, model_O9_Size + init_O9_Size));
			modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_O9.x + model_O9_Position.x, initPos_O9.y + model_O9_Position.y, initPos_O9.z + model_O9_Position.z));
			rotationMatrix = O9_rotation_X * O9_rotation_Y * O9_rotation_Z;
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		}

		model_O9();
		// end of model O9


		// beginning of model S0
		S0_theta = { S0_theta.x, S0_theta.y, S0_theta.z };
		rotationMatrix = S0_rotation_X * S0_rotation_Y * S0_rotation_Z;
		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_S0.x + model_S0_Position.x, initPos_S0.y + model_S0_Position.y, initPos_S0.z + model_S0_Position.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_S0_Size + init_S0_Size, model_S0_Size + init_S0_Size, model_S0_Size + init_S0_Size));
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		if (THREE_KEY_PRESSED)
		{

			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) 
				model_S0_Size += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) 
				model_S0_Size -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				model_S0_Position.x -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				model_S0_Position.x += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				model_S0_Position.z -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				model_S0_Position.z += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				model_S0_Position.y += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				model_S0_Position.y -= deltaTime * modelSpeed;

			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
				model_S0_Position = { 0.0f, 0.0f, 0.0f };

			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
				model_S0_Size = 0.0f;

			if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T)))	// rotate in +y axis
				S0_theta.x = S0_theta.x + glm::radians(5.0f);
			T_KEY = glfwGetKey(window, GLFW_KEY_T);

			if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y))) 	// rotate in -y axis
				S0_theta.x = S0_theta.x - glm::radians(5.0f);
			Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

			if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G))) 	// rotate in +x axis
				S0_theta.y = S0_theta.y + glm::radians(5.0f);
			G_KEY = glfwGetKey(window, GLFW_KEY_G);

			if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H))) 	// rotate in -x axis
				S0_theta.y = S0_theta.y - glm::radians(5.0f);
			H_KEY = glfwGetKey(window, GLFW_KEY_H);

			if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B))) 	// rotate in +z axis
				S0_theta.z = S0_theta.z + glm::radians(5.0f);
			B_KEY = glfwGetKey(window, GLFW_KEY_B);

			if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
				S0_theta.z = S0_theta.z - glm::radians(5.0f);
			N_KEY = glfwGetKey(window, GLFW_KEY_N);

			if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS) 	// reset model orientation
				S0_theta = { 0.0f, 0.0f, 0.0f };  // set theta back to zero

			S0_rotation_X = {
									  1,            0,             0, 0,
									  0, cos(S0_theta.x), -sin(S0_theta.x), 0,
									  0, sin(S0_theta.x),  cos(S0_theta.x), 0,
									  0,            0,             0, 1,
			};

			S0_rotation_Y = {
									  cos(S0_theta.y),  0, sin(S0_theta.y), 0,
												 0,  1,            0, 0,
									 -sin(S0_theta.y),  0, cos(S0_theta.y), 0,
												 0,  0,            0, 1,
			};

			S0_rotation_Z = {
						   cos(S0_theta.z), -sin(S0_theta.z), 0, 0,
						   sin(S0_theta.z),  cos(S0_theta.z), 0, 0,
									  0,             0, 1, 0,
									  0,             0, 0, 1,
			};

			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_S0_Size + init_S0_Size, model_S0_Size + init_S0_Size, model_S0_Size + init_S0_Size));
			modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_S0.x + model_S0_Position.x, initPos_S0.y + model_S0_Position.y, initPos_S0.z + model_S0_Position.z));
			rotationMatrix = S0_rotation_X * S0_rotation_Y * S0_rotation_Z;
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		}

		model_S0();
		// end of model S0

		// beginning of model M6
		M6_theta = { M6_theta.x, M6_theta.y, M6_theta.z };
		rotationMatrix = M6_rotation_X * M6_rotation_Y * M6_rotation_Z;
		modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_M6.x + model_M6_Position.x, initPos_M6.y + model_M6_Position.y, initPos_M6.z + model_M6_Position.z));
		modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_M6_Size + init_M6_Size, model_M6_Size + init_M6_Size, model_M6_Size + init_M6_Size));
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		if (FOUR_KEY_PRESSED)
		{
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) 
				model_M6_Size += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) 
				model_M6_Size -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				model_M6_Position.x -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				model_M6_Position.x += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				model_M6_Position.z -= deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				model_M6_Position.z += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				model_M6_Position.y += deltaTime * modelSpeed;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				model_M6_Position.y -= deltaTime * modelSpeed;

			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
				model_M6_Position = { 0.0f, 0.0f, 0.0f };
			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
				model_M6_Size = 0.0f;

			if ((T_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_T)))	// rotate in +y axis
				M6_theta.x = M6_theta.x + glm::radians(5.0f);
			T_KEY = glfwGetKey(window, GLFW_KEY_T);

			if ((Y_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_Y))) 	// rotate in -y axis
				M6_theta.x = M6_theta.x - glm::radians(5.0f);
			Y_KEY = glfwGetKey(window, GLFW_KEY_Y);

			if ((G_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_G)))	// rotate in +x axis
				M6_theta.y = M6_theta.y + glm::radians(5.0f);
			G_KEY = glfwGetKey(window, GLFW_KEY_G);

			if ((H_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_H)))	// rotate in -x axis
				M6_theta.y = M6_theta.y - glm::radians(5.0f);
			H_KEY = glfwGetKey(window, GLFW_KEY_H);

			if ((B_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_B)))	// rotate in +z axis
				M6_theta.z = M6_theta.z + glm::radians(5.0f);
			B_KEY = glfwGetKey(window, GLFW_KEY_B);

			if ((N_KEY == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_N))) 	// rotate in -z axis
				M6_theta.z = M6_theta.z - glm::radians(5.0f);
			N_KEY = glfwGetKey(window, GLFW_KEY_N);

			if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(window, GLFW_KEY_R)) == GLFW_PRESS) 	// reset model orientation
				M6_theta = { 0.0f, 0.0f, 0.0f };  // set theta back to zero

			M6_rotation_X = {
									  1,            0,             0, 0,
									  0, cos(M6_theta.x), -sin(M6_theta.x), 0,
									  0, sin(M6_theta.x),  cos(M6_theta.x), 0,
									  0,            0,             0, 1,
			};

			M6_rotation_Y = {
									  cos(M6_theta.y),  0, sin(M6_theta.y), 0,
												 0,  1,            0, 0,
									 -sin(M6_theta.y),  0, cos(M6_theta.y), 0,
												 0,  0,            0, 1,
			};

			M6_rotation_Z = {
						   cos(M6_theta.z), -sin(M6_theta.z), 0, 0,
						   sin(M6_theta.z),  cos(M6_theta.z), 0, 0,
									  0,             0, 1, 0,
									  0,             0, 0, 1,
			};

			modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model_M6_Size + init_M6_Size, model_M6_Size + init_M6_Size, model_M6_Size + init_M6_Size));
			modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initPos_M6.x + model_M6_Position.x, initPos_M6.y + model_M6_Position.y, initPos_M6.z + model_M6_Position.z));
			rotationMatrix = M6_rotation_X * M6_rotation_Y * M6_rotation_Z;
			worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		}

		model_M6();
		// end of model M6

		// beginning of model N7
		float frameChangeAmount = deltaTime * modelSpeed;

		N7_rotation_X = {
									  1,            0,             0, 0,
									  0, cos(N7_theta.x), -sin(N7_theta.x), 0,
									  0, sin(N7_theta.x),  cos(N7_theta.x), 0,
									  0,            0,             0, 1,
		};

		N7_rotation_Y = {
								  cos(N7_theta.y),  0, sin(N7_theta.y), 0,
											 0,  1,            0, 0,
								 -sin(N7_theta.y),  0, cos(N7_theta.y), 0,
											 0,  0,            0, 1,
		};

		N7_rotation_Z = {
					   cos(N7_theta.z), -sin(N7_theta.z), 0, 0,
					   sin(N7_theta.z),  cos(N7_theta.z), 0, 0,
								  0,             0, 1, 0,
								  0,             0, 0, 1,
		};

		rotationMatrix = N7_rotation_X * N7_rotation_Y * N7_rotation_Z;


		if (FIVE_KEY_PRESSED) {
			if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)       // UPSCALE MODEL
				N7_change_scaling += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)       // DOWNSCALE MODEL
				N7_change_scaling -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)       // MOVE MODEL IN + Z DIRECTION
				N7_change.z -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)       // MOVE MODEL IN - Z DIRECTION
				N7_change.z += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)       // MOVE MODEL IN - X DIRECTION
				N7_change.x -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)       // MOVE MODEL IN + X DIRECTION
				N7_change.x += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)       // MOVE MODEL IN - Y DIRECTION
				N7_change.y -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)   // MOVE MODEL IN + Y DIRECTION
				N7_change.y += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&     // RESET MODEL TO INITIAL POSITION
				glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
				N7_change = glm::vec3(0.0f, 0.0f, 0.0f);
				N7_theta.x = 0.0f;
				N7_theta.y = 0.0f;
				N7_theta.z = 0.0f;
			}
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&     // RESET MODEL TO INITIAL SIZE
				(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == GLFW_PRESS)
				N7_change_scaling = 0.0f;
			if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)    	// rotate left in y-axis
				N7_theta.y -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)	    // rotate right in y-axis
				N7_theta.y += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)	    // rotate left in x-axis
				N7_theta.x -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)	    // rotate right in x-axis
				N7_theta.x += frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)	    // rotate left in z-axis
				N7_theta.z -= frameChangeAmount;
			if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)	    // rotate right in z-axis
				N7_theta.z += frameChangeAmount;
		}


		model_N7(shaderProgram, (N7_init_scaling + N7_change_scaling), (N7_init_angle + N7_change_angle), rotationMatrix,
			glm::vec3(N7_init.x + N7_change.x, N7_init.y + N7_change.y, N7_init.z + N7_change.z));
		// end of model N7

		// red line
		glLineWidth(5);
		glBindVertexArray(VAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);
		
		// green line
		glBindVertexArray(VAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// blue line
		glBindVertexArray(VAO[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);

		// grid lines
		glLineWidth(1);
		glBindVertexArray(VAO[4]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2 * 2 * numGridLines);
		
		glBindVertexArray(0);

		
		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		//glfwSwapInterval(0);
		glfwPollEvents();

		// Detect inputs
		processInput(window); 

	}

	// de-allocate all resources
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}


// resize window 
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Press L to render model in LINE mode
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Press T to render model in TRIANGLE mode
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Pess P to render model in POINT mode
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}


	// perspective - projection transform
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		projectionMatrix = glm::perspective(90.0f,			// field of view in degrees
			1024.0f / 768.0f,	// aspect ratio
			0.005f, 500.0f);	// near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	// orthographic - projection transform
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		projectionMatrix = glm::ortho(-4.0f, 4.0f,      // left/right
			-3.0f, 3.0f,	  // bottom/top
			-100.0f, 100.0f);  // near/far 

		GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}


	// Press and hold C to disable backface culling
	glEnable(GL_CULL_FACE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !GLFW_RELEASE)
	{
		glDisable(GL_CULL_FACE);
	}


	bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	float currentCameraSpeed = (fastCam) ? cameraSpeedFast : cameraSpeed;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE)) {

		// disable mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// detect mouse input
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		// change in position
		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// conversion to sperical coordinates
		const float cameraAngularSpeed = 5.0f;	// mouse speed
		cameraHorizontalAngle -= dx * cameraAngularSpeed * deltaTime;
		cameraVerticalAngle -= dy * cameraAngularSpeed * deltaTime;

		// limit range of vertical camera angle
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		// conversion to radians
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
		{
			cameraPosition += cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera back
		{
			cameraPosition -= cameraLookAt * deltaTime * currentCameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera forward
		{
			cameraPosition.y += currentCameraSpeed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // move camera back
		{
			cameraPosition.y -= currentCameraSpeed * deltaTime;
		}


		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	// enable mouse cursor if right mouse button is released
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// reset to original world position and orientation
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		// reset camera position and lookAt
		cameraPosition = initialCameraPosition;
		cameraLookAt = initialcameraLookAt;

		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}


	// mouse scroll
	glfwSetScrollCallback(window, scroll_callback); // receives mouse scroll as input

	// press 0 mouse button to select all models
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = true;
	}

	// press right mouse button to deselect all models
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 1 to select model A7
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = true;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 2 to select model O9
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = true;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 3 to select model S0 
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = true;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = false;
	}

	// press 4 to select model M6
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = true;
		FIVE_KEY_PRESSED = false;
	}

	// press 5 to select model (your letter) 
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		ONE_KEY_PRESSED = false;
		TWO_KEY_PRESSED = false;
		THREE_KEY_PRESSED = false;
		FOUR_KEY_PRESSED = false;
		FIVE_KEY_PRESSED = true;
	}
	

	// while in model control, holding shift will increase movement speed or scaling speed
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
	{
		modelSpeed = 6.0;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE))
	{
		modelSpeed = 1.0;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {

		cameraVerticalAngle = cameraVerticalAngle + 1;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		float temp = cameraUp.y;
		if ((int)cameraVerticalAngle % 180 == 0) {
			phi = glm::radians(cameraVerticalAngle - 90);
		}
		cameraLookAt = glm::vec3(-cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {


		cameraVerticalAngle = cameraVerticalAngle - 1;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);
		if ((int)cameraVerticalAngle % 180 == 0) {
			phi = glm::radians(cameraVerticalAngle - 90);
		}
		cameraLookAt = glm::vec3(-cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {

		cameraHorizontalAngle = cameraHorizontalAngle - 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {

		cameraHorizontalAngle = cameraHorizontalAngle + 3;
		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		cameraLookAt = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		glm::vec3 cameraSideVector = glm::cross(cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		GLuint viewMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		|| (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE))
	{

		// scroll up to zoom in 
		if (yoffset < 0) {
			std::cout << fov << std::endl;
			if (fov <= 120.0f)
			{
				fov = fov + 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
		// scroll down to zoom out 
		if (yoffset > 0) {
			std::cout << fov << std::endl;
			if (fov >= 10.0f)
			{
				fov = fov - 5.0;
				projectionMatrix = glm::perspective(glm::radians(fov),			// field of view in degrees
					1024.0f / 768.0f,	// aspect ratio
					0.05f, 500.0f);	// near and far (near > 0)

				GLuint projectionMatrixLocation = glGetUniformLocation(compileAndLinkShaders(), "projectionMatrix");
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			}
		}
	}
}


/*
	Draws "N7" on the screen.

	ScalingFactor is undefined for negative values although permitted.

*/
void model_N7(int shaderProgram, float scalingFactor, float rotationAngle, glm::mat4 rotationMat, glm::vec3 worldPosition) {

	// these will grow the model
	float x_scaling = 0.5f * scalingFactor;
	float y_scaling = 2.5f * scalingFactor;
	float z_scaling = 0.5f * scalingFactor;

	// this matrix rotates the model about itself
	//spinMatrix = glm::mat4(1.0f);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	// this matrix moves the entire model around the world, parameters passed will translate the model to where desired
	glm::mat4 translate_final = glm::translate(glm::mat4(1.0f), glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// these matrices are for N, they define the parent-child relationship (starting letter is to the left of origin and has its middle aligned to x-axis)
	glm::mat4 translate_N = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f * scalingFactor, 0.0f * scalingFactor, -0.025f * scalingFactor));
	glm::mat4 translate_N_child = glm::mat4(1.0f);  // to attach children cubes to the "parent" (parent not touched by child matrix)
	shearingMatrix = glm::mat4(1.0f);

	// Left leg of "N" (parent)
	translationMatrix = translate_N;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(x_scaling, y_scaling, z_scaling));
	modelMatrix = translate_final * rotationMat * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Left right of "N".
	translate_N_child = glm::translate(glm::mat4(1.0f), glm::vec3(0.174f * scalingFactor, 0.0f * scalingFactor, 0.0f * scalingFactor));
	translationMatrix = translate_N_child * translate_N;
	modelMatrix = translate_final * rotationMat * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// diagonal of "N".
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.7, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = translate_N_child * translate_N;
	modelMatrix = translate_final * rotationMat * shearingMatrix * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glm::mat4 translate_7 = glm::translate(glm::mat4(1.0f), glm::vec3(0.025f, 0.0f, -0.025f));
	glm::mat4 translate_7_child = glm::mat4(1.0f);

	// diagonal of "7" (parent)
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.5, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = translate_7;
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(x_scaling, y_scaling, z_scaling));
	modelMatrix = translate_final * rotationMat * shearingMatrix * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// horizontal of "7"
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.60 * y_scaling, x_scaling, x_scaling));
	translate_7_child = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2 * scalingFactor, 0.0f));
	translationMatrix = translate_7_child * translate_7;
	modelMatrix = translate_final * rotationMat * translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void model_A7()
{
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.20f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, -0.05f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.3, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	rotationMatrix = identityMatrix;
	worldMatrix = identityMatrix;


	// end of model A7
}

void model_O9() {
	// beginning of letter O
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.20f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.20f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, -0.20f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, 0.225f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of letter O

	// beginning of number 9
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 0.225f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, -0.025f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, -0.20f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//vertical
	//right
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.42f, -0.20f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, -2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 0.225f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// end of number 9

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object
	modelScalingMatrix = identityMatrix;
	rotationMatrix = identityMatrix;
	worldMatrix = identityMatrix;
	// end of model O9
}

void model_S0() {

	// S
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.3f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.1f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, -0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, -0.3f, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 1.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ZERO
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.3f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	shearingMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -0.3f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// End of model S0

	//Reset 
	modelScalingMatrix = identityMatrix;
	rotationMatrix = identityMatrix;
	worldMatrix = identityMatrix;
}

void model_M6() {


	//draw letter M
		//left upright for letter M
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, -0.2f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right diagonal for letter M
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		-0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	//translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	partMatrix = translationMatrix * shearingMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right upright for letter M
	shearingMatrix = glm::mat4(1.0f);
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, -0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end of M

	//draw 6
		//vertical segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, -0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//horizontal segments
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.2f, 0.0f));
	partMatrix = translationMatrix * scalingMatrix;
	worldMatrix = modelTranslationMatrix * modelScalingMatrix * rotationMatrix * partMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//end 6	

	// reset world matrix, rotation matrix, and model scaling matrix after we're done with it for this object.
	// also reset shearing matrix and scaling matrix, so that they work correctly with model_A7().
	shearingMatrix =
	{
		1.0, 0.0, 0.0, 0.0,
		0.35, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	modelScalingMatrix = identityMatrix;
	rotationMatrix = identityMatrix;
	worldMatrix = identityMatrix;
}
