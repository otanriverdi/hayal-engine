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

typedef struct renderer {
  GLuint vao;
  GLuint vbo;
  GLuint program;
  GLuint default_texture;
  int framebuffer_width;
  int framebuffer_height;
} renderer;

typedef struct {
  float x, y, r, g, b, a, u, v;
} vertex;

static GLuint compile_shader(GLenum kind, const char *src) {
  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  return shader;
};

static GLuint load_texture(uint8_t pixels[], int width, int height) {
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

renderer renderer_init(int framebuffer_width, int framebuffer_height) {
  renderer renderer = {
      .framebuffer_width = framebuffer_width,
      .framebuffer_height = framebuffer_height,
  };

  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLuint fragment_shader =
      compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

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

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                        (const void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
                        (const void *)offsetof(vertex, r));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                        (const void *)offsetof(vertex, u));
  glEnableVertexAttribArray(2);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  uint8_t white[4] = {255, 255, 255, 255};
  renderer.default_texture = load_texture(white, 1, 1);

  return renderer;
};

void renderer_destroy(renderer *renderer) {
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteTextures(1, &renderer->default_texture);
  glDeleteProgram(renderer->program);
}

static const vec2 DEFAULT_UVS[4] = {
    {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};

static void calculate_2d_quad_points(vec2 pos, vec2 size, int framebuffer_width,
                                     int framebuffer_height, vec2 out[4]) {
  vec2 framebuffer_size = {.x = (float)framebuffer_width,
                           .y = (float)framebuffer_height};
  vec2 flipped_pos = {pos.x, framebuffer_size.y - pos.y - size.y};
  vec2 norm_pos = vec2_sub_scalar(
      vec2_mul_scalar(vec2_div(flipped_pos, framebuffer_size), 2.0f), 1.0f);
  vec2 norm_size = vec2_mul_scalar(vec2_div(size, framebuffer_size), 2.0f);

  out[0] = norm_pos;
  out[1] = (vec2){norm_pos.x + norm_size.x, norm_pos.y};
  out[2] = vec2_add(norm_pos, norm_size);
  out[3] = (vec2){norm_pos.x, norm_pos.y + norm_size.y};
}

static void build_2d_triangle_mesh(vec2 points[3], vec2 uvs[3],
                                   rgba_float color, vertex out[3]) {
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

static void build_2d_quad_mesh(vec2 points[4], vec2 uvs[4], rgba_float color,
                               vertex out[6]) {
  vertex bl_vertices[3];
  vec2 bl_triangle_points[3] = {points[0], points[1], points[3]};
  vec2 bl_triangle_uvs[3] = {uvs[0], uvs[1], uvs[3]};
  build_2d_triangle_mesh(bl_triangle_points, bl_triangle_uvs, color,
                         bl_vertices);

  vertex tr_vertices[3];
  vec2 tr_triangle_points[3] = {points[3], points[1], points[2]};
  vec2 tr_triangle_uvs[3] = {uvs[3], uvs[1], uvs[2]};
  build_2d_triangle_mesh(tr_triangle_points, tr_triangle_uvs, color,
                         tr_vertices);

  out[0] = bl_vertices[0];
  out[1] = bl_vertices[1];
  out[2] = bl_vertices[2];
  out[3] = tr_vertices[0];
  out[4] = tr_vertices[1];
  out[5] = tr_vertices[2];
};

static void render_2d_mesh(renderer *renderer, const vertex *vertices,
                           size_t vertices_len, GLuint texture) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(renderer->program, "uTexture"), 0);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices_len, vertices,
               GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, vertices_len);
};

rgba_float normalize_color(rgba color) {
  return rgba_div_scalar(color, 255.0f);
}

void renderer_process_commands(renderer *renderer, render_commands *commands) {
  rgba_float clear = normalize_color(commands->clear);
  glClearColor(clear.r, clear.g, clear.b, clear.a);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < commands->rects.len; i++) {
    rect rect = commands->rects.data[i];

    vec2 points[4];
    calculate_2d_quad_points(rect.pos, rect.size, renderer->framebuffer_width,
                             renderer->framebuffer_height, points);

    rgba_float gl_color = normalize_color(rect.color);
    vertex vertices[6];
    build_2d_quad_mesh(points, (vec2 *)DEFAULT_UVS, gl_color, vertices);

    render_2d_mesh(renderer, vertices, 6, renderer->default_texture);
  }

  for (int i = 0; i < commands->sprites.len; i++) {
    sprite sprite = commands->sprites.data[i];

    if (sprite.asset->texture_id == 0) {
      sprite.asset->texture_id = load_texture(
          sprite.asset->data, sprite.asset->size.x, sprite.asset->size.y);
      ;
    }

    vec2 points[4];
    calculate_2d_quad_points(sprite.pos, sprite.size,
                             renderer->framebuffer_width,
                             renderer->framebuffer_height, points);

    vertex vertices[6];
    build_2d_quad_mesh(points, (vec2 *)DEFAULT_UVS,
                       (rgba_float){1.0, 1.0, 1.0, 1.0}, vertices);

    render_2d_mesh(renderer, vertices, 6, sprite.asset->texture_id);
  }
}
