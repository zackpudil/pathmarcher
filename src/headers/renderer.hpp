#pragma once

#include <pixel.hpp>
#include <pipeline.hpp>
#include <GLFW/glfw3.h>

class Renderer {
private:
  float** reel;
  float* imageData;

  int width;
  int height;

  int pass;
  int frames;
  float timeStep;
public:
  int currentFrame;
  float currentTime;

  Renderer(int, int, float, int, int);

  void prerender(Pixel*);
  void save(char *);
  void init();
  void loadPlayback(char* img);

  void play(Pipeline*);
  void render(Pipeline*, Pixel*, GLFWwindow*);
};
