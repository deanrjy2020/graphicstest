#include "common.h"

extern Common com;

class PathRenderingBasic {
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

// looks like there is no glGen function for the path obj
static GLuint pathObj = 42;

bool PathRenderingBasic::initGraphics() {
    //glViewport(0, 0, com.getDefaultWidth(), com.getDefaultHeight());
    return true;
}

bool PathRenderingBasic::doGraphics() {

    int filling = 1;
    int stroking=1;
    int testClip=0;

    int OpenGL_ES_any = 1;
    int avoid_queries = 1;
    int verbose = 0;
    int use_stencil_then_cover = 0;
    {
            GLint errpos = -666;

            const char *svgPathString =
              // star
              "M100,180 L40,10 L190,120 L10,120 L160,10 z"
              // heart
              "M300 300 C 100 400,100 200,300 100,500 200,500 400,300 300Z"
              ;

            const char *psPathString =
              // star
              "100 180 moveto"
              " 40 10 lineto 190 120 lineto 10 120 lineto 160 10 lineto closepath"
              // heart
              " 300 300 moveto"
              " 100 400 100 200 300 100 curveto"
              " 500 200 500 400 300 300 curveto closepath";

            const char *psPathStringArc =
    "256.94 87.87 moveto\n"
    "267.27 473.81 85.87 351.12 113.22 247.39 curveto\n"
    "-71.27 -52.56 rmoveto\n"
    "184.04 491.72 267.70 382.84 323.24 383.57 curveto\n"
    "411.48 75.97 225.17 267.60 124.88 arcn\n"
    "closepath\n";

            // PRLM = PostScript Reference Language Manual
            // PLRM Example 4.2
            const char *psPathString42 =
    "ucache % This is optional\n"
    "100  200  400  500  setbbox % This is required\n"
    "150  200  moveto\n"
    "250  200  400  390  400  460  curveto\n"
    "400  480  350  500  250  500  curveto\n"
    "100  400  lineto\n"
    "closepath\n";

    #if 0
            const char *psPathString42radix =
    "ucache % This is optional\n"
    "2#1100100  16#C8  10#400  36#Dw  setbbox % This is required\n"
    "3#12120  8#310  moveto\n"
    "10#250  10#200  10#400  10#390  10#400  10#460  curveto\n"
    "10#400  10#480  10#350  10#500  10#250  10#500  curveto\n"
    "10#100  10#400  lineto\n"
    "closepath\n";

            // PLRM Example 4.3
            const char *psPathString43 =
    "{100  200  400  500\n"
    "150  200\n"
    "250  200  400  390  400  460\n"
    "400  480  350  500  250  500\n"
    "100  400\n"
    "}\n"
    "<0B  00  01  22  05  03  0A  >\n";
    #endif

            // PLRM Example 4.4
            const char *psPathString44 =
    "<95200014\n"
    "0064  00C8  0190  01F4\n"
    "0096  00C8\n"
    "00FA  00C8  0190  0186  0190  01CC\n"
    "0190  01E0  015E  01F4  00FA  01F4\n"
    "0064  0190\n"
    ">\n"
    " <0B  00  01  22  05  03  0A  >\n";

            // PLRM Example 4.4 (in binary)
            const GLubyte psPathString44ub[] = {
                0x95,0x20,0x00,0x14,
                0x00,0x64,0x00,0xC8,0x01,0x90,0x01,0xF4,
                0x00,0x96,0x00,0xC8,
                0x00,0xFA,0x00,0xC8,0x01,0x90,0x01,0x86,0x01,0x90,0x01,0xCC,
                0x01,0x90,0x01,0xE0,0x01,0x5E,0x01,0xF4,0x00,0xFA,0x01,0xF4,
                0x00,0x64,0x01,0x90,
                0x8E,0x07,0x0B,0x00,0x01,0x22,0x05,0x03,0x0A
            };

            //const GLubyte psPathString85[] =
            const char psPathString85[] =
    // Caution: avoid trigraphs!  Because ??! = |, use ?\?!
    "<~Pop&@!+Z)N!K[?S!1!W+!;Z]:!K[>:!K[?+!K[?\?!F>g!!;Z`g!+Z+l$3~>\n"
    "< 0B 00 01 22 05 03 0A >";

            const char psPathString85two[] =
    // Caution: avoid trigraphs!  Because ??! = |, use ?\?!
    "<~Pop&@!+Z)N!K[?S!1!W+!;Z]:!K[>:!K[?+!K[?\?!F>g!!;Z`g!+Z+l$3~> \t\r\n"
    "<~$NL2O\"To)~>";  // note escaped "

            static const GLubyte pathCommands[] =
              { GL_MOVE_TO_NV, GL_LINE_TO_NV, GL_LINE_TO_NV, GL_LINE_TO_NV,
                GL_LINE_TO_NV, GL_CLOSE_PATH_NV,
                'M', 'C', 'C', 'Z'
            };  // character aliases
            static const GLshort pathCoords[12][2] =
              { {100,10}, {40,180}, {190,60}, {10,60}, {160,180},
                {300,300}, {100,400}, {100,200}, {300,100},
                {500,200}, {500,400}, {300,300} };

            GLuint program= 0;
            GLint color = 0;
            if (OpenGL_ES_any) {
                const GLchar *source = "#extension GL_ARB_separate_shader_objects : enable\n"
                                       "precision highp float;\n"
                                       "uniform vec3 color;\n"
                                       "void main() {\n"
                                       "    gl_FragColor = vec4(color, 1.0);\n"
                                       "}\n";

                //program = nvglCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &source);
                program = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &source);
                glUseProgram(program);
                color = glGetUniformLocation(program, "color");
            }

     /* Here is an example of specifying and then rendering a five-point
        star and a heart as a path using Scalable Vector Graphics (SVG)
        path description syntax: */

            glPathStringNV(pathObj, GL_PATH_FORMAT_SVG_NV,
                           (GLsizei)strlen(svgPathString), svgPathString);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }

