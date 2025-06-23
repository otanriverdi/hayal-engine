#version 330 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    FragColor = texColor * vertexColor;
}
