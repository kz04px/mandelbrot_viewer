#version 430

uniform sampler1D tex;
uniform vec2 center;
uniform float scale;
uniform int iter;
uniform int offset;

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

void main()
{
  vec2 z, c;
  
  c.x = 1.3333 * (uv.x - 0.5) * scale - center.x;
  c.y = (uv.y - 0.5) * scale - center.y;
  
  z = c;
  
  int i;
  for(i = 0; i < iter; ++i)
  {
    float x = (z.x * z.x - z.y * z.y) + c.x;
    float y = (z.y * z.x + z.x * z.y) + c.y;

    if((x * x + y * y) > 4.0) {break;}
    z.x = x;
    z.y = y;
  }
  
  if(i == iter)
  {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else
  {
    FragColor = vec4(texture(tex, (float(i) + float(offset)/10) / 100.0).rgb, 1.0);
  }
}