            if (!avoid_queries) {
                GLint iv[5];
                GLfloat fv[10];
                GLubyte path_ub[4];
                int i;

//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_STROKE_WIDTH_NV, iv);
//                fv[0] = -667;
//                glGetPathParameterfvNV(pathObj, GL_PATH_STROKE_WIDTH_NV, fv);
//                assert((iv[0]+0.5f > fv[0]) && (iv[0]-0.5f < fv[0]));

                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_INITIAL_END_CAP_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_TERMINAL_END_CAP_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_JOIN_STYLE_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_MITER_LIMIT_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_INITIAL_DASH_CAP_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_TERMINAL_DASH_CAP_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_DASH_OFFSET_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_CLIENT_LENGTH_NV, iv);

                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_FILL_MODE_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_FILL_MASK_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_FILL_COVER_MODE_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_STROKE_COVER_MODE_NV, iv);
                iv[0] = -666;
                glGetPathParameterivNV(pathObj, GL_PATH_STROKE_MASK_NV, iv);

//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_SAMPLE_QUALITY_NV, iv);
//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_STROKE_OVERSAMPLE_COUNT_NV, iv);

//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_COMMAND_COUNT_NV, iv);
//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_COORD_COUNT_NV, iv);
//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_DASH_ARRAY_COUNT_NV, iv);

//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_COMPUTED_LENGTH_NV, iv);

//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_OBJECT_BOUNDING_BOX_NV, iv);
//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_FILL_BOUNDING_BOX_NV, iv);
//                iv[0] = -666;
//                glGetPathParameterivNV(pathObj, GL_PATH_STROKE_BOUNDING_BOX_NV, iv);

                for (i=0; i<10; i++) {
                    fv[i] = -666;
                }
                glGetPathMetricsNV(GL_GLYPH_WIDTH_BIT_NV, 1, GL_UNSIGNED_INT, &pathObj, 0, 0, fv);

                for (i=0; i<10; i++) {
                    fv[i] = -666;
                }
                glGetPathMetricsNV(GL_GLYPH_WIDTH_BIT_NV, 1, GL_UNSIGNED_INT, &pathObj, 0, 0, fv);
                path_ub[0] = 2;
                path_ub[1] = 2;
                path_ub[2] = 2;
                path_ub[3] = 2;
                glGetPathMetricsNV(GL_GLYPH_WIDTH_BIT_NV|GL_GLYPH_HEIGHT_BIT_NV, 4, GL_UNSIGNED_BYTE, path_ub, pathObj-2, 0, fv);
                for (i=0; i<10; i++) {
                    fv[i] = -666;
                }
                glGetPathMetricsNV(GL_GLYPH_WIDTH_BIT_NV|GL_GLYPH_HEIGHT_BIT_NV, 2, GL_UNSIGNED_BYTE, path_ub, pathObj-2, sizeof(GLfloat)*3, fv);

