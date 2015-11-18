#version 150 

in vec2 position;
in vec3 in_colour;
in vec2 texcoord;

out vec3 _colour;
out vec2 _texcoord;

void main()
{
	_texcoord = texcoord;
	_colour = in_colour;
	gl_Position = vec4(position, 0.0, 1.0);
}
