#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define GPU_RENDERER 1
#define CPU_RENDERER 0

typedef struct
{
  float pos[2];
  float zoom;
} s_camera;

typedef struct
{
  int window_width;
  int window_height;
  float window_ratio;
  int window_fullscreen;
  s_camera cam;
  int iterations;
  int trippy;
  int frame;
  int redraw;
  int renderer;
} s_settings;

typedef struct
{
  int w;
  int h;
  unsigned char *data;
} s_texture;

// io.c
int load_settings(s_settings *settings, const char *filename);
int print_log(const char *format, ...);
void print_log_shader_info(GLuint shader_index);
int bmp_load(s_texture* texture, const char *filename);

// callbacks.c
void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void glfw_keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// shaders.c
int create_shader(const char* filename, int type);

#endif // DEFS_H_INCLUDED
