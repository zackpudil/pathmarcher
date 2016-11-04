#pragma once

class Pipeline {
private:
  int width;
  int height;

  GLuint program;
  GLuint vbo;
  GLuint ebo;
  GLuint vao;
  GLuint texture;

  void createProgram();
  GLuint createShader(const char*, GLenum);
  void createTexture();
  void createBuffers();

public:
  float* imageData;

  Pipeline(int, int);
  void draw(float);
};