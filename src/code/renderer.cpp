#include <renderer.hpp>
#include <main.hpp>

Renderer::Renderer(int f, int p, float t) :
  frames(f), pass(p), timeStep(t) { 
    reel = (float **)malloc(sizeof(float*)*f);
}

void Renderer::prerender(Pixel* pixel, ProgressBar* progress, int w, int h) {
  float time = 0.0f;

  for(int i = 0; i < frames; i++) {

    reel[i] = (float *)malloc(sizeof(float)*w*h*4);

    for(int j = 0; j < pass; j++) {

      float timetook = glfwGetTime();
      pixel->computeImage(j, time, reel[i], reel[i]);
      timetook = glfwGetTime() - timetook;

      progress->incrementProgress(timetook);
    }

    time += timeStep;
  }
}

void Renderer::init(int width, int height) {
  reel[0] = (float *)malloc(sizeof(float)*width*height*4);
}

void Renderer::play(Pipeline *pipeline) {
  pipeline->imageData = reel[currentFrame];
  pipeline->draw(pass);
  if(glfwGetTime() - currenTime >= timeStep - (timeStep/2.0f)) {
    currentFrame++;
    if(currentFrame == frames) currentFrame = 0;
    currenTime = glfwGetTime();
  }
}

void Renderer::render(Pipeline *pipeline, Pixel *pixel, GLFWwindow *window) {
  reel[0] =
  pixel->computeImage(currentFrame, currenTime, reel[0], reel[0]);
  pipeline->imageData = reel[0];
  pipeline->draw(currentFrame);
  currentFrame++;

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    currenTime += timeStep;
    currentFrame = 0;
  }
}
