#version 440 core

in vec2 TextureCoordinates;

uniform sampler2D hdrBuffer;
uniform float exposure;

out vec4 fragColor;

void main()
{
    vec3 color = texture(hdrBuffer, TextureCoordinates).rgb;
    vec3 result = vec3(1.0) - exp(-color * exposure);

    // Expand it
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}