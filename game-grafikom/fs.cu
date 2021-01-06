#version 330 core
in vec3 Color;

varying vec2 uv0; /* one way */
varying vec2 uv1; /* other way */

out vec4 FragColor;

uniform sampler2D texture1;

void main(){
    vec2 uvT;
    uvT.x = ( fwidth( uv0.x ) < fwidth( uv1.x )-0.001 )? uv0.x : uv1.x;
    uvT.y = ( fwidth( uv0.y ) < fwidth( uv1.y )-0.001 )? uv0.y : uv1.y;
	vec4 TexColor = texture2D(texture1, uvT);
    if(TexColor.a < 0.01) discard;
	FragColor = TexColor;
}

