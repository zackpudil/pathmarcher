#pragma once

#include <cl.hpp>

class Pixel {
private:
  cl::Platform platform;
  cl::Device device;
  cl::Context context;

  cl::Program::Sources sources;
  cl::Program program;
  cl::CommandQueue queue;

  cl::Buffer heightBuffer;
  cl::Buffer widthBuffer;
  cl::Buffer frameBuffer;
  cl::Buffer timeBuffer;

  cl::Image2D inBuffer;
  cl::Image2D resultBuffer;

  cl::KernelFunctor kernel;

  void initPlatformDeviceContext();
  void initSourcesProgramQueue(const char* path);
  void initKernel();
public:
  Pixel(const char*, int, int);

  float* computeImage(float, float, float*, float*);

  int width;
  int height;
};