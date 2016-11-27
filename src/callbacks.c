#include "defs.h"

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
  s_settings *settings = glfwGetWindowUserPointer(window);
  if(settings == NULL) {return;}
  
  assert(width >= 0);
  assert(height >= 0);
  
  settings->window_width = width;
  settings->window_height = height;
  settings->window_ratio = (float)width/height;
  
  glViewport(0, 0, width, height);
  settings->redraw = 1;
}

int camera_moving = 0;

void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  s_settings *settings = glfwGetWindowUserPointer(window);
  if(settings == NULL) {return;}
  
  static double xlast = 0;
  static double ylast = 0;
  
  if(camera_moving == 1)
  {
    settings->cam.pos[0] += 1.0*(xpos - xlast)/settings->window_width*settings->cam.zoom*settings->window_ratio;
    settings->cam.pos[1] -= 1.0*(ypos - ylast)/settings->window_height*settings->cam.zoom;
    settings->redraw = 1;
  }
  
  xlast = xpos;
  ylast = ypos;
}

void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  s_settings *settings = glfwGetWindowUserPointer(window);
  if(settings == NULL) {return;}
  
  if(yoffset > 0.0) {settings->cam.zoom /= 1.1;}
  else              {settings->cam.zoom *= 1.1;}
  
  #define ZOOM_MIN 0.00000001
  #define ZOOM_MAX 5.0
  
       if(settings->cam.zoom < ZOOM_MIN) {settings->cam.zoom = ZOOM_MIN;}
  else if(settings->cam.zoom > ZOOM_MAX) {settings->cam.zoom = ZOOM_MAX;}
  else {settings->redraw = 1;}
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  s_settings *settings = glfwGetWindowUserPointer(window);
  if(settings == NULL) {return;}
  
  // Button:
  // 0 - Left
  // 1 - Right
  // 2 - Middle

  // Action:
  // 1 - Down
  // 0 - Up
  
  if(button == 1)
  {
    camera_moving = action;
  }
}

void glfw_keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  s_settings *settings = glfwGetWindowUserPointer(window);
  if(settings == NULL) {return;}
  
  // Action:
  //  1 - Press
  //  2 - Repeat
  //  0 - Release
  
  if(action == 0) {return;}

  switch(key)
  {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, 1);
      break;
    case GLFW_KEY_PAGE_UP:
      settings->iterations += 50;
      settings->redraw = 1;
      break;
    case GLFW_KEY_PAGE_DOWN:
      settings->iterations -= 50;
      settings->redraw = 1;
      
      if(settings->iterations < 0)
      {
        settings->iterations = 0;
      }
      break;
    case GLFW_KEY_KP_ADD:
      settings->iterations++;
      settings->redraw = 1;
      break;
    case GLFW_KEY_KP_SUBTRACT:
      settings->iterations--;
      settings->redraw = 1;
      
      if(settings->iterations < 0)
      {
        settings->iterations = 0;
      }
      break;
    case GLFW_KEY_R:
      settings->frame = 0;
      settings->cam.pos[0] = 0.0;
      settings->cam.pos[1] = 0.0;
      settings->cam.zoom = 1.0;
      settings->iterations = 50;
      settings->trippy = 0;
      settings->redraw = 1;
      break;
    case GLFW_KEY_T:
      settings->trippy = 1-settings->trippy;
      settings->redraw = 1;
      break;
    case GLFW_KEY_S:
      printf("Position:\n");
      printf("Zoom: %f\n", settings->cam.zoom);
      printf("Position: %f %f\n", settings->cam.pos[0], settings->cam.pos[1]);
      printf("iterations: %i\n", settings->iterations);
      printf("Tripply: %i\n", settings->trippy);
      printf("\n");
      break;
    default:
      printf("Key: %c(%i) %i\n", key, key, action);
      break;
  }
}
