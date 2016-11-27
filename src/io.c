#include "defs.h"

int load_settings(s_settings *settings, const char *filename)
{
  if(settings == NULL) {return -1;}
  if(filename == NULL) {return -2;}

  FILE *file = fopen(filename, "r");
  if(file == NULL) {return -2;}

  char line[512];
  while(fgets(line, sizeof(line), file))
  {
    if(strlen(line) < 3)
    {
      continue;
    }
    
    char *setting = strtok(line, "=");
    char *value = strtok(NULL, "\n\0");

    if(setting[0] == '#') {continue;}

    if(strncmp(setting, "width", 5) == 0)
    {
      settings->window_width = atoi(value);
    }
    else if(strncmp(setting, "height", 6) == 0)
    {
      settings->window_height = atoi(value);
    }
    else if(strncmp(setting, "fullscreen", 10) == 0)
    {
      if(strncmp(value, "true", 4) == 0)
      {
        settings->window_fullscreen = GL_TRUE;
      }
      else
      {
        settings->window_fullscreen = GL_FALSE;
      }
    }
    else if(strncmp(setting, "trippy", 6) == 0)
    {
      settings->trippy = atoi(value);
    }
    else if(strncmp(setting, "iterations", 10) == 0)
    {
      settings->iterations = atoi(value);
    }
    else
    {
      print_log("ERROR: Unknown setting (%s) value (%s)\n", setting, value);
    }
  }

  fclose(file);
  return 0;
}

int print_log(const char *format, ...)
{
  FILE *file = fopen("log.txt", "a");
  if(!file) {return -1;}

  va_list args;
  va_start(args, format);
  vfprintf(file, format, args);
  va_end(args);

  fclose(file);
  return 0;
}

void print_log_shader_info(GLuint shader_index)
{
  int max_length = 2048;
  int actual_length = 0;
  char log[2048];
  glGetShaderInfoLog(shader_index, max_length, &actual_length, log);
  print_log("Shader info log for GL index %i: %s\n", shader_index, log);
}

int bmp_load(s_texture* texture, const char *filename)
{
  if(filename == NULL) {return -1;}
  FILE *file = fopen(filename, "rb");
  if(file == NULL) {return -2;}
  
  unsigned char header[54];
  if(fread(header, 1, 54, file) != 54) {fclose(file); return -3;}
  if(header[0] != 'B' || header[1] != 'M') {fclose(file); return -4;}
  
  int data_pos   = *(int*)&(header[0x0A]);
  int image_size = *(int*)&(header[0x22]);
  int w = *(int*)&(header[0x12]);
  int h = *(int*)&(header[0x16]);
  //int bpp = *(int*)&(header[0x1C]); // bits per pixel
  int bpp = 24;
  int row_size = ((bpp*w+31)/32)*4;
  int padding = row_size - 3*w;
  
  assert(0 <= padding && padding <= 3);
  
  if(image_size == 0) {image_size = 3*w*h;}
  if(data_pos == 0) {data_pos = 54;}
  //fseek(file, data_pos);
  
  unsigned char *data = malloc(3 * w * h * 2 * sizeof(*data));
  if(data == NULL) {fclose(file); return -5;}
  
  unsigned char junk[3];
  
  int total = 0;
  
  //int r = fread(data, 1, 3 * w * h, file);
  for(int y = 0; y < h; ++y)
  {
    total += fread(data + y*3*w, sizeof(*data), 3*w, file);
    if(padding != 0)
    {
      fread(junk, sizeof(*junk), padding, file);
    }
  }
  
  assert((3*w+padding)*h == image_size);
  assert(ftell(file) == 54 + image_size);
  
  // Swap from BGR to RGB
  for(int i = 0; i < 3 * w * h; i += 3)
  {
    unsigned char temp = data[i+0];
    data[i+0] = data[i+2];
    data[i+2] = temp;
  }
  
  fclose(file);
  
  texture->w = w;
  texture->h = h;
  texture->data = data;
  
  return 0;
}
