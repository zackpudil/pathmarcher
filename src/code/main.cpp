#include <main.hpp>
#include <pixel.hpp>
#include <pipeline.hpp>
#include <progressbar.hpp>
#include <renderer.hpp>

int width = 1280;
int height = 720;

int pass = 10;
int frames = 50;
float timeStep = 0.03f;

int main(int argc, char** argv) {

  bool offline = false;
  if(argc > 1 && std::string("offline").compare(argv[1]) == 0) offline = true;

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

  Pixel pixel(PROJECT_SOURCE_DIR "/kernels/scene.cl", width, height);
  Pipeline pipeline(width, height);
  Renderer renderer(frames, pass, timeStep);

  if(offline) {
    ProgressBar progress(frames*pass);
    renderer.prerender(&pixel, &progress, width, height);
  } else {
    renderer.init(width, height);
  }

  // loop and render cl created texture.
  while(glfwWindowShouldClose(window) == false) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if(offline) {
      renderer.play(&pipeline);
    } else {
      renderer.render(&pipeline, &pixel, window);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  if(!offline) {
    std::cout << renderer.currentFrame << std::endl;
  }

  glfwTerminate();

  return 0;
}
