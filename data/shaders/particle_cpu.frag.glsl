#version 440 core

in GS_OUT {
    vec4 Color;
    vec4 SecData;
    float Pos;
} fs_in;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform sampler2D texture_diffuse;

const vec2 texCoord[4] = vec2[](vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0));

void main()
{
    if(fs_in.SecData.x == 0.0)
        discard;
	
    fragColor = fs_in.Color * texture(texture_diffuse, texCoord[int(fs_in.Pos)] * fs_in.SecData.zw);
    brightColor = vec4(fragColor.rgb * fs_in.SecData.y, 1.0);
}