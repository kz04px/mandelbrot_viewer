#include "matrix.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DEG_TO_RAD(x) (x*0.0174532925)
#define RAD_TO_DEG(x) (x/0.0174532925)

s_mat4 identity_matrix()
{
  s_mat4 mat;
  mat.m[ 0] = 1.0;
  mat.m[ 1] = 0.0;
  mat.m[ 2] = 0.0;
  mat.m[ 3] = 0.0;
  mat.m[ 4] = 0.0;
  mat.m[ 5] = 1.0;
  mat.m[ 6] = 0.0;
  mat.m[ 7] = 0.0;
  mat.m[ 8] = 0.0;
  mat.m[ 9] = 0.0;
  mat.m[10] = 1.0;
  mat.m[11] = 0.0;
  mat.m[12] = 0.0;
  mat.m[13] = 0.0;
  mat.m[14] = 0.0;
  mat.m[15] = 1.0;
  return mat;
}

s_vec4 vec4(float i, float j, float k, float l)
{
  s_vec4 vec;
  vec.i = i;
  vec.j = j;
  vec.k = k;
  vec.l = l;
  return vec;
}

s_vec3 vec3(float i, float j, float k)
{
  s_vec3 vec;
  vec.i = i;
  vec.j = j;
  vec.k = k;
  return vec;
}

float dot_vec4(s_vec4 u, s_vec4 v)
{
  return u.i*v.i + u.j*v.j + u.k*v.k + u.l*v.l;
}

float dot_vec3(s_vec3 u, s_vec3 v)
{
  return u.i*v.i + u.j*v.j + u.k*v.k;
}

s_vec3 cross_vec3(s_vec3 u, s_vec3 v)
{
  s_vec3 vec;
  vec.i = u.j*v.k - u.k*v.j;
  vec.j = u.k*v.i - u.i*v.k;
  vec.k = u.i*v.j - u.j*v.i;
  return vec;
}

s_mat4 mat4(float a, float b, float c, float d,
            float e, float f, float g, float h,
            float i, float j, float k, float l,
            float m, float n, float o, float p)
{
  s_mat4 mat;
  mat.m[ 0] = a; mat.m[ 1] = b; mat.m[ 2] = c; mat.m[ 3] = d;
  mat.m[ 4] = e; mat.m[ 5] = f; mat.m[ 6] = g; mat.m[ 7] = h;
  mat.m[ 8] = i; mat.m[ 9] = j; mat.m[10] = k; mat.m[11] = l;
  mat.m[12] = m; mat.m[13] = n; mat.m[14] = o; mat.m[15] = p;
  return mat;
}

s_mat3 mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
  s_mat3 mat;
  mat.m[0] = a; mat.m[1] = b; mat.m[2] = c;
  mat.m[3] = d; mat.m[4] = e; mat.m[5] = f;
  mat.m[6] = g; mat.m[7] = h; mat.m[8] = i;
  return mat;
}

s_mat4 perspective_matrix(float fov, float ratio, float near, float far)
{
	float f = cos(fov/2) / sin(fov/2); // cot(theta) = cos(theta)/sin(theta)
	return mat4(f/ratio,0,0,0,
              0,f,0,0,
              0,0,(far+near)/(near-far),2*far*near/(near-far),
              0,0,-1,0);
}

s_mat4 ortho(float l, float r, float b, float t, float n, float f)
{
  float tx = -(r+l)/(r-l);
  float ty = -(t+b)/(t-b);
  float tz = -(f+n)/(f-n);

  s_mat4 mat;
  mat.m[ 0] =  2/(r-l); mat.m[ 1] =  0;       mat.m[ 2] =  0;        mat.m[ 3] =  0;
  mat.m[ 4] =  0;       mat.m[ 5] =  2/(t-b); mat.m[ 6] =  0;        mat.m[ 7] =  0;
  mat.m[ 8] =  0;       mat.m[ 9] =  0;       mat.m[10] =  -2/(f-n); mat.m[11] =  0;
  mat.m[12] = tx;       mat.m[13] = ty;       mat.m[14] = tz;        mat.m[15] =  1;
  return mat;
}

s_mat4 fps_view_rh(s_vec3 eye, float pitch, float yaw)
{
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);
  float cosYaw = cos(yaw);
  float sinYaw = sin(yaw);

  s_vec3 xaxis = vec3(cosYaw, 0, -sinYaw);
  s_vec3 yaxis = vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
  s_vec3 zaxis = vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

  // Create a 4x4 view matrix from the right, up, forward and eye position vectors
  return mat4(xaxis.i, xaxis.j, xaxis.k, -dot_vec3(xaxis, eye),
              yaxis.i, yaxis.j, yaxis.k, -dot_vec3(yaxis, eye),
              zaxis.i, zaxis.j, zaxis.k, -dot_vec3(zaxis, eye),
              0, 0, 0, 1);
}

