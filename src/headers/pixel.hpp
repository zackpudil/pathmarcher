#pragma once

#include <cl.hpp>
#include <progressbar.hpp>

struct ImageKernel {
  cl::Buffer widthBuffer;
  cl::Buffer heightBuffer;
  cl::Buffer frameBuffer;
  cl::Buffer timeBuffer;

  cl::Image2D inBuffer;
  cl::Image2D resultBuffer;

  cl::CommandQueue queue;
  cl::KernelFunctor kernel;
};

class Pixel {
private:
  cl::Platform platform;
  std::vector<cl::Device> devices;
  cl::Context context;

  cl::Program::Sources sources;
  cl::Program program;

  std::vector<ImageKernel> kernels;

  std::string linkSource(const char *, const char *);
  void initPlatformDeviceContext();
  void initSourcesProgram(std::string src);
  void initImageKernels();
public:
  Pixel(const char*, int, int);

  void computeImage(int, float, float, float*, float*, ProgressBar* = nullptr);

  int width;
  int height;
};
