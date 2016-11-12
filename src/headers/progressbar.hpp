#pragma once
#include <mutex>

class ProgressBar {

private:
  static std::mutex progressMtx;
  int progress;

public:
  int amount;
  float totalTime;
  float lastTime;

  ProgressBar(int a);

  void clear();
  void incrementProgress(int);
};
