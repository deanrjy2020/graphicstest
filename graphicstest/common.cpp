#include "common.h"
#include "tests.h"
#include "WindowSurface.h"

#include <thread>
#include <chrono>
#include <iomanip>
#include <iostream>

extern int defaultWidth;
extern int defaultHeight;
extern char *runSingleTestName;
extern bool interactive;
extern char *outputDir;
extern char *debugOptions;

Common::Common() {
    framebufferSize = defaultWidth * defaultHeight * 4;
    if (!framebuffer) {
        framebuffer = (GLbyte *) malloc (this->framebufferSize);
    }
}
Common::~Common() {
    if (framebuffer) {
        free(framebuffer);
        framebuffer = nullptr;
    }
}

//===================================================================================not static

// declare the functions, for the non-string use ##.
//void gl101_main();
#define F(x) int x##_main();
    TESTS(F)
#undef F

// for the string, use #
static std::string testNameList[] = {
#define F(x) #x,
    TESTS(F)
#undef F
};

typedef int (*PtrFn)();
static PtrFn testMainFuncList[] = {
    //gl101_main,
#define F(x) &x##_main,
    TESTS(F)
#undef F
};

int Common::runAllTests() {

    for (int i=0; i < ARRAY_SIZE(testMainFuncList); ++i) {
        // if the runSingleTestName is null, we run the test (entire list)
        // or if the runSingleTestName == current test case, we run the test.
        if (!runSingleTestName || !std::strcmp(runSingleTestName,testNameList[i].c_str())) {

            // for the testName, set left alignment and set the width to 20.
            std::cout << "Do " << std::left << std::setw(20) << testNameList[i]
                      << " : done (md5: " << md5.toString() << ")" << " res here" << std::endl;

            testMainFuncList[i]();

            // todo, let the user know if test is not supported or exist.
            if (runSingleTestName) {
                break;
            }
        }
    }
    return 1;
}

//===================================================================================others
EGLint Common::getDefaultWidth()
{
    return defaultWidth;
}
EGLint Common::getDefaultHeight()
{
    return this->defaultHeight;
}

EGLDisplay Common::getDefaultDisplay()
{
    return this->defaultDisplay;
}

void Common::checkError(const char* op) {
    checkEglError(op);
    checkGlError(op);
}

//===================================================================================EGL

void Common::checkEglError(const char* op)
{
    EGLint error = eglGetError();
    if (error != EGL_SUCCESS) {
        DEBUG_PRINT("ERROR. after %s() eglError (0x%x)\n", op, error);
    }
}

