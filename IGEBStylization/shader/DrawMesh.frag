#version 430

uniform vec4 color;
uniform vec3 Ambient;
uniform vec3 LightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float Shininess;
uniform float Strength;
uniform int isBlinn;

in vec3 FragPos;
in vec3 Normal;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 AmbiColor;
layout (location = 2) out vec4 DiffColor;
layout (location = 3) out vec4 SpecColor;

void main() {
	//// compute cosine of the directions, using dot products,
	//// to see how much light would be reflected
	//float diffuse = max(0.0, dot(Normal, LightDirection));
	//float specular = max(0.0, dot(Normal, HalfVector));
	//// surfaces facing away from the light (negative dot products)
	//// won¡¦t be lit by the directional light
	//if (diffuse == 0.0)
	//	specular = 0.0;
	//else
	//	specular = pow(specular, Shininess); // sharpen the highlight
	//vec3 scatteredLight = Ambient + LightColor * diffuse;
	//vec3 reflectedLight = LightColor * specular * Strength;
	//// don¡¦t modulate the underlying color with reflected light,
	//// only with scattered light
	//vec3 rgb = min(Color.rgb * scatteredLight + reflectedLight, vec3(1.0));
	//FragColor = vec4(rgb, Color.a);

	// Ambient
	vec3 ambient = Ambient;
	// Diffuse
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color.rgb;
	// Specular
	vec3 viewDir = -normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	if (isBlinn == 1)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), Shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	}
	vec3 specular = LightColor * spec * Strength; // assuming bright white light color
	FragColor = vec4(ambient + diffuse + specular, 1.0f);
	AmbiColor = vec4(ambient, 1.0f);
	DiffColor = vec4(diffuse, 1.0f);
	SpecColor = vec4(specular, 1.0f);
}