//                if (!dumpPathObjectInfo(pathObj)) {
//                    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red is bad
//                    glClear(GL_COLOR_BUFFER_BIT);
//                    return;
//                }
            }

     /* Alternatively applications oriented around the PostScript imaging
        model can use the PostScript user path syntax instead: */

            glPathStringNV(pathObj, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)strlen(psPathString), psPathString);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }

            glPathStringNV(pathObj, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)strlen(psPathStringArc), psPathStringArc);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }

            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)strlen(psPathString42), psPathString42);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
    #if 0  // this particular example isn't actually supported
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)strlen(psPathString42radix), psPathString42radix);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                (GLsizei)strlen(psPathString43), psPathString43);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
    #endif
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)sizeof(psPathString44ub), psPathString44ub);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                (GLsizei)strlen(psPathString44), psPathString44);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                           //(GLsizei)strlen(psPathString85), psPathString85);
                           (GLsizei)strlen(psPathString85), psPathString85);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }
            glPathStringNV(pathObj+1, GL_PATH_FORMAT_PS_NV,
                           (GLsizei)strlen(psPathString85two), psPathString85two);
            if (!avoid_queries) {
                errpos = -666;
                glGetIntegerv(GL_PATH_ERROR_POSITION_NV, &errpos);
                if (verbose) {
                    printf("GL_PATH_ERROR_POSITION_NV = %d\n", errpos);
                }
            }


     /* The PostScript path syntax also supports compact and precise binary
        encoding and includes PostScript-style circular arcs.

        Or the path's command and coordinates can be specified explicitly: */

            if (testClip) {
                // Just draw the heart
                const char *heart = "M300 300 C 100 400,100 200,300 100,500 200,500 400,300 300Z";
                glPathStringNV(pathObj, GL_PATH_FORMAT_SVG_NV,
                       (GLsizei)strlen(heart), heart);
            } else {
                glPathCommandsNV(pathObj, 10, pathCommands, 24, GL_SHORT, pathCoords);
            }


     /* Before rendering to a window with a stencil buffer, clear the stencil
        buffer to zero and the color buffer to black: */

            glClearStencil(0);
            glClearColor(0.1,0.8,0.2,0);
            glStencilMask(~0);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

     /* Use an orthographic path-to-clip-space transform to map the
        [0..500]x[0..400] range of the star's path coordinates to the [-1..1]
        clip space cube: */

            if (!OpenGL_ES_any) {
//                glMatrixLoadIdentityEXT(GL_PROJECTION);
//                glMatrixOrthoEXT(GL_MODELVIEW, 0, 500, 0, 400, -1, 1);
            } else {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glOrthof(0, 500, 0, 400, -1, 1);
            }
            // This matrix causes the cover geometry to intersect w=0
            if (testClip) {
                float m[16] = {
                    1,0,0,0,
                    0,1,0,-1,
                    0,0,1,0,
                    0,0,0,1,
                };
                if (!OpenGL_ES_any) {
                    //glMatrixLoadfEXT(GL_PROJECTION, m);
                } else {
                    glMatrixMode(GL_PROJECTION);
                    glLoadMatrixf(m);
                }
            }

     /* Stencil the path: */
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, 0, 0);
            if (filling) {
                if (use_stencil_then_cover) {
                    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
                    if (!OpenGL_ES_any) {
                        //glColor3f(1,1,0); // yellow
                    } else {
                        glUniform3f(color,1,1,0);
                    }
                    glStencilThenCoverFillPathNV(pathObj, GL_COUNT_UP_NV, 0x1F, GL_BOUNDING_BOX_NV);
                } else {
                    glStencilFillPathNV(pathObj, GL_COUNT_UP_NV, 0x1F);

                    /* The 0x1F mask means the counting uses with modulo-32 arithmetic. In
                    principle the star's path is simple enough (having a maximum winding
                    number of 2) that modulo-4 arithmetic would be sufficient so the
                    mask could be 0x3.  Or a mask of all 1's (~0) could be used to count
                    with all available stencil bits.

                    Now that the coverage of the star and the heart have been rasterized
                    into the stencil buffer, cover the path with a non-zero fill style
                    (indicated by the GL_NOTEQUAL stencil function with a zero reference
                    value): */

                    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
                    if (!OpenGL_ES_any) {
                        //glColor3f(1,1,0); // yellow
                    } else {
                        glUniform3f(color,1,1,0);
                    }
                    glCoverFillPathNV(pathObj, GL_BOUNDING_BOX_NV);
                }
            }

     /* The result is a yellow star (with a filled center) to the left of
        a yellow heart.

        The GL_ZERO stencil operation ensures that any covered samples
        (meaning those with non-zero stencil values) are zero'ed when
        the path cover is rasterized. This allows subsequent paths to be
        rendered without clearing the stencil buffer again.

        A similar two-step rendering process can draw a white outline
        over the star and heart.

        Before rendering, configure the path object with desirable path
        parameters for stroking.  Specify a wider 6.5-unit stroke and
        the round join style: */

            if (stroking) {
                glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);
    #if 0
                glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_BEVEL_NV);
    #endif
                glPathParameterfNV(pathObj, GL_PATH_STROKE_WIDTH_NV, 10.5);

                /*  Now stencil the path's stroked covered into the stencil buffer,
                setting the stencil to 0x1 for all stencil samples within the
                transformed path. */

                glStencilFunc(GL_ALWAYS, 0, 0);
                if (use_stencil_then_cover) {

                    /*  Cover the path's stroked coverage (with a hull this time instead
                    of a bounding box; the choice doesn't really matter here) while
                    stencil testing that writes white to the color buffer and again
                    zero the stencil buffer. */

                    glStencilFunc(GL_NOTEQUAL, 0, 0x1);
                    if (!OpenGL_ES_any) {
                        //glColor3f(1,1,1); // white
                    } else {
                        glUniform3f(color, 1.0, 1.0, 1.0);
                    }
                    glStencilThenCoverStrokePathNV(pathObj, 0x1, ~0, GL_CONVEX_HULL_NV);
                } else {
                    glStencilStrokePathNV(pathObj, 0x1, ~0);

                    /*  Cover the path's stroked coverage (with a hull this time instead
                    of a bounding box; the choice doesn't really matter here) while
                    stencil testing that writes white to the color buffer and again
                    zero the stencil buffer. */

                    glStencilFunc(GL_NOTEQUAL, 0, 0x1);
                    if (!OpenGL_ES_any) {
                        //glColor3f(1,1,1); // white
                    } else {
                        glUniform3f(color, 1.0, 1.0, 1.0);
                    }
                    glCoverStrokePathNV(pathObj, GL_CONVEX_HULL_NV);
                }
            }
            if (OpenGL_ES_any) {
                glUseProgram(0);
                glDeleteProgram(program);
            }
    }
    return true;
}
#if 0
bool PathRenderingBasic::doGraphics() {

    // set up for ES, doesn't need for GL.
    GLuint program= 0;
    GLint color = 0;
    const GLchar *source = "#extension GL_ARB_separate_shader_objects : enable\n"
                           "precision highp float;\n"
                           "uniform vec3 color;\n"
                           "void main() {\n"
                           "    gl_FragColor = vec4(color, 1.0);\n"
                           "}\n";

    program = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &source);
    glUseProgram(program);
    color = glGetUniformLocation(program, "color");
    DEBUG_PRINT("FS prg = %d, color = %d\n", program, color);


    // Using Scalable Vector Graphics (SVG) path description syntax:
    const char *svgPathString =
            // star
            "M100,180 L40,10 L190,120 L10,120 L160,10 z"
            // heart
            "M300 300 C 100 400,100 200,300 100,500 200,500 400,300 300Z";
    glPathStringNV(pathObject, GL_PATH_FORMAT_SVG_NV,
                   (GLsizei)strlen(svgPathString), svgPathString);

    // Before rendering to a window with a stencil buffer,
    // clear the stencil buffer to zero and the color buffer to black
    glClearStencil(0);
    glClearColor(0, 0, 0, 0);
    // 0 means all bits are 0; ~0 means all bits are 1 (writeing to sbuffer, all pass.),
    // do not to specify the bit number.
    glStencilMask(~0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Use an orthographic path-to-clip-space transform to map the [0..500]x[0..400]
    // range of the star's path coordinates to the [-1..1] clip space cube
    // OpenGL
    //glMatrixLoadIdentityEXT(GL_PROJECTION);
    //glMatrixOrthoEXT(GL_PROJECTION, 0, 500, 0, 400, -1, 1);
    //glMatrixLoadIdentityEXT(GL_MODELVIEW);
    // ES
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrthof(0, 500, 0, 400, -1, 1);



    // Stencil the path:
    glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_ALWAYS, 0, 0);
    // 这句话就是把内容写到Sbuffer里面. 参数什么意思?
    glStencilFillPathNV(pathObject, GL_COUNT_UP_NV, 0x1F); //fill是吧整个obj填满


    // (sbuffer & 0x1F) 和 (0 & 0x1F) 比较, 不相等通过, 因为sbuf里面的内容(heart和star)不是0, 背景是0
    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
    // keep: action to take if the stencil test fails.
    // keep: action to take if the stencil test passes, but the depth test fails.
    // zero: action to take if both the stencil and the depth test pass.
    // 最后一个zero就是说这次做完了就把sbuf清掉, 下次做其他的时候不用在clear了.
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

    // GL
    // glColor3f(1, 1, 0); // yellow
    // ES
    glUniform3f(color,1,1,0);

    glCoverFillPathNV(pathObject, GL_BOUNDING_BOX_NV); //画黄色的两个obj, fill是填满


    // 画两个obj外面的白色的线
    glPathParameteriNV(pathObject, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);
    glPathParameterfNV(pathObject, GL_PATH_STROKE_WIDTH_NV, 10.5); //线的宽度

    glStencilFunc(GL_ALWAYS, 0, 0); //add

    glStencilStrokePathNV(pathObject, 0x1, ~0); //和上面的fill对比, stroke 是把outline写进sbuf.

    glStencilFunc(GL_NOTEQUAL, 0, 0x1); //add

    // GL
    //glColor3f(1, 1, 1); // white
    glUniform3f(color, 1.0, 1.0, 1.0); //ES

    glCoverStrokePathNV(pathObject, GL_CONVEX_HULL_NV);//还是和上面fill对比.

    glUseProgram(0);
    glDeleteProgram(program);

