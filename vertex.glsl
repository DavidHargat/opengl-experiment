#version 330 core

// vertex.glsl

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex;

uniform vec4 vx_color;

out vec4 frag_color;
out vec2 frag_texcord;

void main(){
	gl_Position  = vec4(position.x, position.y, position.z, 1.0);
	frag_color   = vec4(color.xyz, 1.0);
	frag_texcord = tex;
}
