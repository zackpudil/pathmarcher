#include <main.hpp>
#include <pixel.hpp>
#include <pipeline.hpp>
#include <progressbar.hpp>
#include <renderer.hpp>

int width = 1280;
int height = 720;

int pass = 150;
int frames = 300;
float timeStep = 0.03f;

int main(int argc, char** argv) {

  char *imgPath;

  bool prerender = false;
  bool save = false;

  bool append = false;
  bool load = false;

  int continueOnFrame = 0;

  if(argc > 1 && std::string("prerender").compare(argv[1]) == 0) {
    prerender = true;
    if(argc > 2 && std::string("save").compare(argv[2]) == 0) save = true;
    imgPath = argv[3];
  } else if(argc > 1 && std::string("playback").compare(argv[1]) == 0) {
    load = true;
    imgPath = argv[2];
    std::ifstream forFrame(imgPath);
    forFrame >> frames;
    forFrame.close();
  }

  bool prerendered = prerender || load;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "My Title", glfwGetPrimaryMonitor(), NULL);
  glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

  if(window == nullptr) {
    std::cout << "Failed to create OpenGL context.\n";
    exit(1);
  }
  
  glfwMakeContextCurrent(window);
  gladLoadGL();
  std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
  
  Pixel pixel(PROJECT_SOURCE_DIR "/kernels/kali.cl", width, height);
  Pipeline pipeline(width, height);
  Renderer renderer(frames, pass, timeStep, width, height);

  if(prerender) {
    renderer.prerender(&pixel);
    if(save)
      renderer.save(imgPath);
  } else if(load) {
    renderer.loadPlayback(imgPath);
  } else {
    renderer.init();
  }

  renderer.currentFrame = 0;

  // loop and render cl created texture.
  while(glfwWindowShouldClose(window) == false) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if(prerendered) {
      renderer.play(&pipeline);
    } else {
      renderer.render(&pipeline, &pixel, window);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  if(!prerendered) {
    std::cout << renderer.currentFrame << std::endl;
  }

  glfwTerminate();

  return 0;
}
