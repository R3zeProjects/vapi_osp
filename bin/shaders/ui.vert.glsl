#version 450

layout(push_constant) uniform UIPush {
    float screenWidth;
    float screenHeight;
    float pad0;
    float pad1;
};

layout(location = 0) in vec2  inPosition;
layout(location = 1) in vec2  inUV;
layout(location = 2) in vec4  inColor;
layout(location = 3) in float inCornerRadius;
layout(location = 4) in float inBorderWidth;
layout(location = 5) in vec4  inBorderColor;
layout(location = 6) in vec2  inLocalCoord;
layout(location = 7) in vec2  inRectHalfSize;
layout(location = 8) in float inSoftness;
layout(location = 9) in vec2  inRectSize;

layout(location = 0)      out vec2  fragUV;
layout(location = 1)      out vec4  fragColor;
layout(location = 2) flat out float fragCornerRadius;
layout(location = 3) flat out float fragBorderWidth;
layout(location = 4)      out vec4  fragBorderColor;
layout(location = 5)      out vec2  fragLocalCoord;
layout(location = 6) flat out vec2  fragRectHalfSize;
layout(location = 7) flat out float fragSoftness;
layout(location = 8)      out vec2  fragRectSize;

void main() {
    float x = inPosition.x / screenWidth  * 2.0 - 1.0;
    float y = inPosition.y / screenHeight * 2.0 - 1.0;
    gl_Position = vec4(x, y, 0.0, 1.0);

    fragUV            = inUV;
    fragColor         = inColor;
    fragCornerRadius  = inCornerRadius;
    fragBorderWidth   = inBorderWidth;
    fragBorderColor   = inBorderColor;
    fragLocalCoord    = inLocalCoord;
    fragRectHalfSize  = inRectHalfSize;
    fragSoftness      = inSoftness;
    fragRectSize      = inRectSize;
}
