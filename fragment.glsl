#version 330 core

// fragment.glsl
in vec2 frag_texcord;

out vec4 color;

uniform sampler2D tex_data;

void main(){

	//color = vec4(1.0, 1.0, 1.0, 1.0);
	color = texture(tex_data, frag_texcord);
	
}
