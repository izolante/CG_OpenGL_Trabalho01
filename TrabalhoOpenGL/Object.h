#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

// metadado associado aos atributos dos vértices (coordendas que definem a posicao dos vértices, coordenadas de textura, cor de cada vértice, etc)
// Para configurar os atributos dos vértices, cada variável de entrada no vertexshader está associada ao metadado "location"
#define VERTEX_ATTRIB_POS 0   // no vertexshader, a variável de entrada que define a posição do vértice possui "layout (location=0)"
#define VERTEX_ATTRIB_COLOR 1 // no vertexshader, a variável de entrada que define a cor do vértice possui "layout (location=1)"
#define VERTEX_ATTRIB_TEX 2   // no vertexshader, a variável de entrada que define a coord de textura do vértice possui "layout (location=2)"

#include <iostream>
#include <vector>
using namespace std;

#include <Eigen/Dense>
using namespace Eigen;

#include <GL/glew.h> //GLEW

#include "Math.h"
#include "Texture.h"

class Object
{
public:
    //---------METODOS---------
    Object() // construtor,  inicializa os atributos da classe
    {
        resetAttributes();
    }

    // GET

    string getName()
    {
        return name;
    }

    // cria um objeto -> criar VAO e VBOs, carrega os dados dos atributos dos vértices na GPU
    // por padrão, vertexColor é inicializado com "vector<Vector3f>()"", ou seja, um vetor sem dados gravados.
    bool create(string name, vector<Vector3f> vertexPos, vector<Vector3f> vertexColor = vector<Vector3f>(), vector<Vector2f> vertexTexture = vector<Vector2f>())
    {

        // verificação de erros com base nos parâmetros
        if (vertexPos.size() < 3)
        {
            cout << "Object::create -> Erro, o objeto deve possuir pelo menos 3 vértices!\n";
            return false;
        }

        if (vertexPos.size() % 3 != 0)
        {
            cout << "Object::create -> Erro, o número de vértices deve ser múltiplos de 3, pois triângulos serão renderizados!\n";
            return false;
        }

        if (vertexColor.size() > 0 && vertexColor.size() != vertexPos.size())
        {
            cout << "Object::create -> Erro, os vetores vertexPos e vertexColor devem possuir o mesmo tamanho.\n";
            return false;
        }

        if (vertexTexture.size() > 0 && vertexTexture.size() != vertexPos.size())
        {
            cout << "Object::create -> Erro, os vetores vertexPos e vertexTexture devem possuir o mesmo tamanho.\n";
            return false;
        }

        // se um objeto já foi criado, destroi ele primeiro
        destroy();

        this->nVertices = vertexPos.size();
        this->name = name;

        // Cria o Vertex Array Object (VAO). ELe vai armazenar as informações dos atributos dos vértices
        glGenVertexArrays(1, &VAO); // cria um ponteiro para o VAO
        glBindVertexArray(VAO);     // bind

        //-----------------
        // Para CADA atributo -> (1) criar um buffer (VBO) e carregar os dados do atributo para a memória da GPU.
        // Em seguida, (2) definir como estes dados serão acessados no vertex shader, associando o buffer criado ao metadado "location" do vertex shader!
        //-----------------

        //-----------------
        // Atributo -> Posição do vértice
        // cria o buffer para armazenar a posição dos vértices
        glGenBuffers(1, &VBO_POS);                                                                         // cria um ponteiro para o buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO_POS);                                                            // bind
        glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(Vector3f), &vertexPos[0], GL_STATIC_DRAW); // carra os dados para a GPU

        // Associa o location=0 para o atributo posição do vértice.
        glEnableVertexAttribArray(VERTEX_ATTRIB_POS);
        glVertexAttribPointer(VERTEX_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        //-----------------

        //-----------------
        // Atributo -> Cor do vértice
        if (vertexColor.size() > 0)
        {
            glGenBuffers(1, &VBO_COLOR);                                                                              // cria um ponteiro para o buffer
            glBindBuffer(GL_ARRAY_BUFFER, VBO_COLOR);                                                                 // bind
            glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(Vector3f), &vertexColor.at(0), GL_STATIC_DRAW); // carra os dados para a GPU
            glEnableVertexAttribArray(VERTEX_ATTRIB_COLOR);
            glVertexAttribPointer(VERTEX_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        }
        //-----------------

        //-----------------
        // Atributo -> Textura do vértice
        if (vertexTexture.size() > 0)
        {
            glGenBuffers(1, &VBO_TEXTURE);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_TEXTURE);
            glBufferData(GL_ARRAY_BUFFER, vertexTexture.size() * sizeof(Vector2f), &vertexTexture.at(0), GL_STATIC_DRAW);
            glEnableVertexAttribArray(VERTEX_ATTRIB_TEX); // define como o buffer será acessado no vertex shader!
            glVertexAttribPointer(VERTEX_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
        }
        //-----------------

        // unbind all !!
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;

    } // fim createObject

