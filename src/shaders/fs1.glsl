#version 330 core

in vec3 _colour;
in vec2 _texcoord;

out vec4 out_colour;

uniform sampler2D tex;

void main()
{
	out_colour = texture(tex, _texcoord); 
}
