#version 330 core

// vertex.glsl

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex;

uniform vec4 vx_color;
uniform mat4 transform;

out vec4 frag_color;
out vec2 frag_texcord;

void main(){
	gl_Position  = transform * vec4(position.x, position.y, position.z, 1.0);
	frag_color   = transform * vec4(1.0, 1.0, 1.0, 1.0);
	frag_texcord = tex;
}