// The program has only EGL instance and if you want different, create by your own.
int Common::initEGL()
{
    checkEglError("Before Initialize EGL");
    defaultDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (defaultDisplay == EGL_NO_DISPLAY) {
        DEBUG_PRINT("eglGetDisplay returned EGL_NO_DISPLAY.\n");
        return 0;
    }

    ASSERT(eglInitialize(defaultDisplay, &majorVersion, &minorVersion));

    defaultWindow = defaultWindowSurface.getSurface();

    EGLint numConfigs = 0, configId = 0;
    EGLConfig myConfig = {0};
    EGLint s_configAttribs[] = {
        EGL_RED_SIZE,                   8,
        EGL_GREEN_SIZE,                 8,
        EGL_BLUE_SIZE,                  8,
        EGL_ALPHA_SIZE,                 8,
        EGL_SURFACE_TYPE,               EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,            EGL_OPENGL_ES2_BIT,
        EGL_NONE };
    eglChooseConfig(defaultDisplay, s_configAttribs, &myConfig, 1, &numConfigs);

    if (numConfigs == 0) {
        DEBUG_PRINT("Failed to choose a config that supports EGL_MUTABLE_RENDER_BUFFER_BIT_KHR.\n");
    }
    eglGetConfigAttrib(defaultDisplay, myConfig, EGL_CONFIG_ID, &configId);

    EGLint winAttribs[] = {EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
    defaultSurface = eglCreateWindowSurface(defaultDisplay, myConfig, defaultWindow, winAttribs);
    if (defaultSurface == EGL_NO_SURFACE) {
        DEBUG_PRINT("gelCreateWindowSurface failed.\n");
        return 0;
    }

    // EGL_CONTEXT_CLIENT_VERSION only tell the major version. and only for es?
    // if we need the minor version, use below.
    // it can be used for GL and ES.
    // https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_create_context.txt
    EGLint context_attribs[] = { EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                                 EGL_CONTEXT_MINOR_VERSION_KHR, 2,
                                 EGL_NONE };

    defaultContext = eglCreateContext(defaultDisplay, myConfig, EGL_NO_CONTEXT, context_attribs);
    if (defaultContext == EGL_NO_CONTEXT) {
        DEBUG_PRINT("eglCreateContext failed\n");
        return 0;
    }

    makeCurrent();

    eglQuerySurface(defaultDisplay, defaultSurface, EGL_WIDTH, &defaultWidth);
    eglQuerySurface(defaultDisplay, defaultSurface, EGL_HEIGHT, &defaultHeight);
    //GLint dim = defaultWidth < h ? w : defaultHeight;
    DEBUG_PRINT("Window dimensions: %d x %d, EGL(majorVersion, minorVersion)=(%d.%d)\n",
                defaultWidth, defaultHeight,
                majorVersion, minorVersion);

    checkEglError("EGL initialization done.");

    return 1;
}

bool Common::makeCurrent() {
    ASSERT(eglMakeCurrent(defaultDisplay, defaultSurface, defaultSurface, defaultContext));
    return true;
}

void Common::handleFramebuffer() {
    // read the default FBO for the md5 code.
    // framebuffer should be alloced.
    memset(this->framebuffer, 0, this->framebufferSize);
    glReadPixels(0, 0, defaultWidth, defaultHeight,
                 GL_RGBA, GL_UNSIGNED_BYTE, this->framebuffer);

    md5.reset();
    md5.update(this->framebuffer, this->framebufferSize);

    // save the output FB if needed.
    if (outputDir) {
        // save to png.
    }
}
void Common::afterDoGraphics()
{
    //handleFramebuffer();

    eglSwapBuffers(this->defaultDisplay, this->defaultSurface);
    checkEglError("eglSwapBuffers");

    // Sleep 10 s if it is interactive mode.
    if (interactive) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}

//===================================================================================GL
void Common::checkGlError(const char* op)
{
    for (GLint error = glGetError(); error; error = glGetError()) {
        DEBUG_PRINT("ERROR. after %s() glError (0x%x)\n", op, error);
    }
}

GLuint Common::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    fprintf(stderr, "Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint Common::createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    fprintf(stderr, "Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

// this is the old/original way to query, but if you want to use the extension function,
// there is a lot of work (like, function point.)
// see here for the function point. https://www.opengl.org/archives/resources/features/OGLextensions/
// use glew: http://glew.sourceforge.net/basic.html
int Common::isGLExtensionSupported(const char *extension) {
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    /* Extension names should not have spaces. */
    where = (GLubyte *)strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;
    extensions = glGetString(GL_EXTENSIONS);

    /* It takes a bit of care to be fool-proof about parsing the
    OpenGL extensions string. Don't be fooled by sub-strings,
    etc. */
    start = extensions;
    for (;;) {
        where = (GLubyte *)strstr((const char *)start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;
        start = terminator;
    }
    return 0;
}

// EGL ext.
int Common::isEGLExtensionSupported(const char *extension) {
    const char *extensions = NULL;
    const char *start;
    char *where, *terminator;

    /* Extension names should not have spaces. */
    where = (char *)strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;
    extensions = eglQueryString(this->defaultDisplay, EGL_EXTENSIONS);

    /* It takes a bit of care to be fool-proof about parsing the
    OpenGL extensions string. Don't be fooled by sub-strings,
    etc. */
    start = extensions;
    for (;;) {
        where = (char *)strstr((const char *)start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;
        start = terminator;
    }
    return 0;
}
