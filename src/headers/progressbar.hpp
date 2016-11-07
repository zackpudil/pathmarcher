#pragma once

class ProgressBar {
private:
  int progress;
  float avgTimeTook;

  int amount;

public:
  ProgressBar(int a) : amount(a) {};

  void incrementProgress(float);
};