#version 330 core

in vec3 vert;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(vert, 1.0);
}
