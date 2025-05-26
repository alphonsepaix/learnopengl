#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform float mixValue;

uniform sampler2D container;
uniform sampler2D face;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // FragColor = mix(texture(container, texCoord), texture(face, texCoord), mixValue);

    // ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos); // world space coordinates
    vec3 diffuse = max(dot(norm, lightDir), 0.0f) * lightColor;

    // specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    // lightDir is the direction from the fragment to the light source, so we negate it
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = 0.5f;
    int shininess = 128;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
}