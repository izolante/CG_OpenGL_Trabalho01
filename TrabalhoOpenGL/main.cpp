
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <GL/glew.h>	//GLEW
#include <GLFW/glfw3.h> //GLFW

#include "Scene.h"
#include "Screenshot.h"
#include "OBJ.h"

using namespace std;
using namespace Eigen;

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define ERROR()                       \
	{                                 \
		std::set_terminate([]() { std::cout << "Erro na linha " <<  __LINE__ <<  " do arquivo " << __FILE__  << std::endl; std::abort(); }); \
		std::terminate();             \
	}

// GLSL 3.3 -> MAJOR.MINOR
#define GLSL_MAJOR_VERSION 3
#define GLSL_MINOR_VERSION 3

// funções
Vector3f randColor();
GLFWwindow *createWindow(int winWidth, int winHeight, string winTitle, bool fullscreen = false);
void destroyWindow(GLFWwindow *win);

// evento de teclado (o evento é disparado cada vez que usuário pressiona uma tecla)
// ideal para ativar ou desavivar uma funcionalidade do sistema via teclado
void keyEvent(GLFWwindow *win, int key, int scancode, int action, int mods);

// evento de teclado (o evento é disparado a cada passada do loop de renderização)
// ideal para controlar a movimentação da câmera
void renderKeyEvent(GLFWwindow *win);

// vento do mouse
void mouseEvent(GLFWwindow *win, double x, double y);

// GUI
void initImGUI(GLFWwindow *win);
void destroyGUI();
void renderGUI(GLFWwindow *win);

void createCube(vector<Vector3f> &vertexPos);
void createColoredCube(vector<Vector3f> &vertexPos, vector<Vector3f> &vertexColor);
void createColoredPyramid(vector<Vector3f> &vertexPos, vector<Vector3f> &vertexColor);

