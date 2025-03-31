#version 330 core

uniform vec4 FragColorIn;
out vec4 FragColor;

void main(){
	//				 r    g    b    a
	//FragColor = vec4(0.0, 0.7, 0.0, 1.0);
	FragColor = FragColorIn;
}