#include <main.hpp>
#include <pipeline.hpp>

Pipeline::Pipeline(int w, int h) : width(w), height(h) {
  this->createTexture();
  this->createProgram();
  this->createBuffers();
}

void Pipeline::draw(float frame) {
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, imageData);

  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "image"), 0);
  glUniform1f(glGetUniformLocation(program, "frame"), frame);

  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

GLuint Pipeline::createShader(const char* path, GLenum type) {
  std::ifstream v(path);
  std::stringstream buffer;
  
  buffer << v.rdbuf();
  buffer << "\0";
  v.close();
  
  std::string code = buffer.str().c_str();
  auto src = code.c_str();
  
  auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status == GL_FALSE) {
    std::cout << "Error compiling shader." << std::endl;
    
    int maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    
    std::vector<char> errors(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errors[0]);
    
    std::cout << &errors[0] << std::endl;
    
    exit(1);
  }

  return shader;
}

void Pipeline::createProgram() {
  program = glCreateProgram();
  glAttachShader(program, this->createShader(PROJECT_SOURCE_DIR "/shaders/vert.glsl", GL_VERTEX_SHADER));
  glAttachShader(program, this->createShader(PROJECT_SOURCE_DIR "/shaders/frag.glsl", GL_FRAGMENT_SHADER));
  glLinkProgram(program);
  
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(status != GL_TRUE) {
    std::cout << "NOOOOOPE" << std::endl;
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    
    std::vector<char> errors(length);
    glGetProgramInfoLog(program, length, &length, &errors[0]);
    
    std::cout << &errors[0];
    exit(1);
  }
}

void Pipeline::createTexture() {
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Pipeline::createBuffers() {
  GLfloat vertices[] = {
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
  };

  GLuint indices[] = { 0, 1, 3, 1, 2, 3 };

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glGenVertexArrays(1, &vao);

  glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), (GLvoid*)(3*sizeof(GLfloat)));
    glBindVertexArray(0);
  glBindVertexArray(0);
}
