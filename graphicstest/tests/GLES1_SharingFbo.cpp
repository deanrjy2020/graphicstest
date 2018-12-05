#include "common.h"

extern Common com;

class GLES1_SharingFbo {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:

    EGLDisplay m_disp;
    EGLConfig m_config;
    EGLSurface m_pbuffer[2];
    EGLContext m_contexts[2];

    GLuint s_frameBuffer1 = 2;
    GLuint s_texture = 1;
    GLuint s_frameBuffer2 = 4;

    bool makeCurrent(GLint contextId);
};

static const EGLint contextAttrs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 1,
    EGL_NONE,
};

bool GLES1_SharingFbo::makeCurrent(GLint contextId) {
    if (!eglMakeCurrent(m_disp, m_pbuffer[contextId], m_pbuffer[contextId], m_contexts[contextId])) {
        DEBUG_PRINT(("\nERROR: unable to select ES1 Context\n\n"));
        return false;
    }

    return true;
}

bool GLES1_SharingFbo::initGraphics() {

    m_disp = eglGetCurrentDisplay();

    const EGLint configAttrs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE,
    };
    EGLint num_config;
    EGLBoolean success = eglChooseConfig(m_disp, configAttrs, &m_config, 1, &num_config);
    if (!success || num_config == 0) {
        DEBUG_PRINT("ERROR: failed to choose config\n");
        return false;
    }

    m_contexts[0] =
        eglCreateContext(m_disp, m_config, EGL_NO_CONTEXT, contextAttrs);
    m_contexts[1] =
        eglCreateContext(m_disp, m_config, EGL_NO_CONTEXT, contextAttrs);

    const EGLint pbufferAttrs[] = {
        EGL_WIDTH, 640,
        EGL_HEIGHT, 480,
        EGL_NONE,
    };

    for (int i = 0; i < 2; i++) {
        m_pbuffer[i] = eglCreatePbufferSurface(m_disp, m_config, pbufferAttrs);
    }


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    return true;
}
bool GLES1_SharingFbo::doGraphics() {

#if 0
    // cannot repr the driver memleak issue, looks goog for now.
    makeCurrent(0);
    // Create the texture for FBO's
    glBindTexture(GL_TEXTURE_2D, s_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Attach the texture to framebuffer2
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, s_frameBuffer2);
    glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, s_texture, 0);

    // Check that the framebuffer2 status is complete
    MY_ASSERT(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);

    // Set Shared2 context as current and attach 2 FBO's
    MY_ASSERT(makeCurrent(1));

    glDeleteTextures(1, &s_texture);

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, s_frameBuffer2);

    glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, 0, 0);

    com.checkError("last check.");

#endif






    com.makeCurrent();
    return true;
}
bool GLES1_SharingFbo::exitGraphics() {
    return true;
}
bool GLES1_SharingFbo::isSupported() {
    return true;
}

ADD_TEST(GLES1_SharingFbo)
