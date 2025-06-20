#include <fmt/format.h>
#include <stb_image.h>

#include "Texture.h"

#include <iostream>
#include <utility>

Texture::Texture(const std::string &texturePath, const Type type): m_textureId{0}, m_type{type} {
    glGenTextures(1, &m_textureId);
    bind();
    int width, height, nrChannels;
    if (unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0)) {
        GLenum format;
        switch (nrChannels) {
            case 1: format = GL_RED;
                break;
            case 3: format = GL_RGB;
                break;
            case 4: format = GL_RGBA;
                break;
            default: std::unreachable();
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        const auto str = fmt::format("Failed to load texture '{}'", texturePath);
        throw std::runtime_error(str);
    }
    setFilter(Filter::LinearMipmapLinear, Filter::Linear);
    setWrap(Wrap::Repeat, Wrap::Repeat);
}

Texture::~Texture() {
    if (m_textureId != 0)
        glDeleteTextures(1, &m_textureId);
}

Texture::Texture(Texture &&other) noexcept {
    m_textureId = other.m_textureId;
    m_type = other.m_type;
    other.m_textureId = 0; // prevent the destructor from deleting the texture when other goes out of scope
}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        if (m_textureId != 0) {
            glDeleteTextures(1, &m_textureId);
        }
        m_textureId = other.m_textureId;
        m_type = other.m_type;
        other.m_textureId = 0;
    }
    return *this;
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::setUnit(const int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    bind();
}

void Texture::setFilter(const Filter minFilter, const Filter magFilter) const {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
}

void Texture::setWrap(const Wrap wrapS, const Wrap wrapT, const Wrap wrapR) const {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapR));
}

void Texture::setBorderColor(const GLfloat *params) const {
    bind();
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, params);
}
