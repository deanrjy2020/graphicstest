#include "common.h"

extern Common com;

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
#if 0
    // new EGL
    {
        EGLBoolean returnValue;
        EGLConfig myConfig = {0};

        EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
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

        //        if (!printEGLConfigurations(dpy)) {
        //            printf("printEGLConfigurations failed\n");
        //            return 0;
        //        }
        //        com.checkEglError("printEGLConfigurations");

        WindowSurface windowSurface;
        EGLNativeWindowType window = windowSurface.getSurface();


        EGLint configId = 0, numConfigs;
        eglChooseConfig(dpy, s_configAttribs, &myConfig, 1, &numConfigs);
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

#endif

    com.makeCurrent();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
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
