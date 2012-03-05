#include "TestGL.h"
#include <stdlib.h>
#include <stdio.h>


void RunGLMeasure()
{
  FILE* f = fopen("/data/data/org.mozilla.gl_measure/files/output.txt", "w+");
  int numRuns = 20;
  const int numPixelSizes = 2;
  TestGL::PixelSize pixelSizes[numPixelSizes] =
    {TestGL::PixelSize16Bits, TestGL::PixelSize32Bits};

  const int numAlignments = 3;
  int alignments[numAlignments] = {2, 4, 8};
  // Get texture size that is the largest possible mulitple of 8.
  GLsizei maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  if (maxTextureSize > 2048)
    maxTextureSize = 2048;
  maxTextureSize = (maxTextureSize/8)*8;
  for (int i = 0; i < numPixelSizes; i++) {
    for (int textureSizeOffset = 0; textureSizeOffset <=8; textureSizeOffset++) {
      int textureSize = maxTextureSize - textureSizeOffset;
      for (int j = 0; j < numAlignments; j++) {
        TestTexImage2D testTexImage(pixelSizes[i], textureSize, alignments[j]);
        testTexImage.TestAndOutput(numRuns, f);
        TestTexSubImage2DMemcpy testTexSubImageMemcpy(pixelSizes[i], textureSize, alignments[j], textureSize, 0);
        testTexSubImageMemcpy.TestAndOutput(numRuns, f);
        TestTexSubImage2DRowByRow testTexSubImageRowByRow(pixelSizes[i], textureSize, alignments[j], textureSize, 0);
        testTexSubImageRowByRow.TestAndOutput(numRuns, f);
        // TODO: Check if UnpackRowLength is supported by the device before running the test.
        TestUnpackRowLength testUnpackRowLength(pixelSizes[i], textureSize, alignments[j], textureSize, 0);
        testUnpackRowLength.TestAndOutput(numRuns, f);
        TestTexSubImage2DTopHalf testTexSubImageTopHalf(pixelSizes[i], textureSize, alignments[j], textureSize, 0);
        testTexSubImageTopHalf.TestAndOutput(numRuns, f);
        for (int widthOffset = 0; widthOffset <= 8; widthOffset++) {
          int width = textureSize - widthOffset;
          for (int offset = 0; offset <= widthOffset; offset++) {
            //TODO: if it turns out that alignment has no effect on tex*sub*image2D,
            // then we don't need to try all the different alignments for this test.
            TestTexSubImage2D testTexSubImage(pixelSizes[i], textureSize, alignments[j],
                                              width, offset);
            testTexSubImage.TestAndOutput(numRuns, f);
            fflush(f);
          }
        }
      }
    }
  }
  printf("\n");
  fclose(f);
}
