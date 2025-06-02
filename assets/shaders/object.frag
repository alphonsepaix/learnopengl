#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D emission;

    float shininess;
};

struct Light {
    int type;

    vec3 direction;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform bool emission;
uniform vec3 viewPos;
uniform Light lights[30];
uniform int lightCount;

vec3 calcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(Light light, vec3 normal, vec3 viewDir);
vec3 calcSpotLight(Light light, vec3 normal, vec3 viewDir);

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0f);
    for (int i = 0; i < lightCount; i++)
    {
        if (lights[i].type == 0) {
            result += calcDirLight(lights[i], norm, viewDir);
        }
        else if (lights[i].type == 1) {
            result += calcPointLight(lights[i], norm, viewDir);
        }
        else if (lights[i].type == 2) {
            result += calcSpotLight(lights[i], norm, viewDir);
        } else {
            // Unsupported light type, skip
            continue;
        }
    }

    if (emission)
    {
        float borderWidth = 0.1f;
        float mask = step(borderWidth, TexCoords.x)
        * step(TexCoords.x, 1.0f - borderWidth)
        * step(borderWidth, TexCoords.y)
        * step(TexCoords.y, 1.0f - borderWidth);
        result += texture(material.emission, TexCoords).rgb * mask;
    }

    FragColor = vec4(result, 1.0f);
}

vec3 calcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.texture_specular1, TexCoords).rgb * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 calcPointLight(Light light, vec3 normal, vec3 viewDir)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    // diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = texture(material.texture_specular1, TexCoords).rgb * spec * light.specular;

    vec3 result = ambient + diffuse + specular;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    return result * attenuation;
}

vec3 calcSpotLight(Light light, vec3 normal, vec3 viewDir)
{
    // ambient
    vec3 result = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
    if (intensity > 0.0f) {
        // diffuse
        float diff = max(dot(normal, lightDir), 0.0f);
        vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

        // specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        vec3 specular = texture(material.texture_specular1, TexCoords).rgb * spec * light.specular;

        result += (diffuse + specular) * intensity;
    }

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    return result * attenuation;
}
