#version 330

layout (location=0) in vec3 position;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main(void)
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(position, 1.0); //multiplica as matrizes pela coordenada homogê
} 