int main(void)
{
	srand(time(NULL));

	Scene scene;
	GLFWwindow *window = nullptr;

	//--------JANELA--------

	window = createWindow(1024, 768, "Trabalho CG - Bianca e Eduardo"); // inicializa as bibliotecas GLEW e GLFW e cria a janela

	//--------SHADERS--------
	// uma cena pode ter mais de um programa (shaders).
	GLuint defaltProgram = Shader::createProgram("shaders/shader.vs", "shaders/shader.fs"); // compila os shaders, gerando um programa
	if (!defaltProgram)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		ERROR(); // termina a execução do programa
	}

	GLuint vertexColorProgram = Shader::createProgram("shaders/vertexColor.vs", "shaders/vertexColor.fs"); // compila os shaders, gerando um programa
	if (!vertexColorProgram)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		ERROR(); // termina a execução do programa
	}

	GLuint vertexTextureProgram = Shader::createProgram("shaders/vertexTexture.vs", "shaders/vertexTexture.fs"); // compila os shaders, gerando um programa
	if (!vertexTextureProgram)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		ERROR(); // termina a execução do programa
	}

	//--------CAMERA--------
	//(vFOV, winWidth, winHeight, zNear, zFar, cameraPos);
	float vFOV = Math::degree2rad(60.0f);  // 60 graus de angulo de visão
	float zNear = 0.1f;					   // distancia entr e o centro de projeção (posição da câmera) e do plano de projeção
	float zFar = 500.0f;				   // distancia do plano de corte mais afastado
	Vector3f cameraPos(0.0f, 3.0f, 10.0f); // posicação da câmera
	int winWidth, winHeight;			   // largura e altura da janela
	glfwGetWindowSize(window, &winWidth, &winHeight);

	scene.camera = new Camera(winWidth, winHeight, vFOV, zNear, zFar, cameraPos); // cria a câmera

	//--------OBJETOS--------
	vector<Vector3f> vertexPos, vertexColor; // coordenadas dos vértices de um objeto (um objeto pode ter mais de um triângulo)
	vector<Vector2f> vertexTexture;

	// OBJ1
	Object obj1; // primeiro objeto ->  possui o atributo posição para CADA vértice
	createColoredPyramid(vertexPos, vertexColor);
	obj1.create("piramide", vertexPos, vertexColor);
	obj1.setPosition(Vector3f(0.0f, 4.0f, 0.0f));
	obj1.setPosition(Vector3f(5, 4, 0));
	scene.objects.push_back(obj1);

	if (OBJ::load("../OBJs/cone.obj", &vertexPos))
	{
		Object obj;
		obj.create("cone", vertexPos); // vetor de cor=NULL
		obj.setPosition(Vector3f(-3.0f, 5.0f, 0.0f));
		obj.setColor(Vector3f(0, 0.7, 0));
		scene.objects.push_back(obj);
	}

	if (OBJ::load("../OBJs/plano.obj", &vertexPos, &vertexTexture))
	{
		Object obj;
		obj.create("chao", vertexPos, vector<Vector3f>(), vertexTexture); // vetor de cor=NULL
		obj.setTexture("../texturas/chao.jpg");
		obj.setPosition(Vector3f(0.0f, 0.0f, 0.0f));
		scene.objects.push_back(obj);
	}
	if (OBJ::load("../OBJs/caixa.obj", &vertexPos, &vertexTexture))
	{
		Object obj;
		obj.create("caixa", vertexPos, vector<Vector3f>(), vertexTexture); // vetor de cor=NULL
		obj.setTexture("../texturas/caixa.jpg");
		obj.setPosition(Vector3f(3.0f, 1.5f, 0.0f));
		scene.objects.push_back(obj);
	}
	if (OBJ::load("../OBJs/planeta.obj", &vertexPos, &vertexTexture))
	{
		Object obj;
		obj.create("planeta", vertexPos, vector<Vector3f>(), vertexTexture); // vetor de cor=NULL
		obj.setTexture("../texturas/2k_earth_daymap.jpg");
		obj.setPosition(Vector3f(0.0f, 7.0f, 0.0f));
		scene.objects.push_back(obj);
	}
	if (OBJ::load("../OBJs/torus.obj", &vertexPos, &vertexTexture))
	{
		Object obj;
		obj.create("torus", vertexPos, vector<Vector3f>(), vertexTexture); // vetor de cor=NULL
		obj.setTexture("../texturas/tijolo.png");
		obj.setPosition(Vector3f(3.0f, 7.0f, 0.0f));
		scene.objects.push_back(obj);
	}

	if (OBJ::load("../OBJs/tambor.obj", &vertexPos, &vertexTexture))
	{
		Object obj;
		obj.create("tambor_shell", vertexPos, vector<Vector3f>(), vertexTexture); // vetor de cor=NULL
		obj.setTexture("../texturas/tambor2.png");
		obj.setPosition(Vector3f(0.0f, 1.5f, 0.0f));
		scene.objects.push_back(obj);
	}

	if (OBJ::load("../OBJs/torus.obj", &vertexPos, &vertexTexture))
	{
		Object obj;

		obj.create("torus_verde", vertexPos); /// vetor de cor=NULL; vetor de textura=NULL
		obj.setPosition(Vector3f(-9.0f, 4.0f, -10.0f));
		obj.setScale(Vector3f(3.3f, 3.3f, 3.3f));
		obj.setColor(Vector3f(0.45f, 0.85f, 0.43f));
		scene.objects.push_back(obj);
	}

	//--------EVENTOS--------
	glfwSetKeyCallback(window, keyEvent);		  // registra o evento de teclado para função keyEvent
	glfwSetCursorPosCallback(window, mouseEvent); // registra o evento de teclado para função keyEvent
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //habilitar / desalibilitar o cursor do mouse

	//--------REFERENCIA GLOBAL DA CENA--------
	// Passar o ponteiro da cena para a biblioteca GLFW.
	// Assim, pode-se ter acesso a cena criada em qualquer lugar do código, usando o método "glfwGetWindowUserPointer".
	// Este recurso é útil quando ocorre um evento de teclado ou mouse e precisamos ter acesso a um dado da cena
	glfwSetWindowUserPointer(window, &scene);

	//--------CONFIGURACOES DO OPENGL--------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo da tela - > RGBA

	// ativar o teste de profundidade para cada fragmento processado dentro do fragment shader
	// antes de cada fragmento ser desenhado é realizada uma comparação com o valor de profundidade
	glEnable(GL_DEPTH_TEST);

	// define qual algoritmo sera usado para comparar os valores de profundidade dentro do fragment shader
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthFunc.xhtml
	// glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_LESS);

	//--------INICIALIZAR GUI--------
	initImGUI(window);

	//--------LOOP DE RENDERIZACAO--------
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) // a cada iteração, um quadro é renderizado!
	{

		// Limpa os buffers que armazenam os valores de pronfundidade de cada fragmento para seu valor inicial (1.0)
		// Caso isso não seja feito, os fragmentos de novos triângulos serão comparados com os fragmentos renderizados do quadro anterior!
		glClear(GL_DEPTH_BUFFER_BIT);

		// Limpa os buffers que armazenam os valores de cor
		glClear(GL_COLOR_BUFFER_BIT);

		// especifica o uso dos shaders previamente compilados (vertex shader & fragment shader)
		// os shaders compilados foram anexados a um ponteiro denominado programa

		// wireframe
		if (scene.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// renderizar os objetos
		for (size_t i = 0; i < scene.objects.size(); i++)
		{

			// define qual shader será usado para renderizar o objeto!
			GLuint program;
			if (scene.objects[i].hasVertexTextureAtrrib())
				program = vertexTextureProgram;
			else if (scene.objects[i].hasVertexColorAtrrib())
				program = vertexColorProgram;
			else
				program = defaltProgram;

			glUseProgram(program);

			if (!scene.objects[i].hasVertexColorAtrrib())
			{
				GLuint uniformRef = glGetUniformLocation(program, "colorIn");
				glUniform3fv(uniformRef, 1, scene.objects[i].getColor().data());
			}

			GLuint uniformRef;
			uniformRef = glGetUniformLocation(program, "projectionMat");
			glUniformMatrix4fv(uniformRef, 1, GL_FALSE, scene.camera->getProjectionMatrix().data());

			uniformRef = glGetUniformLocation(program, "viewMat");
			glUniformMatrix4fv(uniformRef, 1, GL_FALSE, scene.camera->getViewMatrix().data());

			if (scene.animation == true && scene.objects[i].getName() == "piramide")
			{
				float rY = 0.001f;
				scene.objects[i].setRMatrix(Math::yRotationMat(rY) * scene.objects[i].getRMatrix());
			}

			// antes de renderizar o objeto, definimos a matriz de transformação geométrica do objeto (matriz model)
			Matrix4f model = scene.objects[i].getTMatrix() * scene.objects[i].getRMatrix() * scene.objects[i].getSMatrix(); // matriz de transformação geométrica do modelo 3D

			// passamos a matriz para o vertexshader
			uniformRef = glGetUniformLocation(program, "modelMat");
			glUniformMatrix4fv(uniformRef, 1, GL_FALSE, model.data());

			scene.objects[i].render(); // renderiza o objeto
		}

		// processa os eventos de teclado e mouse
		renderKeyEvent(window);

		glfwPollEvents();

		renderGUI(window); // renderiza a GUI

		// atualiza a janela de visualização com o quadro renderizado
		glfwSwapBuffers(window);

	} // fim while renderizacao

	//--------DESALOCA MEMORIA--------
	// remove os shaders
	if (defaltProgram)
		glDeleteProgram(defaltProgram);

	if (vertexColorProgram)
		glDeleteProgram(vertexColorProgram);

	if (vertexTextureProgram)
		glDeleteProgram(vertexTextureProgram);

	scene.destroy();
	destroyWindow(window);

	destroyGUI();
	scene.destroy();
	destroyWindow(window);

	return EXIT_SUCCESS;

} // fim main

