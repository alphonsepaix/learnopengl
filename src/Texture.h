#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <string>

class Texture {
public:
    enum class Type {
        Diffuse, Specular
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

    explicit Texture(const std::string &texturePath, Type type = Type::Diffuse);

    ~Texture();

    Texture(const Texture &) = delete;

    Texture &operator=(const Texture &) = delete;

    Texture(Texture &&other) noexcept;

    Texture &operator=(Texture &&other) noexcept;

    void bind() const;

    void setUnit(int unit) const;

    void setFilter(Filter minFilter, Filter magFilter) const;

    void setWrap(Wrap wrapS, Wrap wrapT,
                 Wrap wrapR = Wrap::Repeat) const;

    void setBorderColor(const GLfloat *params) const;

    [[nodiscard]] Type getType() const { return m_type; }

private:
    GLuint m_textureId;
    Type m_type;
};

#endif
