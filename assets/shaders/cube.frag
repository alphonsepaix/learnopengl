#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.specular;

    // emission
    float borderWidth = 0.1f;
    float mask = step(borderWidth, TexCoords.x)
    * step(TexCoords.x, 1.0f - borderWidth)
    * step(borderWidth, TexCoords.y)
    * step(TexCoords.y, 1.0f - borderWidth);
    vec3 emission = texture(material.emission, TexCoords).rgb * mask;

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0f);
}