void renderGUI(GLFWwindow *win)
{

	// verifica se imGUI foi inicializada
	if (!ImGui::GetCurrentContext())
		return;

	// pega instância da cena
	Scene *scene = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(win));

	// PASSO 1 - Cria-se um frame -> ÚNICO FRAME (mesmo que varias janelas sejam criadas)
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame(); // novo frame

	// PASSO 2 - opcoes de configuração da GUI
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	// PASSO 3 - Criar a GUI
	// criar uma janela, pode-se criar mais que uma...
	ImGui::Begin("Cena", nullptr, window_flags);
	{
		ImGui::SetWindowPos(ImVec2(0, 0)); // posicao

		// ImGui::Text("Exemplo GUI");

		// ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); //texto normal
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate); // texto com cor

		// criar um Checkbox e definir o evento de clique
		ImGui::Checkbox("Ativar wireframe", &scene->wireframe);

		ImGui::SliderInt("Velocidade ", &scene->velocidade, 1, 20);

		ImGui::SliderInt("Velocidade escala", &scene->velocidadeScala, 1, 10);

		// ImGui::SliderInt("Velocidade Rotação", &scene->velocidadeRotacao, 1, 10);

		// OBJETOS
		const char *labels[] = {"Piramide", "Cone", "Chao", "caixa", "planeta", "torus", "tambor", "torus_verde"};
		ImGui::Combo("Objeto", &scene->objetoSelecionado, labels, IM_ARRAYSIZE(labels));

		//ImGui::Separator();
		ImGui::Text("\n");
		ImGui::RadioButton("T", &scene->trans, 0);
		ImGui::SameLine();
		ImGui::RadioButton("R", &scene->trans, 1);
		ImGui::SameLine();
		ImGui::RadioButton("S", &scene->trans, 2);
		ImGui::SameLine();
		ImGui::Separator();

		// BOTOES PARA TRANSFORMAÇÕES NOS EIXOS
		ImGui::RadioButton("Eixo X", &scene->eixo, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Eixo Y", &scene->eixo, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Eixo Z", &scene->eixo, 2);
		ImGui::SameLine();

		if(scene->trans == 2){
			ImGui::RadioButton("Eixo X,Y,Z", &scene->eixo, 3);
			ImGui::SameLine();
		}
		// ImGui::RadioButton("Eixo X,Y,Z", &scene->eixo, 3);

		
		ImGui::Text("\n");
		// criar um botao e definir o evento de clique
		if (ImGui::Button("Screenshot")) // se entrar no IF, botão foi pressionado
		{
			int w, h;
			glfwGetWindowSize(win, &w, &h); // pega o tamanho da tela
			Screenshot::saveScreenshot(w, h);
		}

		ImGui::Checkbox("Ativar animação", &scene->animation);
		ImGui::Text("\n\n");
	}
	ImGui::End();

	// PASSO 4 - renderizar GUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

} // fim renderGUI

