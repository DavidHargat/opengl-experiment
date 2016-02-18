#version 330 core

layout (location = 0) in vec3 position;

uniform vec4 vx_color;

out vec4 frag_color;

void main(){
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
	frag_color = vx_color;
}
