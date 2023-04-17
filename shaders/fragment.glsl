#version 450

in vec3 pos;
in vec3 nor;
in vec2 uv;
out vec4 color;

uniform sampler2D baseColor;

void main() {
    float shadow = dot(nor, normalize(vec3(10)));
    color = texture(baseColor, uv) * shadow;
}

