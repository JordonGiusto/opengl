#version 330 core

layout(location = 0) out vec4 color;

varying vec4 v_color;
varying vec4 v_pos;

void main() {
    color = vec4(length(v_pos.xy), 0,0,1);
    color = v_color;
}