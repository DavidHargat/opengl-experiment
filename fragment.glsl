#version 330 core

// fragment.glsl
in vec2 frag_texcord;
in vec3 FragCord;

out vec4 color;

uniform sampler2D tex_data;

uniform float time;

void main(){

	//color = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 light = vec3(cos(time/100), 1.0, sin(time/100));
	float dist = 1.0/pow(distance(FragCord, light), 2);
	
	color = texture(tex_data, frag_texcord);
	
}
