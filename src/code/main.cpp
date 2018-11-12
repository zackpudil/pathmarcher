#include <main.hpp>
#include <pixel.hpp>
#include <progressbar.hpp>
#include <renderer.hpp>

int width = 1280;
int height = 720;

int pass = 1;
int frames = 390;
float timeStep = 0.03f;

int main(int argc, char** argv) {

  char *imgPath;

  bool prerender = false;
  bool save = false;

  bool append = false;
  bool load = false;

  int continueOnFrame = 0;

  if(argc == 1) {
    std::cout << "Please provide path to scene.  E.g kernels/kali.cl";
    exit(1);
  }

  char *scene = argv[1];

  bool prerendered = prerender || load;

  Pixel pixel(scene, width, height);
  Renderer renderer(frames, pass, timeStep, width, height);

  renderer.prerender(&pixel);
  renderer.save();

  return 0;
}