void mouseEvent(GLFWwindow *win, double x, double y)
{
}
void renderKeyEvent(GLFWwindow *win)
{
	Scene *scene = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(win));
	float fator = Math::normalize(scene->velocidade, 1, 20, 0.001f, 0.1f);

	float fatorScala = Math::normalize(scene->velocidadeScala, 1, 10, 1.0002, 1.002);

	if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
		fator = abs(fator);

	else if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		fatorScala = 1 / fatorScala;
		fator *= -1.0f;
	}
	else
		return;

	Object &obj = scene->objects[scene->objetoSelecionado];

	Matrix4f m;
	if (scene->trans == 0) // translação
	{
		if(scene->eixo == 3)
			scene->eixo = 2;

		if (scene->eixo == 0)
		{
			m = Math::translationMat(Vector3f(fator, 0, 0));
		}
		else if (scene->eixo == 1)
		{
			m = Math::translationMat(Vector3f(0, fator, 0));
		}
		else if (scene->eixo == 2)
		{
			fator *= -1;
			m = Math::translationMat(Vector3f(0, 0, fator));
		}
		obj.setTMatrix(m * obj.getTMatrix());
	}
	else if (scene->trans == 1) // rotação
	{
		if(scene->eixo == 3)
			scene->eixo = 2;

		Object &obj = scene->objects[scene->objetoSelecionado];
		Matrix4f m;
		if (scene->eixo == 0)
		{
			m = Math::xRotationMat(fator);
		}
		else if (scene->eixo == 1)
		{
			m = Math::yRotationMat(fator);
		}
		else if (scene->eixo == 2)
		{
			m = Math::zRotationMat(fator);
		}
		obj.setRMatrix(m * obj.getRMatrix());
	}
	else if (scene->trans == 2) // escala
	{

		if (scene->eixo == 0)
		{
			m = Math::scaleMat(Vector3f(fatorScala, 1, 1));
		}
		else if (scene->eixo == 1)
		{
			m = Math::scaleMat(Vector3f(1, fatorScala, 1));
		}
		else if (scene->eixo == 2)
		{
			m = Math::scaleMat(Vector3f(1, 1, fatorScala));
		}
		else if (scene->eixo == 3)
		{
			m = Math::scaleMat(Vector3f(fatorScala, fatorScala, fatorScala));

		}
		obj.setSMatrix(m * obj.getSMatrix());
	}
}

