#include "common.h"
#include <string.h>

extern Common com;

class EGL_GL_Info {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

// these functions are copued from somewhere, just use the DEBUG_PRINT for keeping the format,
// no cout yet.

static void printEGLConfiguration(EGLDisplay defaultDisplay, EGLConfig config)
{
#define X(VAL) {VAL, #VAL}
    struct {EGLint attribute; const char* name;} names[] = {
        X(EGL_BUFFER_SIZE),
                X(EGL_ALPHA_SIZE),
                X(EGL_BLUE_SIZE),
                X(EGL_GREEN_SIZE),
                X(EGL_RED_SIZE),
                X(EGL_DEPTH_SIZE),
                X(EGL_STENCIL_SIZE),
                X(EGL_CONFIG_CAVEAT),
                X(EGL_CONFIG_ID),
                X(EGL_LEVEL),
                X(EGL_MAX_PBUFFER_HEIGHT),
                X(EGL_MAX_PBUFFER_PIXELS),
                X(EGL_MAX_PBUFFER_WIDTH),
                X(EGL_NATIVE_RENDERABLE),
                X(EGL_NATIVE_VISUAL_ID),
                X(EGL_NATIVE_VISUAL_TYPE),
                X(EGL_SAMPLES),
                X(EGL_SAMPLE_BUFFERS),
                X(EGL_SURFACE_TYPE),
                X(EGL_TRANSPARENT_TYPE),
                X(EGL_TRANSPARENT_RED_VALUE),
                X(EGL_TRANSPARENT_GREEN_VALUE),
                X(EGL_TRANSPARENT_BLUE_VALUE),
                X(EGL_BIND_TO_TEXTURE_RGB),
                X(EGL_BIND_TO_TEXTURE_RGBA),
                X(EGL_MIN_SWAP_INTERVAL),
                X(EGL_MAX_SWAP_INTERVAL),
                X(EGL_LUMINANCE_SIZE),
                X(EGL_ALPHA_MASK_SIZE),
                X(EGL_COLOR_BUFFER_TYPE),
                X(EGL_RENDERABLE_TYPE),
                X(EGL_CONFORMANT),
    };
#undef X

    for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); ++j) {
        EGLint value = -1;
        EGLint returnVal = eglGetConfigAttrib(defaultDisplay, config, names[j].attribute, &value);
        EGLint error = eglGetError();
        if (returnVal && error == EGL_SUCCESS) {
            DEBUG_PRINT(" %s: ", names[j].name);
            DEBUG_PRINT("%d (0x%x)", value, value);
        }
    }
    DEBUG_PRINT("\n");
}

static int printEGLConfigurations(EGLDisplay defaultDisplay)
{
    EGLint numConfig = 0;
    EGLint returnVal = eglGetConfigs(defaultDisplay, NULL, 0, &numConfig);
    if (!returnVal) {
        return 0;
    }

    DEBUG_PRINT("Number of EGL configuration: %d\n", numConfig);

    EGLConfig* configs = (EGLConfig*) malloc(sizeof(EGLConfig) * numConfig);
    if (! configs) {
        DEBUG_PRINT("Could not allocate configs.\n");
        return 0;
    }

    returnVal = eglGetConfigs(defaultDisplay, configs, numConfig, &numConfig);
    if (!returnVal) {
        free(configs);
        return 0;
    }

    for(int i = 0; i < numConfig; ++i) {
        DEBUG_PRINT("\nConfiguration (this is not EGL_CONFIG_ID) %d\n", i);
        printEGLConfiguration(defaultDisplay, configs[i]);
    }

    free(configs);
    return 1;
}

bool EGL_GL_Info::initGraphics() {
    return true;
}
bool EGL_GL_Info::doGraphics() {
    // Print all the configs.
    if (!printEGLConfigurations(com.getDefaultDisplay())) {
        DEBUG_PRINT("printEGLConfigurations failed\n");
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        return false;
    }

    // Print the EGL ext.
    {
        printf("\n");
        printf("EGL_VENDOR                   : %s\n", eglQueryString(com.getDefaultDisplay(), EGL_VENDOR));
        printf("EGL_CLIENT_APIS              : %s\n", eglQueryString(com.getDefaultDisplay(), EGL_CLIENT_APIS));
        printf("EGL_VERSION                  : %s\n", eglQueryString(com.getDefaultDisplay(), EGL_VERSION));

        const char *src = eglQueryString(com.getDefaultDisplay(), EGL_EXTENSIONS);

        // Replace spaces with new lines so that each extension is printed
        // into own line for better readability.
        size_t size = strlen(src) + 1;

        char *dst = (char*)malloc(size * sizeof(char));
        for (size_t i = 0; i < size; i++) {
            dst[i] = (src[i] == ' ') ? '\n' : src[i];
        }
        printf("EGL_EXTENSIONS               : \n%s\n", dst);
        free(dst);
        dst = NULL;
    }

    // Print the GLES ext.
    {
        printf("\n");
        printf("GL_VENDOR                   : %s\n", glGetString(GL_VENDOR));
        printf("GL_RENDERER                 : %s\n", glGetString(GL_RENDERER));
        printf("GL_VERSION                  : %s\n", glGetString(GL_VERSION));
        // Don't print if shaders are not supported.
        const GLubyte* s = glGetString(GL_SHADING_LANGUAGE_VERSION);
        if (glGetError() == GL_NO_ERROR) {
            printf("GL_SHADING_LANGUAGE_VERSION : %s\n", s);
        }

        const GLubyte *src = glGetString(GL_EXTENSIONS);

        // Replace spaces with new lines so that each extension is printed
        // into own line for better readability.
        GLsizei size = strlen((char*)src) + 1;
        GLubyte *dst = new GLubyte[size];
        for (GLsizei i = 0; i < size; i++) {
            dst[i] = (src[i] == ' ') ? '\n' : src[i];
        }
        printf("GL_EXTENSIONS               : \n%s\n", dst);
        delete [] dst;
    }

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    return true;
}
bool EGL_GL_Info::exitGraphics() {
    return true;
}
bool EGL_GL_Info::isSupported() {
    return true;
}

ADD_TEST(EGL_GL_Info)
