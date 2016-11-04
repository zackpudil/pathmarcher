#include <pixel.hpp>
#include <main.hpp>

Pixel::Pixel(const char* path, int w, int h) {
  width = w;
  height = h;
  imageData = (float *)malloc(width*height*4*sizeof(float));

  this->initPlatformDeviceContext();
  this->initSourcesProgramQueue(path);
  this->initKernel();
}

float *Pixel::computeImage(float frame) {
  cl::size_t<3> origin;
  origin.push_back(0);origin.push_back(0);origin.push_back(0);
  
  cl::size_t<3> region;
  region.push_back(width); region.push_back(height); region.push_back(1);

  queue.enqueueWriteBuffer(widthBuffer, CL_TRUE, 0, sizeof(int), &width);
  queue.enqueueWriteBuffer(heightBuffer, CL_TRUE, 0, sizeof(int), &height);
  queue.enqueueWriteBuffer(frameBuffer, CL_TRUE, 0, sizeof(float), &frame);
  queue.enqueueWriteImage(inBuffer, CL_TRUE, origin, region, 0, 0, imageData);

  kernel(widthBuffer, heightBuffer, frameBuffer, inBuffer, resultBuffer);

  queue.enqueueReadImage(resultBuffer, true, origin, region, 0, 0, imageData);

  return imageData;
}

void Pixel::initPlatformDeviceContext() {
  std::vector<cl::Platform> allplatforms;
  cl::Platform::get(&allplatforms);
  platform = allplatforms[0];
  
  std::vector<cl::Device> alldevices;
  platform.getDevices(CL_DEVICE_TYPE_GPU, &alldevices);
  device = alldevices[0];

  auto s = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

  context = cl::Context({device});
}

void Pixel::initSourcesProgramQueue(const char* path) {
  sources = cl::Program::Sources();
  std::ifstream t(path);
  auto src = std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
  sources.push_back({src.c_str(), src.length()});

  program = cl::Program(context, sources);
  if(program.build({device}) != CL_SUCCESS) {
    std::cout << "Error building Program: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
    exit(1);
  }

  queue = cl::CommandQueue(context, device);
}

void Pixel::initKernel() {
  heightBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
  widthBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
  frameBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float));

  int imageResult;
  cl::ImageFormat imageFormat(CL_RGBA, CL_FLOAT);
  inBuffer = cl::Image2D(context, CL_MEM_READ_ONLY, imageFormat, width, height, 0, NULL, &imageResult);

  if(imageResult != CL_SUCCESS) {
    std::cout << "Failure to create image buffer: " << imageResult;
    exit(1);
  }

  resultBuffer = cl::Image2D(context, CL_MEM_READ_WRITE, imageFormat, width, height, 0, NULL, &imageResult);
  if(imageResult != CL_SUCCESS) {
    std::cout << "Failure to create image buffer: " << imageResult;
    exit(1);
  }

  kernel = cl::KernelFunctor(cl::Kernel(program, "pixel"), queue, cl::NullRange, 
    cl::NDRange(width, height), cl::NDRange(20, 20));
}
