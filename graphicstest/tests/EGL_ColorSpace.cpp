#include "common.h"

extern Common com;
extern char *debugOptions;

class EGL_ColorSpace {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

bool EGL_ColorSpace::initGraphics() {
    return true;
}
bool EGL_ColorSpace::doGraphics() {

    // a new EGL sRGB + FP16
    {
        EGLBoolean returnValue;

        EGLint majorVersion;
        EGLint minorVersion;
        EGLContext context;
        EGLSurface surface;
        EGLint w, h;

        EGLDisplay dpy;

        com.checkEglError("<init>");
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        com.checkEglError("eglGetDisplay");
        if (dpy == EGL_NO_DISPLAY) {
            printf("eglGetDisplay returned EGL_NO_DISPLAY.\n");
            return 0;
        }

        returnValue = eglInitialize(dpy, &majorVersion, &minorVersion);
        com.checkEglError("eglInitialize");
        fprintf(stderr, "EGL version %d.%d\n", majorVersion, minorVersion);
        if (returnValue != EGL_TRUE) {
            printf("eglInitialize failed\n");
            return 0;
        }

        WindowSurface windowSurface;
        EGLNativeWindowType window = windowSurface.getSurface();


        EGLint configId = 0, numConfigs;
        EGLConfig myConfig = {0};
        if (!std::strcmp(debugOptions, "8bit")) {
            EGLint s_configAttribs[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NONE
            };
            eglChooseConfig(dpy, s_configAttribs, &myConfig, 1, &numConfigs);
        } else {
            EGLint s_configAttribs[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_COLOR_COMPONENT_TYPE_EXT, EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
                EGL_NONE
            };
            eglChooseConfig(dpy, s_configAttribs, &myConfig, 1, &numConfigs);
        }

        if (numConfigs == 0) {
            printf(("Failed to choose a config that supports xxxxxx.\n"));
        }
        eglGetConfigAttrib(dpy, myConfig, EGL_CONFIG_ID, &configId);


        EGLint winAttribs[] = {EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
        surface = eglCreateWindowSurface(dpy, myConfig, window, winAttribs);
        com.checkEglError("eglCreateWindowSurface");
        if (surface == EGL_NO_SURFACE) {
            printf("eglCreateWindowSurface failed.\n");
            return 0;
        }

        EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context = eglCreateContext(dpy, myConfig, EGL_NO_CONTEXT, context_attribs);
        com.checkEglError("eglCreateContext");
        if (context == EGL_NO_CONTEXT) {
            printf("eglCreateContext failed\n");
            return 0;
        }
        returnValue = eglMakeCurrent(dpy, surface, surface, context);
        com.checkEglError("eglMakeCurrent");
        if (returnValue != EGL_TRUE) {
            return 0;
        }
        eglQuerySurface(dpy, surface, EGL_WIDTH, &w);
        com.checkEglError("eglQuerySurface");
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &h);
        com.checkEglError("eglQuerySurface");
    }

    // now the default EGL surface is non-linear sRGB + FP16.
    glClearColor(1.0f, 128 / 255.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLint actualEncoding = 0;
    glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_BACK,
                                          GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &actualEncoding);
    // linear=0x2601, srgb=0x8C40
    DEBUG_PRINT("FB encoding value = 0x%x, and it is %s",
                actualEncoding, (actualEncoding==GL_SRGB ? "GL_SRGB" : "GL_LINEAR"));

    if (!std::strcmp(debugOptions, "8bit")) {
        ASSERT(actualEncoding==GL_SRGB);
        uint32_t pixel = 0;;
        glReadPixels(3, 3, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
        DEBUG_PRINT("readback pixel RGBA = (0x%x)", pixel); // BC = 188
    } else {
        ASSERT(actualEncoding==GL_LINEAR);
        // default one, debugOptions == nulptr
        GLfloat *pixel = (GLfloat *)malloc(1*1*4 * sizeof(GLfloat)); // readback 1 pixel.
        glReadPixels(3, 3, 1, 1, GL_RGBA, GL_FLOAT, pixel);
        DEBUG_PRINT("readback pixel RGBA = (%f, %f, %f, %f)", pixel[0], pixel[1], pixel[2], pixel[3]);
        //ASSERT(pixel[0] == 1.0 && pixel[1] == 128/255.0 && pixel[2] == 0.0 && pixel[3] == 1.0 );
        free(pixel);
    }

    com.makeCurrent();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return true;
}
bool EGL_ColorSpace::exitGraphics() {
    return true;
}
bool EGL_ColorSpace::isSupported() {
    return com.isEGLExtensionSupported("EGL_EXT_gl_colorspace_display_p3") &&
            com.isEGLExtensionSupported("EGL_EXT_pixel_format_float");
}

ADD_TEST(EGL_ColorSpace)
