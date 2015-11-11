#version 150 

in vec3 position;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = vec4(position, 1.0);
}
