#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

std::vector<GLuint> compiledPrograms;

struct ShaderProgram {

	GLuint vertexShader = 0;
	GLuint geometryShader = 0;
	GLuint fragmentShader = 0;
};

struct GameObject {

	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 rotation = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);

	glm::vec3 forward = glm::vec3(1.f, 0.f, 0.f);
	float fVelocity = 0.001f;
	float fAngularVelocity = 0.2f;

	float scaleFactor = 1.f;
};

void Resize_Window(GLFWwindow* window, int iFrameBufferWidth, int iFrameBufferHeight) {

	//Definir nuevo tama�o del viewport
	glViewport(0, 0, iFrameBufferWidth, iFrameBufferHeight);

	//static cast generado con IA, 
	glUniform2f(glGetUniformLocation(compiledPrograms[0], "windowSize"), iFrameBufferWidth, iFrameBufferHeight);
	
}

// Funcion que genera una matriz de traslacion representada por un vector
glm::mat4 GenerateTranslationMatrix(glm::vec3 translation) {

	return glm::translate(glm::mat4(1.0f), translation);
}

//funcion para generar matrices de rotacion
glm::mat4 GenerateRotationMatrix(glm::vec3 axis, float fDegrees) {
	return glm::rotate(glm::mat4(1.f), glm::radians(fDegrees), glm::normalize(axis));

}

//funcion que genera una matriz de escalado representada por un vector
glm::mat4 GenerateScaleMatrix(glm::vec3 scale) {
	return glm::scale(glm::mat4(1.0f), scale);
}

