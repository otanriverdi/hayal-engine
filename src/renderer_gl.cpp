#include "glad.h"
#include "renderer.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>

const char *vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vertexColor = aColor;
    TexCoord = aTexCoord;
}
)";

const char *fragment_shader_src = R"(
#version 330 core
in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    FragColor = texColor * vertexColor;
}
)";

static const std::array<glm::vec2, 4> DEFAULT_UVS = {
    glm::vec2{0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}};

GLuint CompileShader(GLenum kind, const char *src) {
  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  return shader;
};

GLuint LoadTexture(uint8_t pixels[], int width, int height) {
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

struct Vertex {
  float x, y, r, g, b, a, u, v;
};

std::array<Vertex, 3> Build2DTriangleMesh(std::array<glm::vec2, 3> points,
                                          std::array<glm::vec2, 3> uvs,
                                          glm::vec4 color) {
  return {
      Vertex{points[0].x, points[0].y, color.r, color.g, color.b, color.a,
             uvs[0].x, uvs[0].y},
      Vertex{points[1].x, points[1].y, color.r, color.g, color.b, color.a,
             uvs[1].x, uvs[1].y},
      Vertex{points[2].x, points[2].y, color.r, color.g, color.b, color.a,
             uvs[2].x, uvs[2].y},
  };
}

std::array<Vertex, 6> Build2DQuadMesh(std::array<glm::vec2, 4> points,
                                      std::array<glm::vec2, 4> uvs,
                                      glm::vec4 color) {
  std::array<Vertex, 3> bl_triangle = Build2DTriangleMesh(
      {points[0], points[1], points[3]}, {uvs[0], uvs[1], uvs[3]}, color);
  std::array<Vertex, 3> tr_triangle = Build2DTriangleMesh(
      {points[3], points[1], points[2]}, {uvs[3], uvs[1], uvs[2]}, color);

  return {
      bl_triangle[0], bl_triangle[1], bl_triangle[2],
      tr_triangle[0], tr_triangle[1], tr_triangle[2],
  };
};

std::array<glm::vec2, 4> Calculate2DQuadPoints(glm::vec2 pos, glm::vec2 size,
                                               glm::vec2 framebuffer_size) {
  glm::vec2 norm_pos = ((pos / framebuffer_size) * 2.0f) - 1.0f;
  glm::vec2 norm_size = (size / framebuffer_size) * 2.0f;
  return {
      norm_pos,
      {norm_pos.x + norm_size.x, norm_pos.y},
      norm_pos + norm_size,
      {norm_pos.x, norm_pos.y + norm_size.y},
  };
};

struct Renderer::Renderer {
  GLuint vao;
  GLuint vbo;
  GLuint program;
  GLuint default_texture;
  glm::vec2 framebuffer_size;
};

void Render2DMesh(Renderer::Renderer *renderer, const Vertex *vertices,
                  size_t vertices_len, GLuint texture) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(renderer->program, "uTexture"), 0);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_len, vertices,
               GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, vertices_len);
};

Renderer::Renderer Renderer::InitRenderer(int framebuffer_width,
                                          int framebuffer_height) {
  Renderer renderer = {};
  renderer.framebuffer_size = {framebuffer_width, framebuffer_height};

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

void Renderer::DestroyRenderer(Renderer *renderer) {
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteTextures(1, &renderer->default_texture);
  glDeleteProgram(renderer->program);
};

void Renderer::RenderCommands(Renderer *renderer, Commands *commands) {
  glm::vec4 clear = commands->clear / 255.0f;
  glClearColor(clear.r, clear.g, clear.b, clear.a);
  glClear(GL_COLOR_BUFFER_BIT);

  for (Rect rect : commands->rects) {
    const std::array<glm::vec2, 4> points =
        Calculate2DQuadPoints(rect.pos, rect.size, renderer->framebuffer_size);
    const std::array<Vertex, 6> vertices =
        Build2DQuadMesh(points, DEFAULT_UVS, rect.color);
    Render2DMesh(renderer, vertices.data(), 6, renderer->default_texture);
  }
}
