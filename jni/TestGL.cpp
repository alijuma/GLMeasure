#include <TestGL.h>
#include <stdlib.h>
#include <stdio.h>

#define GL_UNPACK_ROW_LENGTH 0x0CF2
unsigned int PixelSizeToBytes(TestGL::PixelSize aSize)
{
  switch (aSize) {
    case TestGL::PixelSize16Bits:
      return 2;
    case TestGL::PixelSize24Bits:
      return 3;
    case TestGL::PixelSize32Bits:
      return 4;
  }
}

TestGL::TestGL(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment) :
  mTextureSize(aTextureSize), mAlignment(aAlignment)
{
  switch (aSize) {
    case PixelSize16Bits:
      mFormat = GL_RGB;
      mType = GL_UNSIGNED_SHORT_5_6_5;
      mPixelSize = 2;
      break;
    case PixelSize24Bits:
      mFormat = GL_RGB;
      mType = GL_UNSIGNED_BYTE;
      mPixelSize = 3;
      break;
    case PixelSize32Bits:
      mFormat = GL_RGBA;
      mType = GL_UNSIGNED_BYTE;
      mPixelSize = 4;
  }
  mTextureID = new GLuint[mNumTextures];
  glGenTextures(mNumTextures, mTextureID);
}

TestGL::~TestGL()
{
  if (mTextureID) {
    glDeleteTextures(mNumTextures, mTextureID);
    delete [] mTextureID;
  }
  if (mBuffer) {
    delete [] mBuffer;
  }
}

void TestGL::TestAndOutput(int aNumIterations, FILE* aFile)
{
  TestGLRunner tgr(this);
  Duration d = tgr.Run(aNumIterations);
  OutputTestName(aFile);
  fprintf(aFile, ",%d,%d,%d,%.0f\n", mPixelSize, mTextureSize, mAlignment,
          d.ToMilliseconds());
}

void TestGL::RunTest()
{
  PreTest();
  Test();
  PostTest();
}

Duration TestGLRunner::Run(int aNumIterations)
{
  glFinish();
  Time startTime = Time::Now();
  for (int i = 0; i < aNumIterations; i++) {
    mTest->RunTest();
  }
  Time endTime = Time::Now();
  return endTime - startTime;
}

TestTexImage2D::TestTexImage2D(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment) :
  TestGL(aSize, aTextureSize, aAlignment)
{
  // Adding 7 to accomodate possible unpack alignments.
  mBuffer = new unsigned char[mTextureSize*(mTextureSize*mPixelSize+7)];
  for (unsigned int i = 0; i < mNumTextures; i++) {
    glBindTexture(GL_TEXTURE_2D, mTextureID[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mTextureSize, mTextureSize, 0,
                 mFormat, mType, mBuffer);
  }
}

void TestTexImage2D::Test()
{
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mTextureSize, mTextureSize, 0,
               mFormat, mType, mBuffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TestTexSubImage2D::TestTexSubImage2D(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                                     GLsizei aUploadWidth, GLint aOffset) :
                                     TestTexImage2D(aSize, aTextureSize, aAlignment),
                                     mUploadWidth(aUploadWidth),
                                     mOffset(aOffset) {}

void TestTexSubImage2D::TestAndOutput(int aNumIterations, FILE* aFile)
{
  TestGLRunner tgr(this);
  Duration d = tgr.Run(aNumIterations);
  OutputTestName(aFile);
  fprintf(aFile, ",%d,%d,%d,%d,%d,%.0f\n", mPixelSize, mTextureSize, mAlignment,
          mUploadWidth, mOffset, d.ToMilliseconds());
}

void TestTexSubImage2D::Test()
{
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  glTexSubImage2D(GL_TEXTURE_2D, 0, mOffset, 0, mUploadWidth, mTextureSize,
                  mFormat, mType, mBuffer);
  //if (glGetError() != GL_NO_ERROR) {
    // TODO: Add some error handling, so we don't confuse errors and speed.
  // }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TestTexSubImage2DMemcpy::TestTexSubImage2DMemcpy(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                                                 GLsizei aUploadWidth, GLint aOffset) :
                                                 TestTexSubImage2D(aSize, aTextureSize, aAlignment,
                                                                   aUploadWidth, aOffset) {}

void TestTexSubImage2DMemcpy::Test()
{
  // Adding 7 to accomodate possible unpack alignments.
  unsigned char* tempBuffer = new unsigned char[mTextureSize*(mTextureSize*mPixelSize+7)];
  // We're not taking alignment into account when copying, but our main goal is just
  // to get the right time for copying, not necessarily to copy the right content.
  for (int i = 0; i < mTextureSize; i++) {
    memcpy(tempBuffer+mPixelSize*i*mUploadWidth,
           mBuffer+mPixelSize*i*mUploadWidth, mPixelSize*mUploadWidth);
  }
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  glTexSubImage2D(GL_TEXTURE_2D, 0, mOffset, 0, mUploadWidth, mTextureSize,
                  mFormat, mType, tempBuffer);
  delete [] tempBuffer;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TestTexSubImage2DRowByRow::TestTexSubImage2DRowByRow(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                                                     GLsizei aUploadWidth, GLint aOffset) :
                                                     TestTexSubImage2D(aSize, aTextureSize, aAlignment,
                                                                       aUploadWidth, aOffset) {}

void TestTexSubImage2DRowByRow::Test()
{
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  for (int i = 0; i < mTextureSize; i++) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, mOffset, i, mUploadWidth, 1, mFormat,
                    mType, mBuffer+mPixelSize*i*mTextureSize);
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TestTexSubImage2DTopHalf::TestTexSubImage2DTopHalf(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                                                   GLsizei aUploadWidth, GLint aOffset) :
                                                   TestTexSubImage2D(aSize, aTextureSize, aAlignment,
                                                                     aUploadWidth, aOffset) {}

void TestTexSubImage2DTopHalf::Test()
{
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  glTexSubImage2D(GL_TEXTURE_2D, 0, mOffset, 0, mUploadWidth, mTextureSize/2,
                  mFormat, mType, mBuffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TestUnpackRowLength::TestUnpackRowLength(PixelSize aSize, GLsizei aTextureSize, GLint aAlignment,
                                         GLsizei aUploadWidth, GLint aOffset) :
                                         TestGL(aSize, aTextureSize, aAlignment),
                                         mUploadWidth(aUploadWidth),
                                         mOffset(aOffset)
{
  mBuffer = new unsigned char[2*mTextureSize*(mTextureSize*mPixelSize+7)];
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mTextureSize, mTextureSize, 0,
               mFormat, mType, mBuffer);
}

void TestUnpackRowLength::Test()
{
  glBindTexture(GL_TEXTURE_2D, mTextureID[mTimesTested % mNumTextures]);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 2*mTextureSize);
  glPixelStorei(GL_UNPACK_ALIGNMENT, mAlignment);
  glTexSubImage2D(GL_TEXTURE_2D, 0, mOffset, 0, mUploadWidth, mTextureSize,
                  mFormat, mType, mBuffer);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void TestUnpackRowLength::TestAndOutput(int aNumIterations, FILE* aFile)
{
  TestGLRunner tgr(this);
  Duration d = tgr.Run(aNumIterations);
  OutputTestName(aFile);
  fprintf(aFile, ",%d,%d,%d,%d,%d,%.0f\n", mPixelSize, mTextureSize, mAlignment,
          mUploadWidth, mOffset, d.ToMilliseconds());
}

