#version 300 es
precision highp float;

out vec4 outputColor;

uniform vec4 u_color;


void main() {
    outputColor = (u_color.x == 0.0 && u_color.y == 0.0 && u_color.z == 0.0 && u_color.w == 0.0)? vec4(1.0, 0.0, 1.0, 1.0) : vec4(u_color);
}