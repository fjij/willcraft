#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float brightness;

uniform sampler2D tex;

void main() {
	FragColor = texture(tex, TexCoord) * vec4(brightness, brightness, brightness, 1.0f);
}
