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
  
  //c.x = center.x + scale + iter;
  
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
  
  /*
  double z[2], c[2];
  
  c[0] = 1.3333 * (uv.x - 0.5) * scale - center.x;
  c[1] = (uv.y - 0.5) * scale - center.y;
  
  z[0] = c[0];
  z[1] = c[1];
  
  int i;
  for(i = 0; i < iter; ++i)
  {
    double x = (z[0] * z[0] - z[1] * z[1]) + c[0];
    double y = (z[1] * z[0] + z[0] * z[1]) + c[1];

    if((x * x + y * y) > 4.0) {break;}
    z[0] = x;
    z[1] = y;
  }
  */
  
  if(i == iter)
  {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else
  {
    FragColor = vec4(texture(tex, (float(i) + float(offset)/10) / 100.0).rgb, 1.0);
  }
}