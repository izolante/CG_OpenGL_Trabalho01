#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <vector>
#include <Eigen/Dense>
#include "Math.h"

using namespace std;
using namespace Eigen;


class Camera
{
public:

    //vFOV =  angulo em radianos
    Camera(int winWidth, int winHeight, float vFOV, float zNear, float zFar, Vector3f cameraPos)
    {
        //define a posicao da camera
        position = cameraPos;


        //computa a matriz de projeção
        float aspectRatio = float(winWidth) / float(winHeight);
        float halfFOV = tan(vFOV/2.0f);
        
        
        projectionMatrix = Eigen::Matrix4f::Zero();
        projectionMatrix(0,0) = 1.0f / (halfFOV * aspectRatio);
        projectionMatrix(1,1) = 1.0f / halfFOV;
        projectionMatrix(2,2) = (zNear+zFar) / (-zFar + zNear);
        projectionMatrix(2,3) = (2.0f * zNear * zFar) / (-zFar + zNear);
        projectionMatrix(3,2) = -1.0f;
    }
    
    //GET
    Eigen::Matrix4f getProjectionMatrix()
    {
        return projectionMatrix;
    }
    Eigen::Matrix4f getViewMatrix()
    {
        //translacao inversa
        Eigen::Matrix4f translation = Math::translationMat(-position);

        //Falta a rotação, a qual será calculada nas próximas aulas...

        return translation;
    }
    
private:
    Eigen::Matrix4f projectionMatrix;
    Eigen::Vector3f position;

   
};

#endif