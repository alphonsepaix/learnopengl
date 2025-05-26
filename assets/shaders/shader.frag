#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform float mixValue;
uniform sampler2D container;
uniform sampler2D face;

void main() {
    FragColor = mix(texture(container, texCoord), texture(face, texCoord), mixValue);
}