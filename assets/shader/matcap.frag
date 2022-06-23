#version 300 es
precision highp float;

in vec3 v_normal;

out vec4 outputColor;

uniform sampler2D u_matcap;
uniform mat4 u_view;


void main() {
    vec2 muv = vec2(u_view * vec4(normalize(vec3(-1.0, -1.0, -1.0)*v_normal), 0.0))*0.5 + vec2(0.5,0.5);
    outputColor = vec4(texture(u_matcap, muv).rgb, 1.0);
    //            outputColor = vec4(1.0);
}