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
	vec4 position;
	vec3 intensities;
	float attenuation;
	float ambient_coefficient;
} light;

void lit (out vec4 col)
{
	vec3 norm = normalize(transpose(inverse(mat3(model))) * fragnorm);
	vec3 surfpos = vec3(model * vec4(fragvert, 1));
	vec4 surfcol = texture(tex, fragtexcoord);
	vec3 surf2light;
	float attenuation;

	/* Checks if Light is directional */
	if (light.position.w == 0.0) {
		/* Light is directional */
		surf2light = normalize(light.position.xyz);
		attenuation = 0.0;
	} else {
		surf2light = normalize(light.position.xyz - surfpos);
		attenuation = light.attenuation;
	}

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
	float dist2light = length(light.position.xyz - surfpos);
	attenuation = 1.0 / (1.0 + attenuation * pow(dist2light, 2));

	vec3 linear_colour = ambient + attenuation * (diffuse + specular);

	/* Gamma Correction */
	vec3 gamma = vec3(1.0 / 2.2);

	col = vec4(pow(linear_colour, gamma), surfcol.a);
}

void unlit (out vec4 col)
{
	col = texture(tex, fragtexcoord);
}

void main ()
{
	switch (illum) {
		case 0:
			unlit(out_colour);
			break;
		default:
			lit(out_colour);
			break;
	}
}
