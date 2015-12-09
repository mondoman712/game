#version 130

in vec3 position;
in vec2 texcoord;

out vec2 _texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 pos;

void main()
{
	_texcoord = texcoord;
	gl_Position = proj * view * model * pos * vec4(position, 1.0);
}
