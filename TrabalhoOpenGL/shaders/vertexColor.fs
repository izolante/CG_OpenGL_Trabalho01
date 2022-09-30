#version 330
in vec3 vcolor; //mesmo nome da variável definina no vertex shader
out vec4 FragColor;


void main(void)
{	
	FragColor = vec4(vcolor, 1.0);
}

