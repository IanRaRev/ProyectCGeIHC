/*
Semestre 2019-2
Práctica 10: Iluminación 2 Spotlight: Se agrega código para crear spotlights.
Texturizado: se agrega código para transparencia y blending.
Skybox: Se agrega Skybox como textura ligada a la cámara. 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_texturizado.h"

#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture pisoTexture;

//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;

//PROYECTO FINAL
//VARS PROYECTO FINAL
float tazasVarEstructuraGiro = 0.0f;
float tazasVarTazaGiroPos = 0.0f;
float tazasVarTazaGiroNeg = 0.0f;

float sillasVarGiroPos = 0.0f;

//MODELOS PROYECTO FINAL
//Juego de las tazas:
Model Tazas_TazaRoja;
Model Tazas_TazaRosa;
Model Tazas_TazaDorada;
Model Tazas_TazaVerde;
Model Tazas_TazaAzul;
Model Tazas_TazaAzulGris;
Model Tazas_TazaMorada;
Model Tazas_Estructura;
Model Tazas_BaseGiratoria;

//Juego de las sillas:
Model Sillas_BaseMetalica;
Model Sillas_EstructuraGiro;

//Relleno
Model Pruebas;
Model Lampara;
Model Pasto;
Model Kilahuea;
Model Muro;
Model Excusado;
Texture wc;
Model Banca;
Model Carpa;
Model Tree;
Model Carrusel;

/* Skybox */
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
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


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

	Mesh* cuadro = new Mesh();
	cuadro->CreateMesh(cubo_vertices, cubo_indices, 32, 6);
	meshList.push_back(cuadro);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(-5.0f, 0.0f, 40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	/** T E X T U R A S **/;
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/tierra.tga");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado.tga");
	dadoTexture.LoadTextureA();
	pisoTexture = Texture("Textures/plain.png");
	pisoTexture.LoadTextureA();

	/*Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();*/


	/** M A T E R I A L E S **/

	wc = Texture("Textures/wc.png");
	wc.LoadTexture();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	/** M O D E L O S **/
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt.3ds");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	Excusado = Model();
	Excusado.LoadModel("Models/toilet.obj");

	//JUEGOS MECANICOS
	//JUEGO DE LAS TAZAS
	/* Estructura */
	Tazas_Estructura = Model();
	Tazas_Estructura.LoadModel("Models/Tazas-Estructura-obj.obj");
	/* Base giratoria */
	Tazas_BaseGiratoria = Model();
	Tazas_BaseGiratoria.LoadModel("Models/Tazas-BaseGiratoria-obj.obj");
	/* Tazas */
	Tazas_TazaRoja = Model();
	Tazas_TazaRoja.LoadModel("Models/Tazas-TazaRoja-obj.obj");
	Tazas_TazaRosa = Model();
	Tazas_TazaRosa.LoadModel("Models/Tazas-TazaRosa-obj.obj");
	Tazas_TazaDorada = Model();
	Tazas_TazaDorada.LoadModel("Models/Tazas-TazaDorada-obj.obj");
	Tazas_TazaVerde = Model();
	Tazas_TazaVerde.LoadModel("Models/Tazas-TazaVerde-obj.obj");
	Tazas_TazaAzul = Model();
	Tazas_TazaAzul.LoadModel("Models/Tazas-TazaAzul-obj.obj");
	Tazas_TazaMorada = Model();
	Tazas_TazaMorada.LoadModel("Models/Tazas-TazaMorada-obj.obj");
	Tazas_TazaAzulGris = Model();
	Tazas_TazaAzulGris.LoadModel("Models/Tazas-TazaAzulGris-obj.obj");

	//JUEGO DE LAS SILLAS
	//BASE
	Sillas_BaseMetalica = Model();
	Sillas_BaseMetalica.LoadModel("Models/Sillas-BaseMetalica-obj.obj");
	Sillas_EstructuraGiro = Model();
	Sillas_EstructuraGiro.LoadModel("Models/Sillas-EstructuraGiratoria-obj.obj");

	//OBJETOS DE RELLENO
	Lampara = Model();
	Lampara.LoadModel("Models/lamp.obj");
	Pruebas = Model();
	Pruebas.LoadModel("Models/carrusel.obj");
	Pasto = Model();
	Pasto.LoadModel("Models/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
	Kilahuea = Model();
	Kilahuea.LoadModel("Models/DropTower.blend");
	Muro = Model();
	Muro.LoadModel("Models/wall.obj");
	Banca = Model();
	Banca.LoadModel("Models/bench.obj");
	Carpa = Model();
	Carpa.LoadModel("Models/Tent.obj");
	Tree = Model();
	Tree.LoadModel("Models/Tree1.3ds");
	Carrusel = Model();
	Carrusel.LoadModel("Models/carrusel.obj");


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Lampara izquierda entrada
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-19.5f, 3.0f, -10.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//Carpa central
	pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-5.0f, -2.0f, -35.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//Lampara derecha entrada
	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		9.5f, 3.0f, -10.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;



	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;




	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");	//Derecha		EL ORDEN ES MUY IMPORTANTE
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");	//Izquierda
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");	//Abajo
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");	//Arriba
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");	//Atras
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");	//Adelante

	/*skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");*/


	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	movCoche = 0.0f;
	movOffset = 1.0f;
	avanza = 1;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		if (avanza)
		{
			if (movCoche < 10.0f)
			{
				movCoche += movOffset*deltaTime;
				avanza = 1;
			}
			else
			{
				avanza = 0;
			}
		}
		else
		{
			if (movCoche > -10.0f)
			{
				movCoche -= movOffset*deltaTime;
			}
			else
			{
				avanza = 1;
			}
		}



		//Recibir eventos del usuario
		glfwPollEvents();



		camera.keyControl(mainWindow.getKeys(), deltaTime);

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
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);

		//Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 1.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//pisoTexture.UseTexture();
		dirtTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		

		//water.UseTexture();
		
		/*
		//pruebas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, 3.0f, 20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pruebas.RenderModel();
		*/


		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, -1.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kitt_M.RenderModel();
		spotLights[2].SetPos(glm::vec3(movCoche, -1.5f, 0.0f));

		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(2.0f - movCoche, 2.0f, 0.0f);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, -90* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();
		spotLights[3].SetPos(posblackhawk);*/

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.53f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Camino_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.7f, -2.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tagave.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();*/

		//JUEGO DE LAS TAZAS
		//Matriz temporal:
		glm::mat4 modelTempTazas(1.0);
		//ESTRUCTURA:
		model = glm::mat4(1.0);
		modelTempTazas = model = glm::translate(model, glm::vec3(-41.0f, -2.0f, -30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_Estructura.RenderModel();

		//BASE GIRATORIA:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(0.0f, 0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, (tazasVarEstructuraGiro)* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelTempTazas = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_BaseGiratoria.RenderModel();

		/* Giro base */

		//Giro base:
		/*
		tazasVarEstructuraGiro += 60.0f*deltaTime;
		if (tazasVarEstructuraGiro == 360.0f) {
			tazasVarEstructuraGiro = 0.0f;
		}
		*/

		//TAZA ROJA:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(0.35f, 0.08f, 0.4f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, tazasVarTazaGiroNeg * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaRoja.RenderModel();
		// Giro taza positivo:
		if (mainWindow.mueveTazas())
		{
			tazasVarTazaGiroNeg -= 90.0f * deltaTime;
			if (tazasVarTazaGiroNeg == -360.0f) {
				tazasVarTazaGiroNeg = 0.0f;
			}
			tazasVarEstructuraGiro += 60.0f * deltaTime;
			if (tazasVarEstructuraGiro == 360.0f) {
				tazasVarEstructuraGiro = 0.0f;
			}
		}

		/*
		tazasVarTazaGiroNeg -= 90.0f * deltaTime;
		if (tazasVarTazaGiroNeg == -360.0f) {
			tazasVarTazaGiroNeg = 0.0f;
		}
		*/
		//TAZA ROSA:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(2.2f, 0.08f, 2.83f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, tazasVarTazaGiroNeg * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaRosa.RenderModel();

		//TAZA DORADA:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(-2.0f, 0.08f, 2.83f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, tazasVarTazaGiroNeg * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaDorada.RenderModel();

		//TAZA VERDE:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(-3.15f, 0.08f, -0.78f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, (-45 + tazasVarTazaGiroNeg) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaVerde.RenderModel();

		//TAZA AZUL:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(+3.5f, 0.08f, 0.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, (45 + tazasVarTazaGiroNeg) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaAzul.RenderModel();

		//TAZA MORADA:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(2.15f, 0.08f, -2.23f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, (-225 + tazasVarTazaGiroNeg) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaMorada.RenderModel();

		//TAZA GRIS:
		//model = glm::mat4(1.0);
		model = glm::translate(modelTempTazas, glm::vec3(-0.8f, 0.08f, -2.9f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, (-180 + tazasVarTazaGiroNeg) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tazas_TazaAzulGris.RenderModel();

		//JUEGO DE LAS SILLAS
		glm::mat4 modelTempSillas(1.0);
		//BASE GIRATORIA:
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.0f, -2.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelTempSillas = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Sillas_BaseMetalica.RenderModel();


		//ESTRUCTURA GIRATORIA:
		model = glm::translate(modelTempSillas, glm::vec3(0.0f, 0.4f, -1.83f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, sillasVarGiroPos * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Sillas_EstructuraGiro.RenderModel();

		//Relleno
		//Lamparas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.5f, -2.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.5f, -2.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.5f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.5f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.5f, -2.0f, -30.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.5f, -2.0f, -30.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara.RenderModel();

		//Bardas

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(65.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-65.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		/* Barda trasera */
		//Pastos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, -2.0f, -64.8f));
		model = glm::scale(model, glm::vec3(0.0285f, 0.01f, 0.1f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pasto.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(65.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-65.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, -2.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		/* Muro lateral izquierdo */
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, 23.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, 13.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, 3.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -6.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -16.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -26.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -36.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -46.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -56.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -66.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.4f, -2.0f, -76.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		/* Muro lateral derecho */
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -76.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, 23.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, 13.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, 3.6f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -6.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -16.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -26.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -36.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -46.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -56.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(79.4f, -2.0f, -66.4f));
		model = glm::scale(model, glm::vec3(0.85f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Muro.RenderModel();

		//Pastos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, -2.0f, 15.8f));
		model = glm::scale(model, glm::vec3(0.0285f, 0.01f, 0.1f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pasto.RenderModel();

		//Kilahuea
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 25.7f, -60.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kilahuea.RenderModel();

		//Baños
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.0f, -2.0f, -29.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Excusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(29.0f, -2.0f, -29.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Excusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.0f, -2.0f, -29.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Excusado.RenderModel();

		//Baños cubo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(29.0f, -1.0f, -28.0f));
		model = glm::scale(model, glm::vec3(7.0f, 3.0f, 7.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		//Divisiones
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.5f, -1.0f, -29.0f));
		model = glm::scale(model, glm::vec3(5.0f, 3.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.5f, -1.0f, -29.0f));
		model = glm::scale(model, glm::vec3(5.0f, 3.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(29.0f, -1.0f, -23.0f));
		model = glm::scale(model, glm::vec3(7.0f, 3.0f, 7.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		//Letrero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(29.0f, 2.0f, -21.0f));
		model = glm::scale(model, glm::vec3(7.0f, 3.0f, 7.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		wc.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Bancas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.0f, -1.8f, 15.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.0f, -1.8f, -20.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -1.8f, 15.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -1.8f, -20.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();

		//Carpa de circo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, -2.0f, -40.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Carpa.RenderModel();


		//Carrusel
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Carrusel.RenderModel();


		//Arboles
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, -2.0f, 15.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tree.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tree.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, -2.0f, -6.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tree.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -2.0f, -20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tree.RenderModel();



		//Giro:
		if (mainWindow.mueveSillas())
		{
			sillasVarGiroPos += 90.0f * deltaTime;
			if (sillasVarGiroPos == 360.0f) {
				sillasVarGiroPos = 0.0f;
			}
		}
		/*
		//Giro:
		sillasVarGiroPos += 90.0f * deltaTime;
		if (sillasVarGiroPos == 360.0f) {
			sillasVarGiroPos = 0.0f;
		}
		*/

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}