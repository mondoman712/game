#version 150

in vec3 vert;
in vec2 verttexcoord;
in vec3 vertnorm;

out vec3 fragvert;
out vec2 fragtexcoord;
out vec3 fragnorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 pos;

void main()
{
	fragvert = vert;
	fragtexcoord = verttexcoord;
	fragnorm = vertnorm;

	gl_Position = proj * view * model * pos * vec4(vert, 1.0);
}
