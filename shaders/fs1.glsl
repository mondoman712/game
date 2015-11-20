#version 150

in vec3 _colour;
in vec2 _texcoord;

out vec4 out_colour;

uniform sampler2D texpat;
uniform sampler2D texbmo;

void main()
{
	vec4 colpat = texture(texpat, _texcoord);
	vec4 colbmo = texture(texbmo, _texcoord);
	out_colour = mix(colpat, colbmo, 0.5);
}
