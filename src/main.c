#include "defs.h"
#include "matrix.h"

void update_fps_counter(GLFWwindow* window)
{
  static double previous_seconds = 0;
  static int frame_count = 0;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if(elapsed_seconds > 0.25)
  {
    previous_seconds = current_seconds;
    double fps = (double)frame_count/elapsed_seconds;
    char tmp[128];
    sprintf(tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frame_count = 0;
  }
  frame_count++;
}

int main()
{
  int r;
  GLenum err;
  
  print_log("----------\n");
  
  s_settings* settings = malloc(1*sizeof(s_settings));
  
  // Default values
  settings->window_width = 640;
  settings->window_height = 480;
  settings->window_fullscreen = GL_FALSE;
  settings->frame = 0;
  settings->cam.pos[0] = 0.0;
  settings->cam.pos[1] = 0.0;
  settings->cam.zoom = 1.0;
  settings->iterations = 50;
  settings->trippy = 0;
  settings->redraw = 1;
  settings->renderer = GPU_RENDERER;
  
  r = load_settings(settings, "settings.dat");
  if(r != 0)
  {
    print_log("WARNING: Failed to load settings.dat\n");
  }
  settings->window_ratio = (float)settings->window_width/settings->window_height;
  
  // start GL context and O/S window using the GLFW helper library
  if(!glfwInit())
  {
    print_log("ERROR: could not start GLFW3\n");
    return 1;
  }
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  GLFWwindow *window = NULL;
  if(settings->window_fullscreen == GL_TRUE)
  {
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* vmode = glfwGetVideoMode(mon);
    window = glfwCreateWindow(vmode->width, vmode->height, "Extended GL Init", mon, NULL);
  }
  else
  {
    window = glfwCreateWindow(settings->window_width, settings->window_height, "Hello Triangle", NULL, NULL);
  }
  if(!window)
  {
    print_log("ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  
  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
  glfwSetScrollCallback(window, glfw_mouse_scroll_callback);
  glfwSetKeyCallback(window, glfw_keyboard_callback);
  glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
  
  glfwSetWindowUserPointer(window, settings);

  // Start GLEW extension handler
  glewExperimental = GL_TRUE;
  err = glewInit();
  while((err = glGetError()) != GL_NO_ERROR)
  {
    print_log("ERROR: glewInit() (%i)\n", err);
  }
  
  #ifndef NDEBUG
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString(GL_VERSION); // version as a string
  
  printf("Debug info:\n");
  printf(" Date: %s\n", __DATE__);
  printf(" Time: %s\n", __TIME__);
  printf(" Renderer: %s\n", renderer);
  printf(" OpenGL version supported %s\n", version);
  printf("\n");
  #endif
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  // Create shaders
  GLuint vs = create_shader("shaders//vertex_shader.glsl", GL_VERTEX_SHADER);
  GLuint fs = create_shader("shaders//fragment_shader.glsl", GL_FRAGMENT_SHADER);
  
  // Create shader program
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  glLinkProgram(shader_program);
  
  // Create buffers
  float points[] = {
     -2.0, -1.0, 0.0,
     -2.0,  1.0, 0.0,
      1.0,  1.0, 0.0,
      1.0, -1.0, 0.0};
  
  float uv[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0};
  
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof *points, points, GL_STATIC_DRAW);
  
  GLuint uvbo = 0;
  glGenBuffers(1, &uvbo);
  glBindBuffer(GL_ARRAY_BUFFER, uvbo);
  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof *uv, uv, GL_STATIC_DRAW);
  
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uvbo);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  
  s_texture palette;
  bmp_load(&palette, "textures//palette.bmp");
  
  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  
  // Bind texture
  glBindTexture(GL_TEXTURE_1D, texture_id);
  
  // Texture sampling/filtering operation.
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, palette.w, 0, GL_RGB, GL_UNSIGNED_BYTE, palette.data);
  
  // Find uniforms
  GLint loc_vp_matrix = glGetUniformLocation(shader_program, "vp_matrix");
  if(loc_vp_matrix < 0) {print_log("ERROR: Could not find uniform vp_matrix\n"); return -1;}
  
  GLint center_loc = glGetUniformLocation(shader_program, "center");
  if(center_loc < 0) {print_log("ERROR: Could not find uniform center\n"); return -1;}
  
  GLint scale_loc = glGetUniformLocation(shader_program, "scale");
  if(scale_loc < 0) {print_log("ERROR: Could not find uniform scale\n"); return -1;}
  
  GLint iter_loc = glGetUniformLocation(shader_program, "iter");
  if(iter_loc < 0) {print_log("ERROR: Could not find uniform iter\n"); return -1;}
  
  GLint offset_loc = glGetUniformLocation(shader_program, "offset");
  if(offset_loc < 0) {print_log("ERROR: Could not find uniform offset\n"); return -1;}
  
  // Draw the first frame
  s_mat4 vp_matrix = ortho(-2.0,1.0, -1.0,1.0, 0.0,1.0);
  
  // Set uniforms
  glUniformMatrix4fv(loc_vp_matrix, 1, GL_FALSE, vp_matrix.m);
  glUniform2fv(center_loc, 1, settings->cam.pos);
  glUniform1f(scale_loc, settings->cam.zoom);
  glUniform1i(iter_loc, settings->iterations);
  glUniform1i(offset_loc, settings->frame);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shader_program);
  
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  
  // Drawing
  glClearColor(0.0, 0.0, 0.0, 1.0);
  while(!glfwWindowShouldClose(window))
  {
    update_fps_counter(window);
    
    if(settings->redraw)
    {
      if(settings->renderer == GPU_RENDERER)
      {
        s_mat4 vp_matrix = ortho(-2.0,1.0, -1.0,1.0, 0.0,1.0);
        
        // Set uniforms
        glUniformMatrix4fv(loc_vp_matrix, 1, GL_FALSE, vp_matrix.m);
        glUniform2fv(center_loc, 1, settings->cam.pos);
        glUniform1f(scale_loc, settings->cam.zoom);
        glUniform1i(iter_loc, settings->iterations);
        glUniform1i(offset_loc, settings->frame);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      else
      {
        
      }
    }
    
    settings->redraw = 0;
    
    glfwPollEvents();
    glfwSwapBuffers(window);
    
    if(settings->trippy)
    {
      settings->frame++;
      settings->redraw = 1;
    }
  }
  
  glfwTerminate();
  return 0;
}
