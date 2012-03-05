#include "ConfigEGL.h"

// We borrow useful bits from mozilla::gl::GLContextProviderEGL


static const EGLint kEGLConfigAttribsRGB16[] = {
  EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  EGL_RED_SIZE,        5,
  EGL_GREEN_SIZE,      6,
  EGL_BLUE_SIZE,       5,
  EGL_ALPHA_SIZE,      0,
  EGL_NONE
};


static const EGLint kEGLConfigAttribsRGBA32[] = {
  EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  EGL_RED_SIZE,        8,
  EGL_GREEN_SIZE,      8,
  EGL_BLUE_SIZE,       8,
  EGL_ALPHA_SIZE,      8,
  EGL_NONE
};

static const EGLAttrib kEGLAttribRGB16 = {
  ImageFormatRGB16_565, kEGLConfigAttribsRGB16
};

static const EGLAttrib kEGLAttribRGBA32 = {
  ImageFormatARGB32, kEGLConfigAttribsRGBA32
};

bool CreateConfigARGB32(EGLConfig* aConfig)
{
  return CreateConfig(aConfig, kEGLAttribRGBA32);
}

bool CreateConfigRGB16_565(EGLConfig* aConfig)
{
  return CreateConfig(aConfig, kEGLAttribRGB16);
}


// Return true if a suitable EGLConfig was found and pass it out
// through aConfig.  Return false otherwise.
//
// NB: It's entirely legal for the returned EGLConfig to be valid yet
// have the value null.
bool CreateConfig(EGLConfig* aConfig, const EGLAttrib& aAttrib)
{
  const EGLint ncfg = 64;
  EGLConfig configs[ncfg];
  EGLint numConfigsFound;
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!eglChooseConfig(display, aAttrib.mAttrib,configs, ncfg, &numConfigsFound) ||
      numConfigsFound < 1) {
    return false;
  }
  for (int j = 0; j < numConfigsFound; ++j) {
    EGLConfig config = configs[j];
    EGLint r, g, b, a;

    if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &r) &&
        eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &g) &&
        eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &b) &&
        eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &a) &&
        ((ImageFormatRGB16_565 == aAttrib.mFormat && r == 5 && g == 6 && b == 5) ||
         (ImageFormatARGB32 == aAttrib.mFormat &&r == 8 && g == 8 && b == 8 && a == 8))) {
       *aConfig = config;
       return true;
    }
  }
  return false;
}
