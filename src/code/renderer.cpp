#include <renderer.hpp>
#include <main.hpp>

Renderer::Renderer(int f, int p, float t, int w, int h) :
  frames(f), pass(p), timeStep(t), width(w), height(h) { 
    currentTime = 0.0f;
    currentFrame = 0;
    reel = (float **)malloc(sizeof(float*)*frames);
}

void Renderer::prerender(Pixel* pixel, bool save, char *file) {
  float time = currentTime;
  ProgressBar progress(frames/2);

  for(int i = 0; i < frames; i+=2) {
    float tt = glfwGetTime();
    reel[i] = (float *)malloc(sizeof(float)*width*height*4);
    reel[i + 1] = (float *)malloc(sizeof(float)*width*height*4);

    std::thread iris([=]() {
      for(int j = 0; j < pass; j++) {
        pixel->computeImage(0, j, time, reel[i], reel[i]);
      }
    });

    time += timeStep;

    std::thread amd([=]() {
      for(int j = 0; j < pass; j++) {
        pixel->computeImage(1, j, time, reel[i + 1], reel[i + 1]);
      }
    });
    time += timeStep;

    iris.join();
    amd.join();

    progress.incrementProgress(glfwGetTime() - tt);
  }

  std::cout << std::endl << std::endl;

  if(save) {
    std::cout << std::endl;
    std::cout << "Saving to " << file << std::endl;

    ProgressBar progress(frames);

    std::ofstream img(file, std::ios::out | std::ios::app);

    for(int i = 0; i < frames; i++) {
      float timetook = glfwGetTime();
      for(int j = 0; j < width*height*4; j+=4) 
      for(int k = 0; k < 4; k++) {
          img << reel[i][j + k] << "|";
      }
      timetook = glfwGetTime() - timetook;
      progress.incrementProgress(timetook);
    }
    std::cout << std::endl;
    img.close();
  }
}

void Renderer::init() {
  reel[0] = (float *)malloc(sizeof(float)*width*height*4);
}

void Renderer::loadPlayback(char *imgPath, ProgressBar *progress) {
  std::ifstream img(imgPath);
  char temp;
  for(int i = 0; i < frames; i++) {
    reel[i] = (float *)malloc(sizeof(float)*width*height*4);
    float timetook = glfwGetTime();

    for(int j = 0; j < width*height*4; j+= 4) 
    for(int k = 0; k < 4; k++) {
        img >> reel[i][j+k] >> temp;
    }
    
    timetook = glfwGetTime() - timetook;
    progress->incrementProgress(timetook);
  }
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
  pixel->computeImage(1, currentFrame, currentTime, reel[0], reel[0]);

  pipeline->imageData = reel[0];
  pipeline->draw(currentFrame);
  currentFrame++;

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    currentTime += timeStep;
    currentFrame = 0;
  }
}
