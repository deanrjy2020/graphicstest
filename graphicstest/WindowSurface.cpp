#include "WindowSurface.h"

#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <ui/DisplayInfo.h>

using namespace android;

WindowSurface::WindowSurface(int w, int h) {

}

// do it manually here
#define ANDROID_P

#if defined(ANDROID_P)
WindowSurface::WindowSurface() {
    status_t err;

    sp<SurfaceComposerClient> surfaceComposerClient = new SurfaceComposerClient;
    err = surfaceComposerClient->initCheck();
    if (err != NO_ERROR) {
        fprintf(stderr, "SurfaceComposerClient::initCheck error: %#x\n", err);
        return;
    }

    // Get main display parameters.
    sp<IBinder> mainDpy = SurfaceComposerClient::getBuiltInDisplay(
            ISurfaceComposer::eDisplayIdMain);
    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(mainDpy, &mainDpyInfo);
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to get display characteristics\n");
        return;
    }

    // do not use the default one, use the one passed by user,
    // the surf size should always be 640*480, fix this later.
   uint32_t width, height;
#if 1
   width = 640;
   height = 480;
#else
    if (mainDpyInfo.orientation != DISPLAY_ORIENTATION_0 &&
            mainDpyInfo.orientation != DISPLAY_ORIENTATION_180) {
        // rotated
        width = mainDpyInfo.h;
        height = mainDpyInfo.w;
    } else {
        width = mainDpyInfo.w;
        height = mainDpyInfo.h;
    }
#endif
    sp<SurfaceControl> sc = surfaceComposerClient->createSurface(
            String8("Benchmark"), width, height,
            PIXEL_FORMAT_RGBX_8888, ISurfaceComposerClient::eOpaque);
    if (sc == NULL || !sc->isValid()) {
        fprintf(stderr, "Failed to create SurfaceControl\n");
        return;
    }

    SurfaceComposerClient::Transaction{}
            .setLayer(sc, 0x7FFFFFFF)
            .show(sc)
            .apply();

    mSurfaceControl = sc;
}

#elif defined(ANDROID_O)

WindowSurface::WindowSurface() {
    status_t err;

    sp<SurfaceComposerClient> surfaceComposerClient = new SurfaceComposerClient;
    err = surfaceComposerClient->initCheck();
    if (err != NO_ERROR) {
        fprintf(stderr, "SurfaceComposerClient::initCheck error: %#x\n", err);
        return;
    }

    // Get main display parameters.
    sp<IBinder> mainDpy = SurfaceComposerClient::getBuiltInDisplay(
            ISurfaceComposer::eDisplayIdMain);
    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(mainDpy, &mainDpyInfo);
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to get display characteristics\n");
        return;
    }

    // do not use the default one, use the one passed by user,
    // the surf size should always be 640*480, fix this later.
   uint32_t width, height;
#if 1
   width = 640;
   height = 480;
#else
    uint32_t width, height;
    if (mainDpyInfo.orientation != DISPLAY_ORIENTATION_0 &&
            mainDpyInfo.orientation != DISPLAY_ORIENTATION_180) {
        // rotated
        width = mainDpyInfo.h;
        height = mainDpyInfo.w;
    } else {
        width = mainDpyInfo.w;
        height = mainDpyInfo.h;
    }
#endif

    sp<SurfaceControl> sc = surfaceComposerClient->createSurface(
            String8("Benchmark"), width, height,
            PIXEL_FORMAT_RGBX_8888, ISurfaceComposerClient::eOpaque);
    if (sc == NULL || !sc->isValid()) {
        fprintf(stderr, "Failed to create SurfaceControl\n");
        return;
    }

    SurfaceComposerClient::openGlobalTransaction();
    err = sc->setLayer(0x7FFFFFFF);     // always on top
    if (err != NO_ERROR) {
        fprintf(stderr, "SurfaceComposer::setLayer error: %#x\n", err);
        return;
    }

    err = sc->show();
    if (err != NO_ERROR) {
        fprintf(stderr, "SurfaceComposer::show error: %#x\n", err);
        return;
    }
    SurfaceComposerClient::closeGlobalTransaction();

    mSurfaceControl = sc;
}
#endif

EGLNativeWindowType WindowSurface::getSurface() const {
    sp<ANativeWindow> anw = mSurfaceControl->getSurface();
    return (EGLNativeWindowType) anw.get();
}

