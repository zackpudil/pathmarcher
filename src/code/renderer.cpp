#include <renderer.hpp>
#include <main.hpp>

Renderer::Renderer(int f, int p, float t, int w, int h) :
  frames(f), pass(p), timeStep(t), width(w), height(h) { 
    currentTime = 0.0f;
    currentFrame = 0;
    reel = (float **)malloc(sizeof(float*)*frames);
}

void Renderer::prerender(Pixel* pixel, ProgressBar* progress, bool save, char *file) {
  float time = currentTime;

  for(int i = 0; i < frames; i++) {

    reel[i] = (float *)malloc(sizeof(float)*width*height*4);

    for(int j = 0; j < pass; j++) {

      float timetook = glfwGetTime();
      pixel->computeImage(j, time, reel[i], reel[i]);
      timetook = glfwGetTime() - timetook;

      progress->incrementProgress(timetook);
    }

    time += timeStep;
  }

  if(save) {
    std::cout << std::endl;
    std::cout << "Saving to " << file << std::endl;

    progress->clear();
    progress->amount = frames;

    std::ofstream img(file, std::ios::out | std::ios::app);

    for(int i = 0; i < frames; i++) {
      float timetook = glfwGetTime();
      for(int j = 0; j < width*height*4; j+=4) 
      for(int k = 0; k < 4; k++) {
          img << reel[i][j + k] << "|";
      }
      timetook = glfwGetTime() - timetook;
      progress->incrementProgress(timetook);
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
  pixel->computeImage(currentFrame, currentTime, reel[0], reel[0]);

  pipeline->imageData = reel[0];
  pipeline->draw(currentFrame);
  currentFrame++;

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    currentTime += timeStep;
    currentFrame = 0;
  }
}
