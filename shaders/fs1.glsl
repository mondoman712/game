#version 150

in vec3 _colour;

out vec4 out_colour;

void main()
{
	out_colour = vec4(_colour, 1.0);
}
