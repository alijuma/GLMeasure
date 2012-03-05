#ifndef CONFIGEGL_H_
#define CONFIGEGL_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// TODO: Turn this into a class that encapsualtes the display, surface, and context.

enum ImageFormat {ImageFormatRGB16_565, ImageFormatARGB32};

struct EGLAttrib {
    const ImageFormat mFormat;
    const EGLint* mAttrib;
};

bool CreateConfig(EGLConfig* aConfig, const EGLAttrib& aAttrib);

bool CreateConfigARGB32(EGLConfig* aConfig);

bool CreateConfigRGB16_565(EGLConfig* aConfig);

#endif /* CONFIGEGL_H_ */
