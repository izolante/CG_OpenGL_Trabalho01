#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec3 color;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

out vec3 vcolor;

void main(void)
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(position, 1.0); //multiplica as matrizes pela coordenada homogê
	vcolor = color;
} 
