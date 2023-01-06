#version 450 core
//#extension GL_KHR_vulkan_glsl: enable

layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(set=1, binding=0) uniform sampler2D alphaTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; vec2 AlphaUV; } In;

void main()
{
    vec4 tex = texture(sTexture,In.UV.st);
    vec4 base = texture(alphaTexture,In.AlphaUV.st);
    vec4 top = vec4(tex.rgb,base.a*tex.a);


    fColor = top*In.Color;
}