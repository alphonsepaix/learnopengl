#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int shininess;
uniform float ambientStrength;
uniform float specularStrength;

void main() {
    vec4 viewPos = view * model * vec4(aPos, 1.0f);
    gl_Position = projection * viewPos;

    vec3 fragPos = vec3(viewPos);

    // ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // diffuse lighting
    vec3 norm = normalize(normalMatrix * aNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0f) * lightColor;

    // specular lighting
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0f);
}
