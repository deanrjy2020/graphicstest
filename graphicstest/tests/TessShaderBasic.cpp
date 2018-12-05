#include "common.h"

#include <iostream>

extern Common com;

class TessShaderBasic {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

static GLuint VBO, VAO;
static GLuint shaderProgram;

// Shaders
static const GLchar * vertexShaderSource =
"#version 310 es\n"
"in highp vec4 a_position;"
"void main(void) {"
"    gl_Position = a_position;"
"}";

static const GLchar* tcs2 =
"#version 310 es\n"
"#extension GL_EXT_tessellation_shader : enable\n"
"layout(vertices = 4) out;\n"
"void main()\n"
"{\n"
"    // Pass along the vertex position unmodified  \n"
"    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n"
"\n"
"    gl_TessLevelOuter[0] = float(1);\n"
"    gl_TessLevelOuter[1] = float(32);\n"
"}\n";
static const GLchar* tes2 =
"#version 310 es\n"
"layout(isolines) in;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"void main()\n"
"{\n"
"    float u = gl_TessCoord.x;\n"
"    vec3  p0 = gl_in[0].gl_Position.xyz;\n"
"    vec3  p1 = gl_in[1].gl_Position.xyz;\n"
"    float leng = length(p1 - p0) / 2.0;\n"
"    // Linear interpolation  \n"
"    vec3 p;\n"
"    p.x = p0.x*u + p1.x*(1 - u);\n"
"    p.y = p0.y + leng*sin(u * 2 * 3.1415);\n"
"    // Transform to clip coordinates  \n"
"    gl_Position = ModelViewProjectionMatrix * vec4(p, 1);\n"
"}";

static const GLchar* tcs =
"#version 310 es\n"
"#extension GL_EXT_tessellation_shader : require\n"
"layout(vertices = 1) out;"

"void main()"
"{"
"    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"
"    gl_TessLevelOuter[0] = 3.0;"
"    gl_TessLevelOuter[1] = 3.0;"
"    gl_TessLevelOuter[2] = 3.0;"
"    gl_TessLevelOuter[3] = 3.0;"
"    gl_TessLevelInner[0] = 3.0;"
"    gl_TessLevelInner[1] = 3.0;"
"}";

static const GLchar* tes =
"#version 310 es\n"
"#extension GL_EXT_tessellation_shader : require\n"
"layout(quads) in;"

"out mediump ivec2 v_tessellationGridPosition;"

// note: No need to use precise gl_Position since position does not depend on order
"void main(void)"
"{"
    // Fill the whole viewport
"    gl_Position = vec4(gl_TessCoord.x * 2.0 - 1.0, gl_TessCoord.y * 2.0 - 1.0, 0.0, 1.0);"
    // Calculate position in tessellation grid
"    v_tessellationGridPosition = ivec2(round(gl_TessCoord.xy * float(64)));"
"}";

static const GLchar* gs =
"#version 310 es\n"
"#extension GL_EXT_geometry_shader : require\n"
"layout(triangles, invocations = 4) in;"
"layout(triangle_strip, max_vertices = 128) out;"
"in mediump ivec2 v_tessellationGridPosition[];"
"flat out highp vec4 v_color;"
"void main() {"
"    const float equalThreshold = 0.001;"
"    const float gapOffset = 0.0001; // subdivision performed by the geometry shader might produce gaps. Fill potential gaps by enlarging the output slice a little."
                                    // Input triangle is generated from an axis-aligned rectangle by splitting it in half
                                    // Original rectangle can be found by finding the bounding AABB of the triangle
"    vec4 aabb = vec4(min(gl_in[0].gl_Position.x, min(gl_in[1].gl_Position.x, gl_in[2].gl_Position.x)),"
"        min(gl_in[0].gl_Position.y, min(gl_in[1].gl_Position.y, gl_in[2].gl_Position.y)),"
"        max(gl_in[0].gl_Position.x, max(gl_in[1].gl_Position.x, gl_in[2].gl_Position.x)),"
"        max(gl_in[0].gl_Position.y, max(gl_in[1].gl_Position.y, gl_in[2].gl_Position.y)));"
    // Location in tessellation grid
"    ivec2 gridPosition = ivec2(min(v_tessellationGridPosition[0], min(v_tessellationGridPosition[1], v_tessellationGridPosition[2])));"
    // Which triangle of the two that split the grid cell
"    int numVerticesOnBottomEdge = 0;"
"    for (int ndx = 0; ndx &lt; 3; ++ndx)"
"        if (abs(gl_in[ndx].gl_Position.y - aabb.w) &lt; equalThreshold)"
"            ++numVerticesOnBottomEdge;"
"    bool isBottomTriangle = numVerticesOnBottomEdge == 2;"
    // Fill the input area with slices
    // Upper triangle produces slices only to the upper half of the quad and vice-versa
"    float triangleOffset = (isBottomTriangle) ? ((aabb.w + aabb.y) / 2.0) : (aabb.y);"
    // Each slice is a invocation
"    float sliceHeight = (aabb.w - aabb.y) / float(2 * 4);"
"    float invocationOffset = float(gl_InvocationID) * sliceHeight;"
"    vec4 outputSliceArea;"
"    outputSliceArea.x = aabb.x - gapOffset;"
"    outputSliceArea.y = triangleOffset + invocationOffset - gapOffset;"
"    outputSliceArea.z = aabb.z + gapOffset;"
"    outputSliceArea.w = triangleOffset + invocationOffset + sliceHeight + gapOffset;"
    // Draw slice
"    for (int ndx = 0; ndx &lt; 64; ++ndx)"
"    {"
"        vec4 green = vec4(0.0, 1.0, 0.0, 1.0);"
"        vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);"
"        vec4 outputColor = (((gl_InvocationID + ndx) % 2) == 0) ? (green) : (yellow);"
"        float xpos = mix(outputSliceArea.x, outputSliceArea.z, float(ndx) / float(63));"

"        gl_Position = vec4(xpos, outputSliceArea.y, 0.0, 1.0);"
"        v_color = outputColor;"
"        EmitVertex();"

"        gl_Position = vec4(xpos, outputSliceArea.w, 0.0, 1.0);"
"        v_color = outputColor;"
"        EmitVertex();"
"    }"
"}";

static const GLchar* fragmentShaderSource =
"#version 310 es\n"
"flat in mediump vec4 v_color;"
"layout(location = 0) out mediump vec4 fragColor;"
"void main(void) {"
//"    fragColor = v_color;"
"    fragColor = vec4(0.0,1.0,0.0,1.0);"
"}";

bool TessShaderBasic::initGraphics() {
    glViewport(0, 0, com.getDefaultWidth(), com.getDefaultHeight());

        // Build and compile shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // Check for compile resault
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX:COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // tcs
        GLuint tcShader = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcShader, 1, &tcs2, NULL);
        glCompileShader(tcShader);
        glGetShaderiv(tcShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(tcShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::TCS::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // tes
        GLuint teShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(teShader, 1, &tes2, NULL);
        glCompileShader(teShader);
        glGetShaderiv(teShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(teShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::TES::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        //// gs
        //GLuint gShader = glCreateShader(GL_FRAGMENT_SHADER);
        //glShaderSource(gShader, 1, &gs, NULL);
        //glCompileShader(gShader);
        //glGetShaderiv(gShader, GL_COMPILE_STATUS, &success);
        //if (!success) {
        //    glGetShaderInfoLog(gShader, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::geometry::COMPILATION_FAILED\n" << infoLog << std::endl;
        //}

        // fragmentShader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        //Link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, tcShader);
        glAttachShader(shaderProgram, teShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // Check for Linking resault
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Set up vertex data and attribute pointers
        GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f, 1.0f,   // Left
             0.5f, -0.5f, 0.0f, 1.0f,    // Right
             0.0f,  0.5f, 0.0f, 1.0f      // Top
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // Bind the VAO first, then bind and set vertex buffer(s) and attribute pointer(s)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        //tess
        glPatchParameteri(GL_PATCH_VERTICES, 1);
    return true;
}
bool TessShaderBasic::doGraphics() {
    // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_PATCHES, 0, 3);
        glBindVertexArray(0);
    return true;
}
bool TessShaderBasic::exitGraphics() {
    glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    return true;
}
bool TessShaderBasic::isSupported() {
    return true;
}

ADD_TEST(TessShaderBasic)
