#include "common.h"

#include <iostream>

extern Common com;

class GeometryShaderBasic {
public:
    bool initGraphics();
    bool doGraphics();
    bool exitGraphics();
    bool isSupported();
private:
};

// https://learnopengl-cn.readthedocs.io/zh/latest/04%20Advanced%20OpenGL/09%20Geometry%20Shader/
static GLuint VBO, VAO;
static GLuint shaderProgram;

// Shaders
static const GLchar * vertexShaderSource =
"#version 330 core\n"
// get the pos and color from VAO.
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec3 color;\n"
"out VS_OUT{"
"    vec3 color;"
"} vs_out;"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x,position.y,0.0,1.0);\n"
"vs_out.color = color;"
"}\n";

static const GLchar* geometryShaderPassThrough =
"#version 330 core\n" // must have \n for the version line
"layout(points) in;"
"layout(points, max_vertices = 1) out;"
"void main() {"
"    gl_Position = gl_in[0].gl_Position;" // Why array? 因为大多数渲染基本图形由一个以上顶点组成，几何着色器接收一个基本图形的所有顶点作为它的输入
"    EmitVertex();" //每次我们调用EmitVertex，当前设置到gl_Position的向量就会被添加到基本图形上
"    EndPrimitive();" //调用EndPrimitive，所有为这个基本图形发射出去的顶点都将结合为一个特定的输出渲染基本图形,
//重复调用EndPrimitive就能生成多个基本图形
"}";

static const GLchar* geometryShaderHouse =
"#version 330 core\n"
"#extension GL_EXT_geometry_shader : enable\n"
"layout (points) in;\n" // input one point
"layout (triangle_strip, max_vertices = 5) out;\n"  //output 5 points and link the 5 points with triangle_strip
// match the struct in VS, get from the VS.
"in VS_OUT{"
"    vec3 color;"
"} gs_in[];"
//output to FS
"out vec3 fColor;"
"void build_house(vec4 position)\n"
"{\n"
"fColor = gs_in[0].color;" //只有一个输出颜色，所以直接设置为gs_in[0]
//下面指明哪5个点, 即这5个点和input的这个点什么关系(相对位置).
"    gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f);"// 1:左下角
"    EmitVertex();"
"    gl_Position = position + vec4(0.2f, -0.2f, 0.0f, 0.0f);"// 2:右下角
"    EmitVertex();"
"    gl_Position = position + vec4(-0.2f, 0.2f, 0.0f, 0.0f);"// 3:左上
"    EmitVertex();"
"    gl_Position = position + vec4(0.2f, 0.2f, 0.0f, 0.0f);"// 4:右上
"    EmitVertex();"
"    gl_Position = position + vec4(0.0f, 0.4f, 0.0f, 0.0f);"// 5:屋顶
"    EmitVertex();"
"    EndPrimitive();\n"
"}\n"
"void main() {"
"    build_house(gl_in[0].gl_Position);"
"}";

static const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 color;\n"
"in vec3 fColor;"
"void main()\n"
"{\n"
//"color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"color = vec4(fColor, 1.0f);\n"
"}\n\0";

bool GeometryShaderBasic::initGraphics() {
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
    // geometryShader
    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderHouse, NULL);
    //glShaderSource(geometryShader, 1, &geometryShaderPassThrough, NULL);
    glCompileShader(geometryShader);
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
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
    glAttachShader(shaderProgram, geometryShader);
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
    GLfloat points[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the VAO first, then bind and set vertex buffer(s) and attribute pointer(s)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // pass the values to position
    glEnableVertexAttribArray(0);
    // first argu is /*index of the generic vertex attribute*/, need to match the argu of glEnableVertexAttribArray()
    glVertexAttribPointer(0, 2/*size*/, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat)/*stride*/, (GLvoid*)0 /*offset*/);

    // pass the values to color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}
bool GeometryShaderBasic::doGraphics() {
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 4);
    glBindVertexArray(0);
    return true;
}
bool GeometryShaderBasic::exitGraphics() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return true;
}
bool GeometryShaderBasic::isSupported() {
    return true;
}

ADD_TEST(GeometryShaderBasic)
