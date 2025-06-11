#include "render.h"
#include <glad.h>
#include <stddef.h>
#include <stdlib.h>

const char *vertex_shader_src = "#version 330 core\n"
                                "layout (location = 0) in vec2 aPos;\n"
                                "layout (location = 1) in vec4 aColor;\n"
                                "layout (location = 2) in vec2 aTexCoord;\n"
                                "\n"
                                "out vec4 vertexColor;\n"
                                "out vec2 TexCoord;\n"
                                "\n"
                                "void main() {\n"
                                "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                "    vertexColor = aColor;\n"
                                "    TexCoord = aTexCoord;\n"
                                "}\n";

const char *fragment_shader_src =
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D uTexture;\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main() {\n"
    "    vec4 texColor = texture(uTexture, TexCoord);\n"
    "    FragColor = texColor * vertexColor;\n"
    "}\n";

struct Renderer {
  GLuint vao;
  GLuint vbo;
  GLuint program;
  GLuint default_texture;
  int framebuffer_width;
  int framebuffer_height;
};

typedef struct {
  float x, y, r, g, b, a, u, v;
} Vertex;

static GLuint CompileShader(GLenum kind, const char *src) {
  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  return shader;
};

static GLuint LoadTexture(uint8_t pixels[], int width, int height) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

Renderer RendererInit(int framebuffer_width, int framebuffer_height) {
  Renderer renderer = {
      .framebuffer_width = framebuffer_width,
      .framebuffer_height = framebuffer_height,
  };

  GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, vertex_shader_src);
  GLuint fragment_shader =
      CompileShader(GL_FRAGMENT_SHADER, fragment_shader_src);

  renderer.program = glCreateProgram();

  glAttachShader(renderer.program, vertex_shader);
  glAttachShader(renderer.program, fragment_shader);
  glLinkProgram(renderer.program);

  glDetachShader(renderer.program, vertex_shader);
  glDetachShader(renderer.program, fragment_shader);

  glUseProgram(renderer.program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glGenVertexArrays(1, &renderer.vao);
  glGenBuffers(1, &renderer.vbo);
  glBindVertexArray(renderer.vao);
  glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void *)offsetof(Vertex, r));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void *)offsetof(Vertex, u));
  glEnableVertexAttribArray(2);

  uint8_t white[4] = {255, 255, 255, 255};
  renderer.default_texture = LoadTexture(white, 1, 1);

  return renderer;
};

void RendererDestroy(Renderer *renderer) {
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteTextures(1, &renderer->default_texture);
  glDeleteProgram(renderer->program);
}

void RendererProcessCommands(Renderer *renderer, RenderCommands *commands) {
  float clear_r = commands->clear[0] / 255.0f;
  float clear_g = commands->clear[1] / 255.0f;
  float clear_b = commands->clear[2] / 255.0f;
  float clear_a = commands->clear[3] / 255.0f;

  glClearColor(clear_r, clear_g, clear_b, clear_a);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < commands->rects.len; i++) {
    // rects;
  }

  for (int i = 0; i < commands->sprites.len; i++) {
    // sprites;
  }
}
