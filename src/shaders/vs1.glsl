#version 330 core

in vec3 position;
in vec3 in_colour;
in vec2 texcoord;

out vec3 _colour;
out vec2 _texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	_texcoord = texcoord;
	_colour = in_colour;
	gl_Position = proj * view * model * vec4(position, 1.0);
}
