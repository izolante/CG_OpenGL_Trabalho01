#include "Texture.h"

#include <iostream>
#include <stb/stb_image.h>

#include <GL/glew.h>  //GLEW

using namespace std;



Texture::Texture(){

}



bool Texture::loadOpenGLTexture(std::string path, unsigned int *texturePtr, bool verticalFlip, bool mipmap, bool anisotropicFilter){
    int width; 
    int height;
    int channels;

    
    if(verticalFlip)
        stbi_set_flip_vertically_on_load(true); 


    unsigned char *imgData = stbi_load(path.c_str(), &width, &height, &channels, 0); 
	if (!imgData){
        cout << "loadOpenGLTexture: Erro ao ler o arquivo " << path << endl;
        return false;
    }

    //cout << path << " - " << width << " - " << height << " - " << channels << "\n";
    
    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    //gera um ponteiro para a texura e carrega a imagem na GPU
    unsigned int ptr;
    glGenTextures(1, &ptr);
    glBindTexture(GL_TEXTURE_2D, ptr);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //use isso, pois em imagens grandes (2253x2352) o opengl se perde na leitura da imagem
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    *texturePtr = ptr;

    //Ativar mipmap?
    if(mipmap){
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //default
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//default
    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
    }

   //ativar filtro?
	if (anisotropicFilter && glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);

        //cout << "anisoset: " << anisoset << endl;
	}



    //libera memoria
	glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free( imgData );
	return true;
}
