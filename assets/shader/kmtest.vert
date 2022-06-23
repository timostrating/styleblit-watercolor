#version 300 es
layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;
uniform mat4 normalMatrix;

out vec3 v_color;

void main() {
    vec4 n = normalMatrix*vec4(a_pos,0.0);
    v_color = n.xyz*0.5+vec3(0.5,0.5,0.5);
    gl_Position = MVP * vec4(a_pos, 1.0);
}