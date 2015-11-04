#version 150 

in vec2 position;
in vec3 in_colour;

out vec3 _colour;

void main()
{
	_colour = in_colour;
	gl_Position = vec4(position, 0.0, 1.0);
}
