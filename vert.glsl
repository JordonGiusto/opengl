#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform float u_time;
uniform mat4 u_mvp;
uniform vec3 u_offsets[100];

varying vec4 v_color;
varying vec2 v_texcoord;
varying vec4 v_pos;


void main() {
    gl_Position = v_pos = u_mvp * (position + vec4(u_offsets[gl_InstanceID], 0.0));//vec4(sin(u_time) * position.x, cos(u_time) * position.y, position.z, position.w);
    v_color = color;
}