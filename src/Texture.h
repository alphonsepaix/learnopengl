#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <string>

class Texture {
public:
    enum class Type {
        Texture2D = GL_TEXTURE_2D,
    };

    enum class Format {
        RGB = GL_RGB,
        RGBA = GL_RGBA,
    };

    enum class Wrap {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
    };

    enum class Filter {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
    };

    Texture(const std::string &texturePath, Type type,
            Format format);

    void bind() const;

    void setUnit(int unit) const;

    void setFilter(Filter minFilter, Filter magFilter) const;

    void setWrap(Wrap wrapS, Wrap wrapT,
                 Wrap wrapR = Wrap::Repeat) const;

    void setBorderColor(const GLfloat *params) const;

private:
    GLuint m_textureId;
    Type m_type;
    Format m_format;
};

#endif
