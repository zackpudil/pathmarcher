#version 330 core

in vec2 tex;
layout (location = 0) out vec4 fragColor;

uniform sampler2D image;
uniform float frame;

void main() {
	vec3 col = vec3(0);

	col = texture(image, tex).xyz;
	col = pow(col, vec3(1.1/2.2));

	fragColor = vec4(col, 1);
}