s_mat4 multiply_mat4(s_mat4 u, s_mat4 v)
{
  s_mat4 mat;
  mat.m[ 0] = u.m[ 0]*v.m[ 0] + u.m[ 1]*v.m[ 4] + u.m[ 2]*v.m[ 8] + u.m[ 3]*v.m[12];
  mat.m[ 1] = u.m[ 0]*v.m[ 1] + u.m[ 1]*v.m[ 5] + u.m[ 2]*v.m[ 9] + u.m[ 3]*v.m[13];
  mat.m[ 2] = u.m[ 0]*v.m[ 2] + u.m[ 1]*v.m[ 6] + u.m[ 2]*v.m[10] + u.m[ 3]*v.m[14];
  mat.m[ 3] = u.m[ 0]*v.m[ 3] + u.m[ 1]*v.m[ 7] + u.m[ 2]*v.m[11] + u.m[ 3]*v.m[15];

  mat.m[ 4] = u.m[ 4]*v.m[ 0] + u.m[ 5]*v.m[ 4] + u.m[ 6]*v.m[ 8] + u.m[ 7]*v.m[12];
  mat.m[ 5] = u.m[ 4]*v.m[ 1] + u.m[ 5]*v.m[ 5] + u.m[ 6]*v.m[ 9] + u.m[ 7]*v.m[13];
  mat.m[ 6] = u.m[ 4]*v.m[ 2] + u.m[ 5]*v.m[ 6] + u.m[ 6]*v.m[10] + u.m[ 7]*v.m[14];
  mat.m[ 7] = u.m[ 4]*v.m[ 3] + u.m[ 5]*v.m[ 7] + u.m[ 6]*v.m[11] + u.m[ 7]*v.m[15];

  mat.m[ 8] = u.m[ 8]*v.m[ 0] + u.m[ 9]*v.m[ 4] + u.m[10]*v.m[ 8] + u.m[11]*v.m[12];
  mat.m[ 9] = u.m[ 8]*v.m[ 1] + u.m[ 9]*v.m[ 5] + u.m[10]*v.m[ 9] + u.m[11]*v.m[13];
  mat.m[10] = u.m[ 8]*v.m[ 2] + u.m[ 9]*v.m[ 6] + u.m[10]*v.m[10] + u.m[11]*v.m[14];
  mat.m[11] = u.m[ 8]*v.m[ 3] + u.m[ 9]*v.m[ 7] + u.m[10]*v.m[11] + u.m[11]*v.m[15];

  mat.m[12] = u.m[12]*v.m[ 0] + u.m[13]*v.m[ 4] + u.m[14]*v.m[ 8] + u.m[15]*v.m[12];
  mat.m[13] = u.m[12]*v.m[ 1] + u.m[13]*v.m[ 5] + u.m[14]*v.m[ 9] + u.m[15]*v.m[13];
  mat.m[14] = u.m[12]*v.m[ 2] + u.m[13]*v.m[ 6] + u.m[14]*v.m[10] + u.m[15]*v.m[14];
  mat.m[15] = u.m[12]*v.m[ 3] + u.m[13]*v.m[ 7] + u.m[14]*v.m[11] + u.m[15]*v.m[15];
  return mat;
}

s_mat4 rotation_z_mat4(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat4 mat;
  mat.m[ 0] = c; mat.m[ 1] = -s; mat.m[ 2] = 0; mat.m[ 3] = 0;
  mat.m[ 4] = s; mat.m[ 5] =  c; mat.m[ 6] = 0; mat.m[ 7] = 0;
  mat.m[ 8] = 0; mat.m[ 9] =  0; mat.m[10] = 1; mat.m[11] = 0;
  mat.m[12] = 0; mat.m[13] =  0; mat.m[14] = 0; mat.m[15] = 1;
  return mat;
}

s_mat4 rotation_y_mat4(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat4 mat;
  mat.m[ 0] =  c; mat.m[ 1] =  0; mat.m[ 2] =  s; mat.m[ 3] =  0;
  mat.m[ 4] =  0; mat.m[ 5] =  1; mat.m[ 6] =  0; mat.m[ 7] =  0;
  mat.m[ 8] = -s; mat.m[ 9] =  0; mat.m[10] =  c; mat.m[11] =  0;
  mat.m[12] =  0; mat.m[13] =  0; mat.m[14] =  0; mat.m[15] =  1;
  return mat;
}