void keyEvent(GLFWwindow *win, int key, int scancode, int action, int mods)
{

	Scene *scene = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(win));
}

void destroyWindow(GLFWwindow *win)
{
	// remove a janela
	glfwDestroyWindow(win);
	glfwTerminate();
}

GLFWwindow *createWindow(int winWidth, int winHeight, string winTitle, bool fullscreen)
{

	GLFWwindow *window = nullptr;

	// inicializar  a biblioteca GLFW
	if (!glfwInit())
		ERROR(); // termina a execução do programa

	// define que a versão minímina necessária do OPENGL é 3.3 ou superior!
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLSL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLSL_MINOR_VERSION);

	// não permite redimensionar a janela
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (fullscreen)
	{
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		// pega informacoes do monitor
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		// cout << mode->redBits << endl;
		// cout << mode->greenBits << endl;
		// cout << mode->blueBits << endl;
		// cout << mode->refreshRate << endl << endl;

		// cria a janela
		window = glfwCreateWindow(mode->width, mode->height, winTitle.c_str(), glfwGetPrimaryMonitor(), NULL); // tela cheia
	}
	else
	{
		window = glfwCreateWindow(winWidth, winHeight, winTitle.c_str(), NULL, NULL); // janela
	}

	// veririca se a janela foi criada com sucesso
	if (!window)
	{
		glfwTerminate();
		ERROR(); // termina a execução do programa
	}

	// cria o contexto de renderização
	glfwMakeContextCurrent(window);

	// iniciliza a biblioteca GleW
	if (glewInit() != GLEW_OK)
		ERROR(); // termina a execução do programa

	// VSYNC
	// Monitor de 240HZ -> 0= DESATIVADO, 1= 240FPS, 2=120FPS, 3=80FPS
	glfwSwapInterval(0);

	return window;

} // fim createWindow

void initImGUI(GLFWwindow *win)
{

	// iniciar IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// cor da janela
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// jenela de renderização
	ImGui_ImplGlfw_InitForOpenGL(win, true);

	// versão do GLSL -> 3.3
	std::string glslVersion = Shader::shaderProcessorVersion(GLSL_MAJOR_VERSION, GLSL_MINOR_VERSION);
	ImGui_ImplOpenGL3_Init(glslVersion.c_str());
}

