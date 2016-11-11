#include <pixel.hpp>
#include <main.hpp>

Pixel::Pixel(const char* path, int w, int h) {
  width = w;
  height = h;

  this->initPlatformDeviceContext();
  this->initSourcesProgram(path);
  this->initImageKernels();
}

void Pixel::computeImage(int device, float frame, float time, float *in, float *out, ProgressBar* progress) {
  cl::size_t<3> origin;
  origin.push_back(0); origin.push_back(0); origin.push_back(0);
  
  cl::size_t<3> region;
  region.push_back(width); region.push_back(height); region.push_back(1);

  kernels[device].queue.enqueueWriteBuffer(kernels[device].widthBuffer, CL_TRUE, 0, sizeof(int), &width);
  kernels[device].queue.enqueueWriteBuffer(kernels[device].heightBuffer, CL_TRUE, 0, sizeof(int), &height);
  kernels[device].queue.enqueueWriteBuffer(kernels[device].frameBuffer, CL_TRUE, 0, sizeof(float), &frame);
  kernels[device].queue.enqueueWriteBuffer(kernels[device].timeBuffer, CL_TRUE, 0, sizeof(float), &time);

  kernels[device].queue.enqueueWriteImage(kernels[device].inBuffer, CL_TRUE, origin, region, 0, 0, in);

  kernels[device].kernel(kernels[device].widthBuffer, 
                        kernels[device].heightBuffer, 
                        kernels[device].frameBuffer, 
                        kernels[device].timeBuffer, 
                        kernels[device].inBuffer, 
                        kernels[device].resultBuffer);

  kernels[device].queue.enqueueReadImage(kernels[device].resultBuffer, true, origin, region, 0, 0, out);

  if(progress != nullptr) {
    progress->incrementProgress(device);
  }
}

void Pixel::initPlatformDeviceContext() {
  std::vector<cl::Platform> allplatforms;
  cl::Platform::get(&allplatforms);
  platform = allplatforms[0];

  platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

  context = cl::Context(devices);
}

void Pixel::initSourcesProgram(const char* path) {
  sources = cl::Program::Sources();
  std::ifstream t(path);
  auto src = std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
  sources.push_back({src.c_str(), src.length()});

  program = cl::Program(context, sources);
  if(program.build(devices) != CL_SUCCESS) {
    std::cout << "Error building Program: " << std::endl;

    for(auto &device : devices) {
      std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
    }

    exit(1);
  }

}

void Pixel::initImageKernels() {
  cl::ImageFormat imageFormat(CL_RGBA, CL_FLOAT);

  for(auto &device : devices) {
    auto heightBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
    auto widthBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
    auto frameBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float));
    auto timeBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float));

    auto inBuffer = cl::Image2D(context, CL_MEM_READ_ONLY, imageFormat, width, height, 0, NULL, nullptr);
    auto resultBuffer = cl::Image2D(context, CL_MEM_READ_WRITE, imageFormat, width, height, 0, NULL, nullptr);
    auto queue = cl::CommandQueue(context, device);
    auto kernel = cl::KernelFunctor(cl::Kernel(program, "pixel"), queue, cl::NullRange,
      cl::NDRange(width, height), cl::NDRange(16, 16));

    kernels.push_back(ImageKernel {
      widthBuffer,
      heightBuffer,
      frameBuffer,
      timeBuffer,
      inBuffer,
      resultBuffer,
      queue,
      kernel
    });
  }
}






