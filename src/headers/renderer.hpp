#pragma once

#include <pixel.hpp>
#include <progressbar.hpp>
#include <pipeline.hpp>
#include <GLFW/glfw3.h>

class Renderer {
private:
  float** reel;

  int width;
  int height;

  int pass;
  int frames;
  float timeStep;
public:
  int currentFrame;
  float currentTime;

  Renderer(int, int, float, int, int);

  void prerender(Pixel*, ProgressBar*, bool, char*);
  void init();
  void loadPlayback(char* img, ProgressBar*);

  void play(Pipeline*);
  void render(Pipeline*, Pixel*, GLFWwindow*);
};
