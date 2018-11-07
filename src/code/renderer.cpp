#include <main.hpp>
#include <renderer.hpp>
#include <progressbar.hpp>
#include <sys/stat.h>

Renderer::Renderer(int f, int p, float t, int w, int h) :
  frames(f), pass(p), timeStep(t), width(w), height(h) { 
    currentTime = 0.0f;
    currentFrame = 0;
    reel = (float **)malloc(sizeof(float*)*frames);
}

void Renderer::prerender(Pixel* pixel) {
  std::vector<std::thread> threads;

  for(int k = 0; k < pixel->deviceCount; k++) {
    threads.push_back(std::thread([&, k]() {
      
      int start = k*(frames/pixel->deviceCount);
      int end = (k + 1)*(frames/pixel->deviceCount);
      
      float time = timeStep*k*(frames/pixel->deviceCount);
      ProgressBar *progress = new ProgressBar((frames/pixel->deviceCount)*pass);
      
      for(int i = start; i < end; i++) {
        reel[i] = (float *)malloc(sizeof(float)*width*height*4);
        for(int j = 0; j < pass; j++) {
          pixel->computeImage(k, j, time, reel[i], reel[i], progress);
        }
        time += timeStep;
      }
    }));
  }

  for(auto& th : threads) th.join();

  std::cout << std::endl << std::endl;

}

void Renderer::save(char *file) {
  std::cout << std::endl;
  std::cout << "Saving to " << file << std::endl;

  ProgressBar progress(frames);

  std::ofstream img(file);
  img << frames << "|";

  for(int i = 0; i < frames; i++) {

    for(int j = 0; j < width*height*4; j+=4) 
    for(int k = 0; k < 4; k++) {
        img << reel[i][j + k] << "|";
    }

    progress.incrementProgress(0);
  }
  std::cout << std::endl;
  img.close();
}

void Renderer::init() {
  reel[0] = (float *)malloc(sizeof(float)*width*height*4);
}

void Renderer::loadPlayback(char *imgPath) {
  std::ifstream img(imgPath);
  ProgressBar progress(frames);

  int te;
  char temp;
  img >> te >> temp;
  for(int i = 0; i < frames; i++) {
    reel[i] = (float *)malloc(sizeof(float)*width*height*4);

    for(int j = 0; j < width*height*4; j+= 4) 
    for(int k = 0; k < 4; k++) {
        img >> reel[i][j+k] >> temp;
    }

    progress.incrementProgress(0);
  }

  std::cout << std::endl;
  img.close();
}

void Renderer::play(Pipeline *pipeline) {
  pipeline->imageData = reel[currentFrame];

  pipeline->draw(pass);
  if(glfwGetTime() - currentTime >= timeStep - (timeStep/2.0f)) {
    currentFrame++;
    if(currentFrame == frames) currentFrame = 0;
    currentTime = glfwGetTime();
  }
}

void Renderer::render(Pipeline *pipeline, Pixel *pixel, GLFWwindow *window) {
  pixel->computeImage(currentFrame % pixel->deviceCount, currentFrame, currentTime, reel[0], reel[0]);

  pipeline->imageData = reel[0];
  pipeline->draw(currentFrame);
  currentFrame++;

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    currentTime += timeStep;
    currentFrame = 0;
  }
}
