#version 440 core

in GS_OUT {
    vec4 Color;
    vec2 SecData;
    vec4 UV;
    vec2 UVg;
} fs_in;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform sampler2D texture_diffuse;

void main()
{
    if(fs_in.SecData.x == 0.0)
        discard;
        
    vec2 texCoordRes = vec2(0.0, 0.0);
    texCoordRes.x = (fs_in.UV.x + fs_in.UVg.x) * fs_in.UV.z;
    texCoordRes.y = (fs_in.UV.y + fs_in.UVg.y) * fs_in.UV.w;
	
    fragColor = fs_in.Color * texture(texture_diffuse, texCoordRes);
    if (fragColor.a < 0.3)
        discard;
    brightColor = vec4(fragColor.rgb * fs_in.SecData.y, 1.0);
}