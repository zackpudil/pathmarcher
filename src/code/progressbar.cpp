#include <main.hpp>
#include <progressbar.hpp>

std::mutex ProgressBar::progressMtx;

ProgressBar::ProgressBar(int a) : amount(a) {
  progress = 0;
  totalTime = 0.0f;
  lastTime = glfwGetTime();
}

void ProgressBar::incrementProgress(int line) {
  ProgressBar::progressMtx.lock();
  for(int i = 0; i < line; i++) 
    std::cout << std::endl;

  progress++;

  totalTime += glfwGetTime() - lastTime;
  lastTime = glfwGetTime();

  int eta = (totalTime/progress)*(amount - progress);

  std::cout << "[";
  float prog = (float)(progress)/(float)(amount - 1);

  int pos = 100*prog;

  for(int k = 0; k < 100; k++) {
    if(k < pos) std::cout << "=";
    else if(k == pos) std::cout << ">";
    else std::cout << " ";
  }

  std::cout << "] " << int(prog*100.0f) << "% " 
    << progress << "/" << amount << " "
    << (eta/60/60 % 60) << "h " << (eta/60 % 60) << "m " << (eta % 60) << "s     "
    << "\r";

  for(int i = 0; i < line; i++) 
    std::cout << "\e[A";
  std::cout.flush();

  ProgressBar::progressMtx.unlock();
}

void ProgressBar::clear() {
  progress = 0;
  totalTime = 0.0f;
}
