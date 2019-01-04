#include "common.h"

extern Common com;
extern char *debugOptions;

class Texture3D {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

bool Texture3D::initGraphics() {
    return true;
}

void mipmapExpandingFromBaseLevelBugDriver() {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);

    // load and generate the texture
    //int width=1024, height=8, depth=1, bpp = 2;
    int width=2048, height=2048, depth=1, bpp = 2;
    int size = width * height * depth * bpp;
    unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char) * size);

    if (data) {
        memset(data, 128, size);
        com.checkGlError("before glTexImage3D1");
        //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
        com.checkGlError("after glTexImage3D1");
        glGenerateMipmap(GL_TEXTURE_3D);
        com.checkGlError("glGenerateMipmap");

        {
            width=512, height=64, depth=32;
            size = width * height * depth * bpp;
            unsigned char *data2 = (unsigned char*)malloc(sizeof(unsigned char) * size);
            memset(data, 113, size);
            com.checkGlError("before glTexImage3D2");
            //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data2);
            com.checkGlError("after glTexImage3D2");

//            // bug 1, failed.
//            DEBUG_PRINT("run bug1");
//            glGenerateMipmap(GL_TEXTURE_3D);
//            com.checkGlError("glGenerateMipmap2");

            free(data2);
        }

        // bug 2, system down in the BindTexture().
        {
            GLfloat value = 10;
            glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, &value);
            com.checkGlError("glTexParameterfv");
        }

        glBindTexture(GL_TEXTURE_3D, texture);
    } else {
        DEBUG_PRINT("Failed to load texture");
    }

    free(data);
}

bool Texture3D::doGraphics() {
    mipmapExpandingFromBaseLevelBugDriver();

    com.makeCurrent();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return true;
}
bool Texture3D::exitGraphics() {
    return true;
}
bool Texture3D::isSupported() {
    return true;
}

ADD_TEST(Texture3D)
