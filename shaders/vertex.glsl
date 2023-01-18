#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
out vec3 pos;
out vec3 nor;
out vec2 uv;

uniform mat4 modelViewProjection;
uniform mat4 transform;

void main() {
    gl_Position = modelViewProjection * transform * vec4(position, 1.0f);
    pos = position;
    nor = normal;
    uv = texCoord;
}

