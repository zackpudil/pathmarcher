#pragma once
#include <mutex>

class ProgressBar {

private:
  static std::mutex progressMtx;
  int progress;

public:
  int amount;

  ProgressBar(int a);

  void clear();
  void incrementProgress(int);
};
