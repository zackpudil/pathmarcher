#include <main.hpp>
#include <progressbar.hpp>

void ProgressBar::incrementProgress(float timetook) {
  avgTimeTook += timetook;

  progress++;
  int eta = ((avgTimeTook/progress)*(amount - progress));

  std::cout << "[";
  float prog = (float)(progress)/(float)(amount - 1);

  int pos = 100*prog;

  for(int k = 0; k < 100; k++) {
    if(k < pos) std::cout << "=";
    else if(k == pos) std::cout << ">";
    else std::cout << " ";
  }

  int minutes = eta/60;
  int hours = minutes/60;
  int seconds = eta % 60;

  std::cout << "] " << int(prog*100.0f) << "% " 
    << progress << "/" << amount << " " 
    << (hours) << "h " << (minutes % 60) << "m " << seconds << "s" "\r";
  std::cout.flush();
}

void ProgressBar::clear() {
  progress = 0;
  avgTimeTook = 0;
}