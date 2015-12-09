#version 130

in vec2 _texcoord;

out vec4 out_colour;

uniform sampler2D tex;

void main()
{
	out_colour = texture(tex, _texcoord); 
}
