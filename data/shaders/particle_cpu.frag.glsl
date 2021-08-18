#version 440 core

in GS_OUT {
    vec4 Color;
    vec2 SecData;
    float Pos;
} fs_in;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

const vec2 texCoord[4] = vec2[](vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0));

void main()
{
    if(fs_in.SecData.x == 0.0 || fs_in.Color.a == 0.0)
        discard;
	
    fragColor = fs_in.Color;
    brightColor = vec4(fragColor.rgb * fs_in.SecData.y, 1.0);
}