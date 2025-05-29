#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <string>

class Texture {
public:
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

    explicit Texture(const std::string &texturePath);

    void bind() const;

    static void unbind();

    void setUnit(int unit) const;

    void setFilter(Filter minFilter, Filter magFilter) const;

    void setWrap(Wrap wrapS, Wrap wrapT,
                 Wrap wrapR = Wrap::Repeat) const;

    void setBorderColor(const GLfloat *params) const;

private:
    GLuint m_textureId;
};

#endif
