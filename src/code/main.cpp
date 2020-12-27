#include <main.hpp>
#include <pixel.hpp>
#include <pipeline.hpp>
#include <progressbar.hpp>
#include <renderer.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int width = 1280/2;
int height = 720/2;

int pass = 200;
int frames = 1;
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

  if(argc > 2 && std::string("prerender").compare(argv[2]) == 0) {
    prerender = true;
    if(argc > 3 && std::string("save").compare(argv[3]) == 0) save = true;
    imgPath = argv[4];
  } else if(argc > 2 && std::string("playback").compare(argv[2]) == 0) {
    load = true;
    imgPath = argv[3];
    std::ifstream forFrame(imgPath);
    forFrame >> frames;
    forFrame.close();
  }

  bool prerendered = prerender || load;

  if (!glfwInit()) {
      exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  GLFWwindow* window = glfwCreateWindow(width * 2, height * 2, "My Title", NULL, NULL);
  glfwMakeContextCurrent(window);

  if (window == nullptr) {
      std::cout << "Failed to create OpenGL context.\n";
      exit(1);
  }
  
  if(!gladLoadGL()) {
    std::cout << "Failed to load openGL" << std::endl;
    exit(1);
  }

  if(glGetError() != GL_NO_ERROR) {
    std::cout << "There is an error going on.";
    exit(1);
  }
  
  std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  //ImGui::StyleColorsDark();

  ImGui_ImplOpenGL3_Init("#version 330");
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  
  Pixel pixel(scene, width, height);
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
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if(prerendered) {
      renderer.play(&pipeline);
    } else {
      renderer.render(&pipeline, &pixel, window);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  if(!prerendered) {
    std::cout << renderer.currentFrame << std::endl;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
