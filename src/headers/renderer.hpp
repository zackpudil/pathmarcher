#pragma once

#include <pixel.hpp>
#include <progressbar.hpp>
#include <pipeline.hpp>
#include <GLFW/glfw3.h>

class Renderer {
private:
  float** reel;

  int pass;
  int frames;
  float timeStep;

public:
  int currentFrame;
  float currenTime;

  Renderer(int, int, float);

  void prerender(Pixel*, ProgressBar*, int, int);
  void init(int, int);

  void play(Pipeline*);
  void render(Pipeline*, Pixel*, GLFWwindow*);
};
