#version 330

layout (location=0) in vec3 position;
layout (location=2) in vec2 texture;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

out vec2 txCoord;

void main(void)
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(position, 1.0); //multiplica as matrizes pela coordenada homogênea
	txCoord = texture;
} 



