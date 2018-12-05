#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <GLES3/gl31.h>
#include <GLES3/gl32.h>

#include "WindowSurface.h"
#include "md5.h"

// use this for debug output and cout for must output.
#define ENABLE_PRINT 1
#if ENABLE_PRINT
    #define DEBUG_PRINT(...) \
    do {                     \
        printf(__VA_ARGS__); \
        printf("\n");        \
        fflush(stdout);      \
    } while (0)
#else
#define DEBUG_PRINT(...)
#endif

#define ASSERT( x )                                                        \
do {                                                                       \
    if (!(x)) {                                                            \
        DEBUG_PRINT("ERROR. Assert at: %s line %d\n", __FILE__, __LINE__); \
    }                                                                      \
} while (0)


#define ARRAY_SIZE(a) ( sizeof(a) / sizeof((a)[0]) )

// separate the class and the name later? but now use the same one.
#define ADD_TEST(_CLASS)                                        \
int _CLASS##_main()                                             \
{                                                               \
    _CLASS test;                                                \
    if (com.initEGL()) {                                        \
        DEBUG_PRINT("EGL initialization pass.\n\n");            \
    }                                                           \
    if (!test.isSupported()) {                                  \
        DEBUG_PRINT("Test is not supported.\n\n");              \
        return 0;                                               \
    }                                                           \
    if (!test.initGraphics()) {                                 \
        DEBUG_PRINT("initGraphics failed.\n\n");                \
    }                                                           \
    test.doGraphics();                                          \
    com.afterDoGraphics();                                      \
    DEBUG_PRINT("Test PASS.\n");                                \
    return 1;                                                   \
}


class Common {
public:
    Common();
    ~Common();

    int runAllTests();

    // all gets.
    EGLint getDefaultWidth();
    EGLint getDefaultHeight();
    EGLDisplay getDefaultDisplay();

    void  checkError(const char* op);

    // EGL related functions.
    void checkEglError(const char* op);
    int isEGLExtensionSupported(const char *extension);
    int printEGLConfigurations(EGLDisplay dpy);
    void printEGLConfiguration(EGLDisplay dpy, EGLConfig config);

    int initEGL();
    bool makeCurrent();

    void handleFramebuffer();
    void afterDoGraphics();

    // GL related functions.
    void checkGlError(const char* op);
    int isGLExtensionSupported(const char *extension);
    GLuint loadShader(GLenum shaderType, const char* pSource);
    GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);

private:

    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay defaultDisplay;
    EGLContext defaultContext;
    EGLSurface defaultSurface;

    // this is get from the hard-coded.
    EGLint defaultWidth, defaultHeight;
    WindowSurface defaultWindowSurface;
    EGLNativeWindowType defaultWindow;

    GLbyte *framebuffer = nullptr;
    int framebufferSize;
    MD5 md5;
};
