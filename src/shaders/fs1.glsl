#version 150

in vec3 fragvert;
in vec2 fragtexcoord;
in vec3 fragnorm;

out vec4 out_colour;

uniform mat4 model;
uniform sampler2D tex;
uniform struct Light {
	vec3 position;
	vec3 intensities;
} light;

void main()
{
	mat3 norm_mat = transpose(inverse(mat3(model)));
	vec3 norm = normalize(norm_mat * fragnorm);

	vec3 fragpos = vec3(model * vec4(fragvert, 1));

	vec3 surf2light = light.position - fragpos;

	float brightness = dot(norm, surf2light) / (length(surf2light) *
			length(norm));
	brightness = clamp(brightness, 0, 1);

	vec4 surfcol = texture(tex, fragtexcoord); 
	out_colour = vec4(brightness * light.intensities * surfcol.rgb,
			surfcol.a);
}
