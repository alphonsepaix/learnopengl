#include "Texture.h"

#include "stb_image.h"

#include <iostream>

Texture::Texture(const std::string &texturePath, const Type type, const Format format): m_textureId{0}, m_type{type},
    m_format{format} {
    glGenTextures(1, &m_textureId);
    bind();
    int width, height, nrChannels;
    if (unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0)) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, static_cast<GLenum>(m_format), GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture '" << texturePath << "'\n";
    }
}

void Texture::bind() const {
    glBindTexture(static_cast<GLenum>(m_type), m_textureId);
}

void Texture::setUnit(const int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    bind();
}

void Texture::setFilter(const Filter minFilter, const Filter magFilter) const {
    glTexParameteri(static_cast<GLenum>(m_type), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
    glTexParameteri(static_cast<GLenum>(m_type), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
}

void Texture::setWrap(const Wrap wrapS, const Wrap wrapT, const Wrap wrapR) const {
    glTexParameteri(static_cast<GLenum>(m_type), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapS));
    glTexParameteri(static_cast<GLenum>(m_type), GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapT));
    glTexParameteri(static_cast<GLenum>(m_type), GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapR));
}

void Texture::setBorderColor(const GLfloat *params) const {
    glTexParameterfv(static_cast<GLenum>(m_type), GL_TEXTURE_BORDER_COLOR, params);
}