s_mat4 rotation_x_mat4(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat4 mat;
  mat.m[ 0] = 1; mat.m[ 1] =  0; mat.m[ 2] =  0; mat.m[ 3] =  0;
  mat.m[ 4] = 0; mat.m[ 5] =  c; mat.m[ 6] = -s; mat.m[ 7] =  0;
  mat.m[ 8] = 0; mat.m[ 9] =  s; mat.m[10] =  c; mat.m[11] =  0;
  mat.m[12] = 0; mat.m[13] =  0; mat.m[14] =  0; mat.m[15] =  1;
  return mat;
}

s_mat3 rotation_z_mat3(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat3 mat;
  mat.m[0] =  c; mat.m[1] = -s; mat.m[2] =  0;
  mat.m[3] =  s; mat.m[4] =  c; mat.m[5] =  0;
  mat.m[6] =  0; mat.m[7] =  0; mat.m[8] =  1;
  return mat;
}

s_mat3 rotation_y_mat3(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat3 mat;
  mat.m[0] =  c; mat.m[1] = 0; mat.m[2] = s;
  mat.m[3] =  0; mat.m[4] = 1; mat.m[5] = 0;
  mat.m[6] = -s; mat.m[7] = 0; mat.m[8] = c;
  return mat;
}

s_mat3 rotation_x_mat3(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  s_mat3 mat;
  mat.m[0] =  1; mat.m[1] =  0; mat.m[2] =  0;
  mat.m[3] =  0; mat.m[4] =  c; mat.m[5] = -s;
  mat.m[6] =  0; mat.m[7] =  s; mat.m[8] =  c;
  return mat;
}

s_mat4 translation_xyz_mat4(float x, float y, float z)
{
  s_mat4 mat;
  mat.m[ 0] = 1; mat.m[ 1] =  0; mat.m[ 2] = 0; mat.m[ 3] = 0;
  mat.m[ 4] = 0; mat.m[ 5] =  1; mat.m[ 6] = 0; mat.m[ 7] = 0;
  mat.m[ 8] = 0; mat.m[ 9] =  0; mat.m[10] = 1; mat.m[11] = 0;
  mat.m[12] = x; mat.m[13] =  y; mat.m[14] = z; mat.m[15] = 1;
  return mat;
}

s_mat4 scale_xyz_mat4(float x, float y, float z)
{
  s_mat4 mat;
  mat.m[ 0] = x; mat.m[ 1] =  0; mat.m[ 2] = 0; mat.m[ 3] = 0;
  mat.m[ 4] = 0; mat.m[ 5] =  y; mat.m[ 6] = 0; mat.m[ 7] = 0;
  mat.m[ 8] = 0; mat.m[ 9] =  0; mat.m[10] = z; mat.m[11] = 0;
  mat.m[12] = 0; mat.m[13] =  0; mat.m[14] = 0; mat.m[15] = 1;
  return mat;
}

void print_mat4(float *m)
{
  // print the matrix the normal way
  printf("%.4g %.4g %.4g %.4g\n", m[0], m[1], m[2], m[3]);
  printf("%.4g %.4g %.4g %.4g\n", m[4], m[5], m[6], m[7]);
  printf("%.4g %.4g %.4g %.4g\n", m[8], m[9], m[10], m[11]);
  printf("%.4g %.4g %.4g %.4g\n", m[12], m[13], m[14], m[15]);
  return;
}

void print_mat3(float *m)
{
  // print the matrix the normal way
  printf("%.4g %.4g %.4g\n", m[0], m[1], m[2]);
  printf("%.4g %.4g %.4g\n", m[3], m[4], m[5]);
  printf("%.4g %.4g %.4g\n", m[6], m[7], m[8]);
  return;
}

/*
mat4 LookAtRH(vec3 eye, vec3 target, vec3 up)
{
    vec3 zaxis = normalize(eye - target);    // The "forward" vector.
    vec3 xaxis = normalize(cross(up, zaxis));// The "right" vector.
    vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.

    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
    mat4 viewMatrix = {
        vec4(     xaxis.x,          yaxis.x,          zaxis.x,     0),
        vec4(     xaxis.y,          yaxis.y,          zaxis.y,     0),
        vec4(     xaxis.z,          yaxis.z,          zaxis.z,     0),
        vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
    };

    return viewMatrix;
}
*/
