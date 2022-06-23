#version 300 es
precision highp float;

in vec3 v_color;

out vec4 outputColor;

// converts the color against the white background and the color against a
// black background to the K and S coefficients following the Kubelka-Munk model
// Computer-Generated watercolor 1997
//        void RGBtoKM(vec3 Rw, vec3 Rb, out vec3 K, out vec3 S)
//        {
//            vec3 a = 0.5 * (Rw + ((Rb - Rw + 1.0) / Rb));
//            vec3 b = sqrt(pow(a, 2.0) - 1.0);
//            vec3 temp1 = pow(b, 2.0) - (a - Rw) * (a - 1.0);
//            vec3 temp2 = b * (1.0 - Rw);
//
//            S = 1/b * coth(temp1 / temp2);
//
//            K = S * (a -1)
//        }

void main() {
    outputColor = vec4(v_color, 1.0);
}