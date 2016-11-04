#version 330 core

in vec2 tex;
out vec4 fragColor;

uniform sampler2D image;
uniform float frame;

void main() {
	vec3 col = vec3(0);

  if(frame > 0.0) {
    col = texture(image, tex).xyz;
    col /= frame;
    col = pow(col, vec3(1.1/2.2));
  } 

	fragColor = vec4(col, 1);
}
