#include "common.h"

extern Common com;

class GL101 {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

static const char gVertexShader[] =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char gFragmentShader[] =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

static const GLfloat gTriangleVertices[] = {
    0.0f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f };
static GLuint gProgram;
static GLuint gvPositionHandle;

bool GL101::initGraphics() {
    gProgram = com.createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    com.checkGlError("glGetAttribLocation");
    DEBUG_PRINT("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);

    //glViewport(0, 0, com.getDefaultWidth(), com.getDefaultHeight());
    com.checkGlError("glViewport");
    return true;
}
bool GL101::doGraphics() {
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    com.checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    com.checkGlError("glClear");

    glUseProgram(gProgram);
    com.checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    com.checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    com.checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    com.checkGlError("glDrawArrays");
    return true;
}
bool GL101::exitGraphics() {
    return true;
}
bool GL101::isSupported() {
    return true;
}

ADD_TEST(GL101)