//Funcion que devolvera una string con todo el archivo leido
std::string Load_File(const std::string& filePath) {

	std::ifstream file(filePath);

	std::string fileContent;
	std::string line;
	
	//Lanzamos error si el archivo no se ha podido abrir
	if (!file.is_open()) {
		std::cerr << "No se ha podido abrir el archivo: " << filePath << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//Leemos el contenido y lo volcamos a la variable auxiliar
	while (std::getline(file, line)) {
		fileContent += line + "\n";
	}

	//Cerramos stream de datos y devolvemos contenido
	file.close();

	return fileContent;
}

GLuint LoadFragmentShader(const std::string& filePath) {

	// Crear un fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Usamos la funcion creada para leer el fragment shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el fragment shader con su c�digo fuente
	glShaderSource(fragmentShader, 1, &cShaderSource, nullptr);

	// Compilar el fragment shader
	glCompileShader(fragmentShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el fragment shader
	if (success) {

		return fragmentShader;

	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(fragmentShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el fragment shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

GLuint LoadGeometryShader(const std::string& filePath) {

	// Crear un vertex shader
	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

	//Usamos la funcion creada para leer el vertex shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el vertex shader con su c�digo fuente
	glShaderSource(geometryShader, 1, &cShaderSource, nullptr);

	// Compilar el vertex shader
	glCompileShader(geometryShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el vertex shader
	if (success) {

		return geometryShader;
	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(geometryShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el vertex shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

GLuint LoadVertexShader(const std::string& filePath) {

	// Crear un vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Usamos la funcion creada para leer el vertex shader y almacenarlo 
	std::string sShaderCode = Load_File(filePath);
	const char* cShaderSource = sShaderCode.c_str();

	//Vinculamos el vertex shader con su c�digo fuente
	glShaderSource(vertexShader, 1, &cShaderSource, nullptr);

	// Compilar el vertex shader
	glCompileShader(vertexShader);

	// Verificar errores de compilaci�n
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	//Si la compilacion ha sido exitosa devolvemos el vertex shader
	if (success) {

		return vertexShader;

	}else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

		//Obtenemos el log
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(vertexShader, logLength, nullptr, errorLog.data());

		//Mostramos el log y finalizamos programa
		std::cerr << "Se ha producido un error al cargar el vertex shader:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

//Funci�n que dado un struct que contiene los shaders de un programa generara el programa entero de la GPU
GLuint CreateProgram(const ShaderProgram& shaders) {

	//Crear programa de la GPU
	GLuint program = glCreateProgram();

	//Verificar que existe un vertex shader y adjuntarlo al programa
	if (shaders.vertexShader != 0) {
		glAttachShader(program, shaders.vertexShader);
	}

	if (shaders.geometryShader != 0) {
		glAttachShader(program, shaders.geometryShader);
	}

	if (shaders.fragmentShader != 0) {
		glAttachShader(program, shaders.fragmentShader);
	}

	// Linkear el programa
	glLinkProgram(program);

	//Obtener estado del programa
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	//Devolver programa si todo es correcto o mostrar log en caso de error
	if (success) {

		//Liberamos recursos
		if (shaders.vertexShader != 0) {
			glDetachShader(program, shaders.vertexShader);
		}

		//Liberamos recursos
		if (shaders.geometryShader != 0) {
			glDetachShader(program, shaders.geometryShader);
		}

		//Liberamos recursos
		if (shaders.fragmentShader != 0) {
			glDetachShader(program, shaders.fragmentShader);
		}

		return program;
	}
	else {

		//Obtenemos longitud del log
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		//Almacenamos log
		std::vector<GLchar> errorLog(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, errorLog.data());

		std::cerr << "Error al linkar el programa:  " << errorLog.data() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void main(){

	//Definir semillas del rand seg�n el tiempo
	srand(static_cast<unsigned int>(time(NULL)));

	//Inicializamos GLFW para gestionar ventanas e inputs
	glfwInit();

	//Configuramos la ventana
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//Inicializamos la ventana
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Engine", NULL, NULL);

	//Asignamos funci�n de callback para cuando el frame buffer es modificado
	glfwSetFramebufferSizeCallback(window, Resize_Window);

	//Definimos espacio de trabajo
	glfwMakeContextCurrent(window);

	//Permitimos a GLEW usar funcionalidades experimentales
	glewExperimental = GL_TRUE;

	//Activamos cull face
	glEnable(GL_CULL_FACE);

	//Indicamos lado del culling
	glCullFace(GL_BACK);	

	//Inicializamos GLEW y controlamos errores
	if (glewInit() == GLEW_OK) {

		//Declarar vec2 para definir el offset
		glm::vec2 offset = glm::vec2(0.f, 0.f);

		//Declaro instancia del cubo
		GameObject cube;

		//vaores iniciales del cubo
		cube.position = glm::vec3(-0.6f, 0.f, 0.f);
		cube.forward = glm::vec3(0.f, 1.f, 0.f);
		cube.fVelocity = 0.001f;
		cube.fAngularVelocity = 0.1f;

		//Declarar el ortoedro
		GameObject ortoedro;

		//Setear variables del ortoedro
		ortoedro.scale = glm::vec3(0.3f);
		ortoedro.scaleFactor = -1.f;
		ortoedro.fAngularVelocity = 0.1f;

		//Declaro instancia dela piramide
		GameObject pyramid;

		//valores iniciales de la piramide
		pyramid.position = glm::vec3(0.75f, 0.f, 0.f);
		pyramid.forward = glm::vec3(0.f, 1.f, 0.f);
		pyramid.fVelocity = 0.001f;
		pyramid.fAngularVelocity = 0.1f;

		//Compilar shaders
		ShaderProgram myFirstProgram;
		myFirstProgram.vertexShader = LoadVertexShader("MyFirstVertexShader.glsl");
		myFirstProgram.geometryShader = LoadGeometryShader("MyFirstGeometryShader.glsl");
		myFirstProgram.fragmentShader = LoadFragmentShader("MyFirstFragmentShader.glsl");

		//Compilar programa
		compiledPrograms.push_back(CreateProgram(myFirstProgram));

		//Definimos color para limpiar el buffer de color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		GLuint vaoCube, vboCube, vaoPyramid, vboPyramid, vaoOrtoedro, vboOrtoedro;;

		//Definimos cantidad de vao a crear y donde almacenarlos 
		glGenVertexArrays(1, &vaoCube);		

		//Indico que el VAO activo de la GPU es el que acabo de crear
		glBindVertexArray(vaoCube);		

		//Definimos cantidad de vbo a crear y donde almacenarlos
		glGenBuffers(1, &vboCube);		

		//Indico que el VBO activo es el que acabo de crear y que almacenar� un array. Todos los VBO que genere se asignaran al �ltimo VAO que he hecho glBindVertexArray
		glBindBuffer(GL_ARRAY_BUFFER, vboCube);		
		
		//Posici�n X e Y del cubo
		GLfloat cubePunto[] = {
			-0.5f, +0.5f, -0.5f, // 3
			+0.5f, +0.5f, -0.5f, // 2
			-0.5f, -0.5f, -0.5f, // 6
			+0.5f, -0.5f, -0.5f, // 7
			+0.5f, -0.5f, +0.5f, // 4
			+0.5f, +0.5f, -0.5f, // 2
			+0.5f, +0.5f, +0.5f, // 0
			-0.5f, +0.5f, -0.5f, // 3
			-0.5f, +0.5f, +0.5f, // 1
			-0.5f, -0.5f, -0.5f, // 6
			-0.5f, -0.5f, +0.5f, // 5
			+0.5f, -0.5f, +0.5f, // 4
			-0.5f, +0.5f, +0.5f, // 1
			+0.5f, +0.5f, +0.5f  // 0
		};

		//Ponemos los valores en el VBO creado
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubePunto), cubePunto, GL_STATIC_DRAW);

		//Indicamos donde almacenar y como esta distribuida la informaci�n
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//Indicamos que la tarjeta gr�fica puede usar el atributo 0
		glEnableVertexAttribArray(0);

		//Desvinculamos VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Desvinculamos VAO
		glBindVertexArray(0);

		//pyramid
		//Definimos cantidad de vao a crear y donde almacenarlos 
		glGenVertexArrays(1, &vaoPyramid);

		//Indico que el VAO activo de la GPU es el que acabo de crear
		glBindVertexArray(vaoPyramid);

		//Definimos cantidad de vbo a crear y donde almacenarlos
		glGenBuffers(1, &vboPyramid);

		//Indico que el VBO activo es el que acabo de crear y que almacenar� un array. Todos los VBO que genere se asignaran al �ltimo VAO que he hecho glBindVertexArray
		glBindBuffer(GL_ARRAY_BUFFER, vboPyramid);

		//Posici�n X e Y de la piramide
		GLfloat pyramidPunto[] = {

			// Cara frontal
			+0.0f, +0.5f, +0.0f,
			-0.5f, -0.5f, +0.5f,
			+0.5f, -0.5f, +0.5f,

			// Cara derecha
			+0.0f, +0.5f, +0.0f,
			+0.5f, -0.5f, +0.5f,
			+0.5f, -0.5f, -0.5f,

			// Cara trasera
			+0.0f, +0.5f, +0.0f,
			+0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			// Cara izquierda
			+0.0f, +0.5f, +0.0f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, +0.5f,

			// Base de la piramide - triangulo 1
			-0.5f, -0.5f, +0.5f,
			-0.5f, -0.5f, -0.5f,
			+0.5f, -0.5f, -0.5f,

			// Base de la piramide - triangulo 2
			-0.5f, -0.5f, +0.5f,
			+0.5f, -0.5f, -0.5f,
			+0.5f, -0.5f, +0.5f
		};

		//Ponemos los valores en el VBO creado
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPunto), pyramidPunto, GL_STATIC_DRAW);

		//Indicamos donde almacenar y como esta distribuida la informaci�n
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//Indicamos que la tarjeta gr�fica puede usar el atributo 0
		glEnableVertexAttribArray(0);

		//Desvinculamos VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Desvinculamos VAO
		glBindVertexArray(0);

		/*
		CONFIGURACION DEL ORTOEDRO
		*/

		//Crear VAO para el ortoedro
		glGenVertexArrays(1, &vaoOrtoedro);
		glBindVertexArray(vaoOrtoedro);

		//Crear VBO para el ortoedro
		glGenBuffers(1, &vboOrtoedro);
		glBindBuffer(GL_ARRAY_BUFFER, vboOrtoedro);

		//Vertices ortoedro
		GLfloat verticesOrtoedro[] = {
			-0.5f, +1.f, -0.5f, // 3
			+0.5f, +1.f, -0.5f, // 2
			-0.5f, -1.f, -0.5f, // 6
			+0.5f, -1.f, -0.5f, // 7
			+0.5f, -1.f, +0.5f, // 4
			+0.5f, +1.f, -0.5f, // 2
			+0.5f, +1.f, +0.5f, // 0
			-0.5f, +1.f, -0.5f, // 3
			-0.5f, +1.f, +0.5f, // 1
			-0.5f, -1.f, -0.5f, // 6
			-0.5f, -1.f, +0.5f, // 5
			+0.5f, -1.f, +0.5f, // 4
			-0.5f, +1.f, +0.5f, // 1
			+0.5f, +1.f, +0.5f  // 0
		};

		//Ponemos los valores en el VBO creado
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesOrtoedro), verticesOrtoedro, GL_STATIC_DRAW);

		//Indicamos donde almacenar y como esta distribuida la informaci�n
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//Indicamos que la tarjeta gr�fica puede usar el atributo 0
		glEnableVertexAttribArray(0);

		//Desvinculamos VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Desvinculamos VAO
		glBindVertexArray(0);


		//Definimos modo de dibujo para cada cara
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Indicar a la tarjeta GPU que programa debe usar
		glUseProgram(compiledPrograms[0]);

		//Asignar valores iniciales al programa
		glUniform2f(glGetUniformLocation(compiledPrograms[0], "windowSize"), WINDOW_WIDTH, WINDOW_HEIGHT);

		//Generamos el game loop
		while (!glfwWindowShouldClose(window)) {

			//Pulleamos los eventos (botones, teclas, mouse...)
			glfwPollEvents();

			//Limpiamos los buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Generar el modelo de la matriz MVP
			glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
			glm::mat4 pyramidModelMatrix = glm::mat4(1.0f);
			glm::mat4 ortoedroModelMatrix = glm::mat4(1.0f);


			//Calculamos la nueva posicion del cubo
			cube.position = cube.position + cube.forward * cube.fVelocity;
			cube.rotation = cube.rotation + glm::vec3(1.f, 1.f, 0.f) * cube.fAngularVelocity;

			//invertimos direccion si se sale de los limites
			if (cube.position.y >= 0.75 || cube.position.y <= -0.75f) {
				cube.forward = cube.forward * -1.f;
			}

			//Calculamos la nueva posicion de la piramide
			pyramid.position = pyramid.position + pyramid.forward * pyramid.fVelocity;
			pyramid.rotation = pyramid.rotation + glm::vec3(1.f, 1.f, 0.f) * pyramid.fAngularVelocity;

			//invertimos direccion si se sale de los limites
			if (pyramid.position.y >= 0.75 || pyramid.position.y <= -0.75f) {
				pyramid.forward = pyramid.forward * -1.f;
			}

			//Genero matriz de traslacion
			glm::mat4 cubeTranslationMatrix = GenerateTranslationMatrix(cube.position);
			
			//Geneamos matriz de rotacion
			glm::mat4 cubeRotationMatrix = GenerateRotationMatrix(glm::vec3(0.f, 1.f, 0.f), cube.rotation.y);

			//Escalamos el cubo
			glm::mat4 cubeScaleMatrix = GenerateScaleMatrix(glm::vec3(0.35f, 0.35f, 0.35f));

			// Aplicamos matriz
			cubeModelMatrix = cubeTranslationMatrix * cubeRotationMatrix * cubeScaleMatrix;

			/*
			DIBUJAR ORTOEDRO
			*/

			//Actualizamos la rotaci�n en Z
			ortoedro.rotation.z = ortoedro.rotation.z + ortoedro.fAngularVelocity;

			//Actualizamos escalado
			ortoedro.scale.y += 0.001f * ortoedro.scaleFactor;

			if (ortoedro.scale.y <= 0.1f) {
				ortoedro.scaleFactor = 1.f;
			}
			else if (ortoedro.scale.y >= 0.5f) {
				ortoedro.scaleFactor = -1.f;
			}

			//Genero matriz de traslacion
			glm::mat4 ortoedroTranslationMatrix = GenerateTranslationMatrix(ortoedro.position);

			//Geneamos matriz de rotacion
			glm::mat4 ortoedroRotationMatrix = GenerateRotationMatrix(glm::vec3(0.f, 0.f, 1.f), ortoedro.rotation.z);

			//Geneamos matriz de escalado
			glm::mat4 ortoedroScaleMatrix = GenerateScaleMatrix(ortoedro.scale);

			// Aplicamos matriz
			ortoedroModelMatrix = ortoedroTranslationMatrix * ortoedroRotationMatrix * ortoedroScaleMatrix;

			//Genero matriz de traslacion
			glm::mat4 pyramidTranslationMatrix = GenerateTranslationMatrix(pyramid.position);

			//Geneamos matriz de rotacion
			glm::mat4 pyramidRotationXMatrix = GenerateRotationMatrix(glm::vec3(1.f, 0.f, 0.f), pyramid.rotation.x);
			glm::mat4 pyramidRotationYMatrix = GenerateRotationMatrix(glm::vec3(0.f, 1.f, 0.f), pyramid.rotation.y);			

			//Escalamos
			glm::mat4 pyramidScaleMatrix = GenerateScaleMatrix(glm::vec3(0.35f, 0.35f, 0.35f));

			// Aplicamos matriz
			pyramidModelMatrix = pyramidTranslationMatrix * pyramidRotationYMatrix * pyramidRotationXMatrix * pyramidScaleMatrix;

			// Calculamos el color de la piramide cada dos segundos
			int pyramidColorIndex = static_cast<int>(glfwGetTime() / 2.0) % 3; //Linea implementada con IA

			glm::vec3 pyramidColor = glm::vec3(1.f, 0.f, 0.f);

			if (pyramidColorIndex == 0) {
				pyramidColor = glm::vec3(1.f, 0.f, 0.f); // Rojo
			}
			else if (pyramidColorIndex == 1) {
				pyramidColor = glm::vec3(0.f, 1.f, 0.f); // Verde
			}
			else {
				pyramidColor = glm::vec3(0.f, 0.f, 1.f); // Azul
			}

			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "transform"), 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

			// Indicamos el color
			glUniform1i(glGetUniformLocation(compiledPrograms[0], "colorMode"), 0);

			//Usamos VAO del cubo
			glBindVertexArray(vaoCube);

			// Definimos que queremos dibujar
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "transform"), 1, GL_FALSE, glm::value_ptr(ortoedroModelMatrix));

			//Definimos que queremos usar el VAO del ortoedro
			glBindVertexArray(vaoOrtoedro);

			//Dibujamos el ortoedro
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

			//Dejamos de usar el VAO indicado anteriormente
			glBindVertexArray(0);

			//matriz piramide
			glUniformMatrix4fv(glGetUniformLocation(compiledPrograms[0], "transform"), 1, GL_FALSE, glm::value_ptr(pyramidModelMatrix));

			// Indicamos que la piramide usa color solido
			glUniform1i(glGetUniformLocation(compiledPrograms[0], "colorMode"), 1);

			// Enviamos el color actual de la piramide
			glUniform3f(glGetUniformLocation(compiledPrograms[0], "solidColor"), pyramidColor.x, pyramidColor.y,pyramidColor.z);

			glBindVertexArray(vaoPyramid);

			//Definimos que queremos dibujar
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 18);
			
			//Dejamos de usar el VAO indicado anteriormente
			glBindVertexArray(0);

			//Cambiamos buffers
			glFlush();
			glfwSwapBuffers(window);
		}

		//Desactivar y eliminar programa
		glUseProgram(0);
		glDeleteProgram(compiledPrograms[0]);

	}else {
		std::cout << "Ha petao." << std::endl;
		glfwTerminate();
	}

	//Finalizamos GLFW
	glfwTerminate();

}