#include "common.h"

extern Common com;

class XFBBasic {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

static const char gVertexShader[] =
    "#version 300 es\n"
    "in vec4 vPosition;\n"
    "out vec2 outValue;\n"
    "void main() {\n"
    "  outValue = vPosition.xy;\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char gFragmentShader[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 outValue;\n" //do nothing
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

static const GLfloat gTriangleVertices[] = {
    0.0f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f };
static GLuint gProgram;
static GLuint gvPositionHandle;

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = com.loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = com.loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        com.checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        com.checkGlError("glAttachShader");

        // Before linking the program, we have to tell OpenGL which output attributes
        // we want to capture into a buffer.
        const GLchar* feedbackVaryings[] = { "outValue" };
        glTransformFeedbackVaryings(program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

        // a GPU memory to receive the XFB data.
        GLuint tbo;
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(gTriangleVertices), NULL, GL_STATIC_READ);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo); //very important
        com.checkGlError("glBindBufferBase");

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
                    DEBUG_PRINT("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

bool XFBBasic::initGraphics() {
    gProgram = createProgram(gVertexShader, gFragmentShader);
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
bool XFBBasic::doGraphics() {

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


    glBeginTransformFeedback(GL_TRIANGLES);
    com.checkGlError("glBeginTransformFeedback");
    glDrawArrays(GL_TRIANGLES, 0, 3); // expected pass.
    com.checkGlError("glDrawArrays1");

#if 0
    unsigned int idx[] = {1, 2, 3};
    glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_BYTE, idx); // expected ERROR
    com.checkGlError("glDrawElements");
    glDrawArrays(GL_TRIANGLES, 0, 3); // should pass because the glDrawArrays1 is passing
    com.checkGlError("glDrawArrays2");
#endif

    glFlush();

    glEndTransformFeedback();
    com.checkGlError("glEndTransformFeedback");

    // Map the transform feedback buffer to local address space.
    com.checkGlError(" pre-glMapBufferRange ");
    float* mappedBuffer =  (float*) glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER,
                                                     0, sizeof(gTriangleVertices), GL_MAP_READ_BIT);
    com.checkGlError(" glMapBufferRange");
    // We can get all the gTriangleVertices here
    for (int i = 0; i< 6; ++i) {
        DEBUG_PRINT("XFB data: %f .\n", mappedBuffer[i]);
        if (mappedBuffer[i] != gTriangleVertices[i]) {
            DEBUG_PRINT("ERROR, this value is not correct.");
        }
    }

    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    return true;
}
bool XFBBasic::exitGraphics() {
    return true;
}
bool XFBBasic::isSupported() {
    return true;
}

ADD_TEST(XFBBasic)
