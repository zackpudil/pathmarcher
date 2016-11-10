#pragma once

class ProgressBar {
private:
  int progress;
  float avgTimeTook;
public:
  int amount;

  ProgressBar(int a);

  void clear();
  void incrementProgress(float);
};