void destroyGUI()
{

	if (!ImGui::GetCurrentContext())
		return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

Vector3f randColor()
{
	Vector3f cor;

	for (int i = 0; i < 3; i++)
		cor(i) = (rand() % 256) / 255.0f;

	// cout << cor.transpose() << endl;

	return cor;
}

void createCube(vector<Vector3f> &vertexPos)
{
	// vertices do vertexPos
	// n faces = 12 triangulos
	// nvertices = 36  (12 x 3)

	// Vértice A, B, C, D, E, F, G, H
	// E---------H
	//|  A-------|--D
	//|  |       |  |
	//|  |       |  |
	// F-|-------G  |
	//   B----------C
	Vector3f a(-1, 1, 1);
	Vector3f b(-1, -1, 1);
	Vector3f c(1, -1, 1);
	Vector3f d(1, 1, 1);
	Vector3f e(-1, 1, -1);
	Vector3f f(-1, -1, -1);
	Vector3f g(1, -1, -1);
	Vector3f h(1, 1, -1);

	vertexPos = vector<Vector3f>(36);
	// frente
	vertexPos[0] = a;
	vertexPos[1] = b;
	vertexPos[2] = d;
	vertexPos[3] = d;
	vertexPos[4] = b;
	vertexPos[5] = c;

	// tras
	vertexPos[6] = e;
	vertexPos[7] = f;
	vertexPos[8] = h;
	vertexPos[9] = h;
	vertexPos[10] = f;
	vertexPos[11] = g;

	// esquerda
	vertexPos[12] = e;
	vertexPos[13] = f;
	vertexPos[14] = a;
	vertexPos[15] = a;
	vertexPos[16] = f;
	vertexPos[17] = b;

	// direita
	vertexPos[18] = d;
	vertexPos[19] = c;
	vertexPos[20] = h;
	vertexPos[21] = h;
	vertexPos[22] = c;
	vertexPos[23] = g;

	// topo
	vertexPos[24] = e;
	vertexPos[25] = a;
	vertexPos[26] = h;
	vertexPos[27] = h;
	vertexPos[28] = a;
	vertexPos[29] = d;

	// base
	vertexPos[30] = f;
	vertexPos[31] = b;
	vertexPos[32] = g;
	vertexPos[33] = g;
	vertexPos[34] = b;
	vertexPos[35] = c;
}

void createColoredCube(vector<Vector3f> &vertexPos, vector<Vector3f> &vertexColor)
{
	// vertices do vertexPos
	// n faces = 12 triangulos
	// nvertices = 36  (12 x 3)

	// Vértice A, B, C, D, E, F, G, H
	// E---------H
	//|  A------|--D
	//|  |      |  |
	//|  |      |  |
	// F--|------G  |
	//    B---------C
	Vector3f a(-1, 1, 1);
	Vector3f b(-1, -1, 1);
	Vector3f c(1, -1, 1);
	Vector3f d(1, 1, 1);
	Vector3f e(-1, 1, -1);
	Vector3f f(-1, -1, -1);
	Vector3f g(1, -1, -1);
	Vector3f h(1, 1, -1);

	Vector3f ca(1, 0, 0);		// vermelho
	Vector3f cb(0, 1, 0);		// verde
	Vector3f cc(0, 0, 1);		// azul
	Vector3f cd(0.5, 0, 0.5);	// roxo
	Vector3f ce(1, 1, 0);		// amarelo
	Vector3f cf(0, 1, 1);		// ciano
	Vector3f cg(1, 0.5, 0);		// laranja
	Vector3f ch(0.5, 0.5, 0.5); // cinza

	vertexPos = vector<Vector3f>(36);
	// frente
	vertexPos[0] = a;
	vertexPos[1] = b;
	vertexPos[2] = d;
	vertexPos[3] = d;
	vertexPos[4] = b;
	vertexPos[5] = c;

	// tras
	vertexPos[6] = e;
	vertexPos[7] = f;
	vertexPos[8] = h;
	vertexPos[9] = h;
	vertexPos[10] = f;
	vertexPos[11] = g;

	// esquerda
	vertexPos[12] = e;
	vertexPos[13] = f;
	vertexPos[14] = a;
	vertexPos[15] = a;
	vertexPos[16] = f;
	vertexPos[17] = b;

	// direita
	vertexPos[18] = d;
	vertexPos[19] = c;
	vertexPos[20] = h;
	vertexPos[21] = h;
	vertexPos[22] = c;
	vertexPos[23] = g;

	// topo
	vertexPos[24] = e;
	vertexPos[25] = a;
	vertexPos[26] = h;
	vertexPos[27] = h;
	vertexPos[28] = a;
	vertexPos[29] = d;

	// base
	vertexPos[30] = f;
	vertexPos[31] = b;
	vertexPos[32] = g;
	vertexPos[33] = g;
	vertexPos[34] = b;
	vertexPos[35] = c;

	// uma cor por vértice
	vertexColor = vector<Vector3f>(36);
	vertexColor[0] = ca;
	vertexColor[1] = cb;
	vertexColor[2] = cd;
	vertexColor[3] = cd;
	vertexColor[4] = cb;
	vertexColor[5] = cc;
	vertexColor[6] = ce;
	vertexColor[7] = cf;
	vertexColor[8] = ch;
	vertexColor[9] = ch;
	vertexColor[10] = cf;
	vertexColor[11] = cg;
	vertexColor[12] = ce;
	vertexColor[13] = cf;
	vertexColor[14] = ca;
	vertexColor[15] = ca;
	vertexColor[16] = cf;
	vertexColor[17] = cb;
	vertexColor[18] = cd;
	vertexColor[19] = cc;
	vertexColor[20] = ch;
	vertexColor[21] = ch;
	vertexColor[22] = cc;
	vertexColor[23] = cg;
	vertexColor[24] = ce;
	vertexColor[25] = ca;
	vertexColor[26] = ch;
	vertexColor[27] = ch;
	vertexColor[28] = ca;
	vertexColor[29] = cd;
	vertexColor[30] = cf;
	vertexColor[31] = cb;
	vertexColor[32] = cg;
	vertexColor[33] = cg;
	vertexColor[34] = cb;
	vertexColor[35] = cc;
}

void createColoredPyramid(vector<Vector3f> &vertexPos, vector<Vector3f> &vertexColor)
{
	// vertices do vertexPos
	// n faces = 12 triangulos
	// nvertices = 36  (12 x 3)

	// Vértice A, B, C, D, E
	// A---------D
	//|         |
	//|    E    |
	//|         |
	// B---------C

	Vector3f a(-1, 0, -1);
	Vector3f b(-1, 0, 1);
	Vector3f c(1, 0, 1);
	Vector3f d(1, 0, -1);
	Vector3f e(0, 2, 0);

	Vector3f ca(1, 0.5, 0.78);	// vermelho
	Vector3f cb(1, 0.5, 0.5);	// verde
	Vector3f cc(1, 0.74, 0.76); // azul
	Vector3f cd(1, 0.5, 0.77);	// roxo
	Vector3f ce(1, 0.5, 0.9);	// amarelo

	vertexPos = vector<Vector3f>(18);
	// Base
	vertexPos[0] = a;
	vertexPos[1] = b;
	vertexPos[2] = d;
	vertexPos[3] = d;
	vertexPos[4] = b;
	vertexPos[5] = c;

	// tras
	vertexPos[6] = e;
	vertexPos[7] = d;
	vertexPos[8] = a;

	// esquerda
	vertexPos[9] = e;
	vertexPos[10] = a;
	vertexPos[11] = b;

	// direita
	vertexPos[12] = e;
	vertexPos[13] = c;
	vertexPos[14] = d;

	// frente
	vertexPos[15] = e;
	vertexPos[16] = b;
	vertexPos[17] = c;

	// uma cor por vértice
	vertexColor = vector<Vector3f>(18);
	vertexColor[0] = ca;
	vertexColor[1] = cb;
	vertexColor[2] = cd;
	vertexColor[3] = cd;
	vertexColor[4] = cb;
	vertexColor[5] = cc;
	vertexColor[6] = ce;
	vertexColor[7] = cd;
	vertexColor[8] = ca;
	vertexColor[9] = ce;
	vertexColor[10] = ca;
	vertexColor[11] = cb;
	vertexColor[12] = ce;
	vertexColor[13] = cc;
	vertexColor[14] = cd;
	vertexColor[15] = ce;
	vertexColor[16] = cb;
	vertexColor[17] = cc;
}