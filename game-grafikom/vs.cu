#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

varying vec2 uv0; /* one way */
varying vec2 uv1; /* other way */

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0);
    uv0 = fract( aTexCoord.st );
    uv1 = fract( aTexCoord.st + vec2(0.5,0.5) ) - vec2(0.5,0.5); /* see paper */
	Color = aColor;
}

