#version 300 es
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 MVP;
uniform mat4 normalMatrix;

out vec3 v_color;

void main() {
    vec3 n = 0.0 - (normalMatrix*vec4(a_normal,0.0)).xyz;
    v_color = n * 0.5 + 0.5;
    //            v_color = vec3(1.0, 0.0, 0.0);
    gl_Position = MVP * vec4(a_pos, 1.0);
}