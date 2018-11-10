#pragma once

#include <pixel.hpp>

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

  void prerender(Pixel*);
  void save();
};
