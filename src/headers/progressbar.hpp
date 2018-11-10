#pragma once
#include <mutex>

class ProgressBar {

private:
  static std::mutex progressMtx;
  int progress;

public:
  int amount;
  float totalTime;
  std::chrono::time_point<std::chrono::system_clock> lastTime;

  ProgressBar(int a);

  void clear();
  void incrementProgress(int);
};
