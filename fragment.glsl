#version 330 core

// fragment.glsl

in vec4 frag_color;
in vec2 frag_texcord;

out vec4 color;

uniform sampler2D tex_data;

void main(){

	//color = frag_color;
	//color = vec4(1.0, 1.0, 1.0, 1.0);
	color = frag_color;
}
