#version 150

uniform vec3 tri_colour;

out vec4 out_colour;

void main()
{
	out_colour = vec4(tri_colour, 1.0);
}
