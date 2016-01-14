#version 330 core

in vec3 vert;
in vec2 verttexcoord;
in vec3 vertnorm;

out vec3 fragvert;
out vec2 fragtexcoord;
out vec3 fragnorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	fragvert = vert;
	fragtexcoord = verttexcoord;
	fragnorm = vertnorm;

	gl_Position = proj * view * model * vec4(vert, 1.0);
}
