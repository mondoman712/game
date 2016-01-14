#version 330 core

in vec3 fragvert;
in vec2 fragtexcoord;
in vec3 fragnorm;

out vec4 out_colour;

uniform mat4 model;
uniform vec3 campos;

uniform sampler2D tex;
uniform float mat_shine;
uniform vec3 mat_specularcol;
uniform int illum;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambient_coefficient;
} light;

void main()
{
	if (illum == 0) {
		out_colour = texture(tex, fragtexcoord);
		return;
	}

	vec3 norm = normalize(transpose(inverse(mat3(model))) * fragnorm);
	vec3 surfpos = vec3(model * vec4(fragvert, 1));
	vec4 surfcol = texture(tex, fragtexcoord);
	vec3 surf2light = normalize(light.position - surfpos);

	/* Diffuse Lighting */
	float diffuse_coefficient = max(0.0, dot(norm, surf2light));
	vec3 diffuse = diffuse_coefficient * surfcol.rgb * light.intensities;

	/* Ambient Lighting */
	vec3 ambient = light.ambient_coefficient * surfcol.rgb *
		light.intensities;

	/* Specular Lighting */
	vec3 incidence = -surf2light;
	vec3 reflection = reflect(incidence, norm);
	vec3 surf2cam = normalize(campos - surfpos);
	float cosang = max(0.0, dot(surf2cam, reflection));
	float specular_coefficient = pow(cosang, mat_shine);

	vec3 specular = specular_coefficient * mat_specularcol *
		light.intensities;

	/* Attenuation (Inverse Square Rule) */
	float dist2light = length(light.position - surfpos);
	float attenuation = 1.0 / (1.0 + light.attenuation
			* pow(dist2light, 2));

	vec3 linear_colour = ambient + attenuation * (diffuse + specular);

	/* Gamma Correction */
	vec3 gamma = vec3(1.0 / 2.2);

	out_colour = vec4(pow(linear_colour, gamma), surfcol.a);
}
