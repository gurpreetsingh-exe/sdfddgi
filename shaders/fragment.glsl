#version 450

in vec3 pos;
in vec3 nor;
in vec2 uv;
out vec4 color;

void main() {
    color = vec4(uv, 0.0f, 1.0f);
}

