#include <renderer.hpp>
#include <main.hpp>
#include <progressbar.hpp>
#include <sys/stat.h>

Renderer::Renderer(int f, int p, float t, int w, int h) :
  frames(f), pass(p), timeStep(t), width(w), height(h) { 
    currentTime = 0.0f;
    currentFrame = 0;
    reel = (float **)malloc(sizeof(float*)*frames);
}

void Renderer::prerender(Pixel* pixel) {
  float time1 = 0.0f;
  float time2 = timeStep*(frames/2);
  std::vector<ProgressBar*> progresses = {new ProgressBar((frames/2)*pass), new ProgressBar((frames/2)*pass) };
  std::vector<std::thread> threads;

  threads.push_back(std::thread([&]() {
    for(int i = 0; i < frames/2; i++) {
      reel[i] = (float *)malloc(sizeof(float)*width*height*4);
      for(int j = 0; j < pass; j++) {
        pixel->computeImage(0, j, time1, reel[i], reel[i], progresses[0]);
      }
      time1 += timeStep;
    }
  }));

  threads.push_back(std::thread([&]() {
    for(int i = frames/2; i < frames; i++) {
      reel[i] = (float *)malloc(sizeof(float)*width*height*4);
      for(int j = 0; j < pass; j++) {
        pixel->computeImage(1, j, time2, reel[i], reel[i], progresses[1]);
      }
      time2 += timeStep;
    }
  }));

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
  pixel->computeImage(currentFrame % 2, currentFrame, currentTime, reel[0], reel[0]);

  pipeline->imageData = reel[0];
  pipeline->draw(currentFrame);
  currentFrame++;

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    currentTime += timeStep;
    currentFrame = 0;
  }
}
