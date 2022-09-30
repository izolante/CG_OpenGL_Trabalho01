#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <string>

class Texture
{
public:
    Texture();

    static bool loadOpenGLTexture(std::string path, unsigned int *texturePtr, bool verticalFlip=true, bool mipmap=true, bool anisotropicFilter =true);

};

#endif