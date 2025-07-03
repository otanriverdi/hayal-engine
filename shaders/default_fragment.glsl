#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor; 

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    vec3 blendedColor = mix(texColor.rgb, uColor.rgb, uColor.a);
    FragColor = vec4(blendedColor, texColor.a);
}