    bool hasVertexColorAtrrib()
    {
        if (VBO_COLOR != 0)
            return true;
        else
            return false;
    }

    bool hasVertexTextureAtrrib()
    {
        if (VBO_TEXTURE != 0 && texturePtr != 0)
            return true;
        else
            return false;
    }

    // renderiza um objeto
    void render()
    {
        if (!VAO || nVertices <= 0)
            return;

        if (hasVertexTextureAtrrib() == true)
        {
            glActiveTexture(GL_TEXTURE0); // unidade de textura zero
            glBindTexture(GL_TEXTURE_2D, texturePtr);
        }

        // Ativa o VAO e os atributos dos vértices associados a este VAO
        glBindVertexArray(VAO);

        // renderizar os triangulos
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)nVertices);

        // desativa o VAO
        glBindVertexArray(0);

        // desativa a textura
        if (hasVertexTextureAtrrib() == true)
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
        }
    }

    bool setTexture(std::string texPath)
    {

        if (!VBO_TEXTURE)
        {
            cout << "Object::setTexture: Erro, o objeto nao possui coordenadas de textura!" << endl;
            return false;
        }

        // remove caso ela já tenha sido criada anteriormente
        if (texturePtr > 0)
        {
            glDeleteTextures(1, &texturePtr);
            texturePtr = 0;
        }

        return Texture::loadOpenGLTexture(texPath, &texturePtr);
    }

    void destroy()
    {

        // remove buffer
        if (VBO_POS)
            glDeleteBuffers(1, &VBO_POS);

        // remove buffer
        if (VBO_COLOR)
            glDeleteBuffers(1, &VBO_COLOR);

        // remove buffer
        if (VBO_TEXTURE)
            glDeleteBuffers(1, &VBO_TEXTURE);

        // remove o VAO
        if (VAO)
            glDeleteVertexArrays(1, &VAO);

        // remove a textura
        if (texturePtr > 0)
            glDeleteTextures(1, &texturePtr);

        resetAttributes();
    }

    void setColor(Vector3f c)
    {
        color = c;
    }

    Vector3f getColor()
    {
        return color;
    }

    void setRMatrix(Matrix4f R)
    {
        rotationMat = R;
    }
    void setTMatrix(Matrix4f T)
    {
        translationMat = T;
    }
    void setSMatrix(Matrix4f S)
    {
        scaleMat = S;
    }
    void setPosition(Vector3f pos)
    {
        translationMat = Math::translationMat(pos);
    }

    void setScale(Vector3f scl)
    {
        scaleMat = Math::scaleMat(scl);
    }

    Matrix4f getRMatrix()
    {
        return rotationMat;
    }
    Matrix4f getTMatrix()
    {
        return translationMat;
    }
    Matrix4f getSMatrix()
    {
        return scaleMat;
    }

    Vector3f getPosition()
    {
        Vector3f pos;

        pos(0) = translationMat(0, 3);
        pos(1) = translationMat(1, 3);
        pos(2) = translationMat(2, 3);

        return pos;
    }

    Vector3f getScale()
    {
        Vector3f scl;

        scl(0) = scaleMat(0, 0);
        scl(1) = scaleMat(1, 1);
        scl(2) = scaleMat(2, 2);

        return scl;
    }

private:
    void resetAttributes() // inicializa os atributos da classe
    {
        VAO = 0;
        VBO_POS = 0;
        VBO_COLOR = 0;
        VBO_TEXTURE = 0;
        nVertices = 0;
        texturePtr = 0;
        name = "";
        color = Vector3f(1.0f, 0.0f, 0.0f);
        rotationMat = Matrix4f::Identity();
        translationMat = Matrix4f::Identity();
        scaleMat = Matrix4f::Identity();
    }

    //---------ATRIBUTOS---------
    GLuint VAO;
    GLuint VBO_POS;
    GLuint VBO_COLOR;
    GLuint VBO_TEXTURE;
    size_t nVertices;
    GLuint texturePtr;
    string name;
    Vector3f color;
    Matrix4f rotationMat;
    Matrix4f translationMat;
    Matrix4f scaleMat;
};

#endif