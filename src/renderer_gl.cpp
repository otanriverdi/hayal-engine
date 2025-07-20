#include "platform.hpp"
#include "renderer.hpp"
#include <glad.h>
#include <glm/glm.hpp>
#include <stdlib.h>

static char *load_shader(const char *path) {
  size_t file_size;
  platform_get_file_size(path, &file_size);
  // TODO: should we use an allocator here?
  char *file_memory = static_cast<char *>(malloc(file_size + 1));
  platform_read_entire_file(path, file_size, file_memory);
  file_memory[file_size] = '\0';
  return file_memory;
}

static GLuint compile_shader(GLenum kind, const char *src) {
  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  return shader;
};

static GLuint load_sprite_texture(uint8_t pixels[], int width, int height) {
  GLuint texture;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  return texture;
}

struct renderer {
  GLuint quad_program;
  unsigned int quad_vbo;
  unsigned int quad_vao;
  unsigned int quad_ebo;
  GLuint empty_texture;
  glm::vec2 framebuffer_size;
  glm::vec2 camera_pos;
  GLint model_loc;
  GLint view_loc;
  GLint projection_loc;
  GLint texture_loc;
  GLint color_loc;
  GLint single_channel_loc;
};

renderer renderer_init(int framebuffer_width, int framebuffer_height) {
  renderer renderer = {.framebuffer_size = glm::vec2(static_cast<float>(framebuffer_width),
                                                     static_cast<float>(framebuffer_height))};

  // Create quad program
  renderer.quad_program = glCreateProgram();
  char *vertex_shader_src = load_shader("shaders/default_vertex.glsl");
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
  glAttachShader(renderer.quad_program, vertex_shader);
  char *fragment_shader_src = load_shader("shaders/default_fragment.glsl");
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
  glAttachShader(renderer.quad_program, fragment_shader);
  glLinkProgram(renderer.quad_program);
  free(vertex_shader_src);
  free(fragment_shader_src);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // Create quad VAO
  float quad_vertices[] = {
      // positions        // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 0.0f  // top left
  };
  unsigned int quad_indices[] = {0, 1, 3, 1, 2, 3};
  glGenVertexArrays(1, &renderer.quad_vao);
  glGenBuffers(1, &renderer.quad_vbo);
  glGenBuffers(1, &renderer.quad_ebo);
  glBindVertexArray(renderer.quad_vao);

  glBindBuffer(GL_ARRAY_BUFFER, renderer.quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.quad_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)0); // position
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float))); // color
  glEnableVertexAttribArray(1);

  // Create empty texture
  uint8_t white[4] = {255, 255, 255, 255};
  renderer.empty_texture = load_sprite_texture(white, 1, 1);

  // Cache uniform locations
  renderer.model_loc = glGetUniformLocation(renderer.quad_program, "model");
  renderer.view_loc = glGetUniformLocation(renderer.quad_program, "view");
  renderer.projection_loc = glGetUniformLocation(renderer.quad_program, "projection");
  renderer.texture_loc = glGetUniformLocation(renderer.quad_program, "uTexture");
  renderer.color_loc = glGetUniformLocation(renderer.quad_program, "uColor");
  renderer.single_channel_loc = glGetUniformLocation(renderer.quad_program, "uIsSingleChannel");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return renderer;
}

void renderer_destroy(renderer *renderer) {
  glDeleteBuffers(1, &renderer->quad_vbo);
  glDeleteBuffers(1, &renderer->quad_ebo);
  glDeleteVertexArrays(1, &renderer->quad_vao);
  glDeleteTextures(1, &renderer->empty_texture);
  glDeleteProgram(renderer->quad_program);
}

static void render_quad(renderer *renderer, GLuint texture_id, glm::vec3 pos, glm::vec2 size,
                        glm::vec4 color) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, pos);
  model = glm::scale(model, glm::vec3(size.x, size.y, 0.0f));
  glUniformMatrix4fv(renderer->model_loc, 1, GL_FALSE, glm::value_ptr(model));
  glUniform4f(renderer->color_loc, color.r, color.g, color.b, color.a);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderer_move_camera(struct renderer *renderer, glm::vec2 delta) {
  renderer->camera_pos = renderer->camera_pos + delta;
}

void renderer_begin_frame(struct renderer *renderer) {
  glUseProgram(renderer->quad_program);
  glBindVertexArray(renderer->quad_vao);
  glUniform1i(renderer->texture_loc, 0);
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(-renderer->camera_pos.x, -renderer->camera_pos.y, 0.0f));
  glUniformMatrix4fv(renderer->view_loc, 1, GL_FALSE, glm::value_ptr(view));
  glm::mat4 projection =
      glm::ortho(0.0f, renderer->framebuffer_size.x, 0.0f, renderer->framebuffer_size.y, -1.0f, 10.0f);
  glUniformMatrix4fv(renderer->projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
}

void renderer_render_clear(struct renderer *renderer, glm::vec4 color) {
  glm::vec4 gl_color = color / 255.0f;
  glClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_render_quad(struct renderer *renderer, render_cmd_quad quad) {
  glm::vec4 gl_color = glm::vec4(quad.color) / 255.0f;
  GLuint texture_id = quad.texture_id != 0 ? quad.texture_id : renderer->empty_texture;
  glUniform1i(renderer->single_channel_loc, GL_FALSE);
  render_quad(renderer, texture_id, quad.pos, quad.size, gl_color);
}

void renderer_render_glyph(struct renderer *renderer, render_cmd_glyph glyph) {
  glm::vec4 gl_color = glm::vec4(glyph.color) / 255.0f;
  glUniform1i(renderer->single_channel_loc, GL_TRUE);
  assert(glyph.texture_id != 0);
  render_quad(renderer, glyph.texture_id, glyph.pos, glyph.size, gl_color);
}

void renderer_delete_texture(struct renderer *renderer, render_cmd_delete_texture delete_texture) {
  glDeleteTextures(1, delete_texture.texture_id);
  *delete_texture.texture_id = 0;
}

void renderer_load_texture(struct renderer *renderer, render_cmd_load_texture load_texture) {
  *load_texture.texture_id = load_sprite_texture(load_texture.data, load_texture.size.x, load_texture.size.y);
}

void renderer_load_glyph(struct renderer *renderer, render_cmd_load_glyph load_glyph) {
  GLuint texture;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, load_glyph.size.x, load_glyph.size.y, 0, GL_RED, GL_UNSIGNED_BYTE,
               load_glyph.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  *load_glyph.texture_id = texture;
}
