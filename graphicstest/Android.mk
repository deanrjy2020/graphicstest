LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    tests/EGL_ColorSpace.cpp \
    tests/EGL_GL_Info.cpp \
    tests/GL101.cpp \
    tests/GLES1_SharingFbo.cpp \
    tests/GeometryShaderBasic.cpp \
    tests/PathRenderingBasic.cpp \
    tests/TessShaderBasic.cpp \
    main.cpp \
    common.cpp \
    global.cpp \
    WindowSurface.cpp \
    md5.cpp \
    md5_test.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libEGL \
    libGLESv1_CM \
    libGLESv2 \
    libui \
    libgui \
    libutils

LOCAL_STATIC_LIBRARIES += libglTest

LOCAL_C_INCLUDES += $(call include-path-for, opengl-tests-includes)

LOCAL_MODULE:= graphicstest

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := \
    -DGL_GLEXT_PROTOTYPES \
    -Wall \
    -Wno-error \
    -g \
    -O0

include $(BUILD_EXECUTABLE)
