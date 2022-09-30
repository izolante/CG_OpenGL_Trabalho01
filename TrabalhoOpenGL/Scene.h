#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <vector>
using namespace std;

//#include <Eigen/Dense>
//using namespace Eigen;

#include <GL/glew.h> //GLEW

#include "Shader.h"
#include "Object.h"
#include "Camera.h"

class Scene
{
public:
    
    //---------ATRIBUTOS---------  

    //vetor de objetos
	vector<Object> objects;


    Camera *camera;
    bool animation;
    bool wireframe;
    int eixo;
    int trans;
    int objetoSelecionado;
    int velocidade;
    int velocidadeScala;
    int velocidadeRotacao;

    //---------METODOS---------
    Scene() //construtor,  inicializa os atributos da classe
    {
        camera = nullptr;
        wireframe = false;
        animation = false;

        velocidade = 1;
        eixo=0;
        trans =0;
        objetoSelecionado= 0;
        velocidadeScala = 1;
        velocidadeRotacao = 1;
    };

    void destroy()
    {

        for(Object o: objects)
            o.destroy();
        
        objects.clear();

        if(camera)
        {
            delete camera;
            camera = nullptr;
        }
    }

 

};

#endif