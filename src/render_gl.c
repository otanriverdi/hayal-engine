#include "math.h"
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

static const Vec2 DEFAULT_UVS[4] = {
    {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};

static void Calculate2DQuadPoints(Vec2 pos, Vec2 size, int framebuffer_width,
                                  int framebuffer_height, Vec2 out[4]) {
  Vec2 framebuffer_size = {.x = (float)framebuffer_width,
                           .y = (float)framebuffer_height};
  Vec2 flipped_pos = { pos.x, framebuffer_size.y - pos.y - size.y };
  Vec2 norm_pos = Vec2SubScalar(Vec2MulScalar(Vec2Div(flipped_pos, framebuffer_size), 2.0f), 1.0f);
  Vec2 norm_size = Vec2MulScalar(Vec2Div(size, framebuffer_size), 2.0f);

  out[0] = norm_pos;
  out[1] = (Vec2){norm_pos.x + norm_size.x, norm_pos.y};
  out[2] = Vec2Add(norm_pos, norm_size);
  out[3] = (Vec2){norm_pos.x, norm_pos.y + norm_size.y};
}

static void Build2DTriangleMesh(Vec2 points[3], Vec2 uvs[3], RGBAf color,
                                Vertex out[3]) {
  for (int i = 0; i < 3; ++i) {
    out[i].x = points[i].x;
    out[i].y = points[i].y;
    out[i].r = color.r;
    out[i].g = color.g;
    out[i].b = color.b;
    out[i].a = color.a;
    out[i].u = uvs[i].x;
    out[i].v = uvs[i].y;
  }
}

static void Build2DQuadMesh(Vec2 points[4], Vec2 uvs[4], RGBAf color,
                            Vertex out[6]) {
  Vertex bl_vertices[3];
  Vec2 bl_triangle_points[3] = {points[0], points[1], points[3]};
  Vec2 bl_triangle_uvs[3] = {uvs[0], uvs[1], uvs[3]};
  Build2DTriangleMesh(bl_triangle_points, bl_triangle_uvs, color, bl_vertices);

  Vertex tr_vertices[3];
  Vec2 tr_triangle_points[3] = {points[3], points[1], points[2]};
  Vec2 tr_triangle_uvs[3] = {uvs[3], uvs[1], uvs[2]};
  Build2DTriangleMesh(tr_triangle_points, tr_triangle_uvs, color, tr_vertices);

  out[0] = bl_vertices[0];
  out[1] = bl_vertices[1];
  out[2] = bl_vertices[2];
  out[3] = tr_vertices[0];
  out[4] = tr_vertices[1];
  out[5] = tr_vertices[2];
};

static void Render2DMesh(Renderer *renderer, const Vertex *vertices,
                         size_t vertices_len, GLuint texture) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(renderer->program, "uTexture"), 0);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_len, vertices,
               GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, vertices_len);
};

RGBAf NormalizeColor(RGBA color) { return RGBADivScalar(color, 255.0f); }

void RendererProcessCommands(Renderer *renderer, RenderCommands *commands) {
  RGBAf clear = NormalizeColor(commands->clear);
  glClearColor(clear.r, clear.g, clear.b, clear.a);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < commands->rects.len; i++) {
    Rect rect = commands->rects.data[i];

    Vec2 points[4];
    Calculate2DQuadPoints(rect.pos, rect.size, renderer->framebuffer_width,
                          renderer->framebuffer_height, points);

    Vertex vertices[6];
    RGBAf gl_color = NormalizeColor(rect.color);
    Build2DQuadMesh(points, (Vec2 *)DEFAULT_UVS, gl_color, vertices);

    Render2DMesh(renderer, vertices, 6, renderer->default_texture);
  }

  for (int i = 0; i < commands->sprites.len; i++) {
    // Sprite sprite = commands->sprites.data[i];
  }
}
