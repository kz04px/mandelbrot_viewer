#version 430

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_uv;

layout(location = 0) out vec2 uv;

uniform mat4 vp_matrix;

void main()
{
  gl_Position = vp_matrix * vec4(vertex_position, 0.0, 1.0);
  uv = vertex_uv;
}