#if 0
    // 画OpenGL字
    const char *word = "OpenGLXY";
    const GLsizei wordLen = (GLsizei)strlen(word);
    GLuint glyphBase = glGenPathsNV(wordLen);
    const GLfloat emScale = 2048; // match TrueType convention
    GLuint templatePathObject = ~0; // Non-existant path object
    // 在字库里面找字
    glPathGlyphsNV(glyphBase,
                   GL_SYSTEM_FONT_NAME_NV, "Helvetica", GL_BOLD_BIT_NV,
                   wordLen, GL_UNSIGNED_BYTE, word,
                   GL_SKIP_MISSING_GLYPH_NV, templatePathObject, emScale);
    glPathGlyphsNV(glyphBase,
                   GL_SYSTEM_FONT_NAME_NV, "Arial", GL_BOLD_BIT_NV,
                   wordLen, GL_UNSIGNED_BYTE, word,
                   GL_SKIP_MISSING_GLYPH_NV, templatePathObject, emScale);
    glPathGlyphsNV(glyphBase,
                   GL_STANDARD_FONT_NAME_NV, "Sans", GL_BOLD_BIT_NV,
                   wordLen, GL_UNSIGNED_BYTE, word,
                   GL_USE_MISSING_GLYPH_NV, templatePathObject, emScale);

    // GLfloat xtranslate[5 + 1]; // wordLen+1
    GLfloat *xtranslate = (GLfloat*) malloc((wordLen + 1) * sizeof(GLfloat));
    if (!xtranslate) {
        printf("ERROR: xtranslate.\n");
        return;
    }

    xtranslate[0] = 0; // Initial glyph offset is zero
    glGetPathSpacingNV(GL_ACCUM_ADJACENT_PAIRS_NV,
                       wordLen + 1, GL_UNSIGNED_BYTE,
                       //多了, 和字符数对应就行.
                       "\000\001\002\003\004\005\006\007\008\009\009", // repeat last
                       // letter twice
                       glyphBase,
                       1.0f, 1.0f,
                       GL_TRANSLATE_X_NV,
                       xtranslate + 1);

    // bug, VS下找不到这两个macro
#define GL_FONT_Y_MIN_BOUNDS_NV                             0x00020000
#define GL_FONT_Y_MAX_BOUNDS_NV                             0x00080000

    GLfloat yMinMax[2];
    glGetPathMetricRangeNV(GL_FONT_Y_MIN_BOUNDS_NV | GL_FONT_Y_MAX_BOUNDS_NV,
                           glyphBase, /*count*/1,
                           2 * sizeof(GLfloat),
                           yMinMax);

    glMatrixLoadIdentityEXT(GL_PROJECTION);
    glMatrixOrthoEXT(GL_PROJECTION,
                     0, xtranslate[wordLen], yMinMax[0], yMinMax[1],
            -1, 1);
    glMatrixLoadIdentityEXT(GL_MODELVIEW);

    glStencilFillPathInstancedNV(wordLen, GL_UNSIGNED_BYTE,
                                 "\000\001\002\003\004\005\006\007\008\009",
                                 glyphBase,
                                 GL_PATH_FILL_MODE_NV, 0xFF,
                                 GL_TRANSLATE_X_NV, xtranslate);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glColor3f(0.5, 0.5, 0.5); // 50% gray
    //// a linear gradient color, bug??
    //const GLfloat rgbGen[3][3] = {
    //    { 0, 0, 0 }, // red = constant zero
    //    { 0, 1, 0 }, // green = varies with y from bottom (0) to top (1)
    //    { 0, -1, 1 } // blue = varies with y from bottom (1) to top (0)
    //};
    //glPathColorGenNV(GL_PRIMARY_COLOR, GL_PATH_OBJECT_BOUNDING_BOX_NV,
    //    GL_RGB, &rgbGen[0][0]);

    glCoverFillPathInstancedNV(wordLen, GL_UNSIGNED_BYTE,
                               "\000\001\002\003\004\005\006\007\008\009",
                               glyphBase,
                               GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV,
                               GL_TRANSLATE_X_NV, xtranslate);



    if (xtranslate) {
        free(xtranslate);
        xtranslate = NULL;
    }

#endif
    return true;
}

#endif

bool PathRenderingBasic::exitGraphics() {
#if 0
    // clean up and restore the status back
    glDeletePathsNV(pathObject, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, ~0U);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glClearColor(0, 0, 0, 0);
#endif
    return true;
}
bool PathRenderingBasic::isSupported() {
    // make sure your query string is correct, like it is GL_NV_path_rendering, not NV_path_rendering
    int pr = com.isGLExtensionSupported("GL_NV_path_rendering");
    if (!pr) {
        printf("GL_NV_path_rendering is not suppported.\n");
        return false;
    }

    //    // In OpenGL ES, NV_path_rendering incorporates the relevant parts of EXT_direct_state_access.
    //    int dsa = com.isExtensionSupported("GL_EXT_direct_state_access");
    //    if (!dsa) {
    //        printf("GL_EXT_direct_state_access is not suppported.\n");
    //        return false;
    //    }

    // && glStencilBits > 0
    return true;
}

ADD_TEST(PathRenderingBasic)
