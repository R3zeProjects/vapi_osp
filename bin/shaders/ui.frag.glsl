#version 450

layout(set = 0, binding = 0) uniform sampler2D uAtlas;

layout(location = 0)      in vec2  fragUV;
layout(location = 1)      in vec4  fragColor;
layout(location = 2) flat in float fragCornerRadius;
layout(location = 3) flat in float fragBorderWidth;
layout(location = 4)      in vec4  fragBorderColor;
layout(location = 5)      in vec2  fragLocalCoord;
layout(location = 6) flat in vec2  fragRectHalfSize;
layout(location = 7) flat in float fragSoftness;
layout(location = 8)      in vec2  fragRectSize;

layout(location = 0) out vec4 outColor;

float roundedBoxSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 q = abs(p) - halfSize + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// SDF rounded rectangle (PR path: uses rect size + UV 0..1)
float roundedRectSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 d = abs(p) - halfSize + vec2(radius);
    return length(max(d, vec2(0.0))) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    // === EXISTING PATH: text and flat rects (rectSize == 0) ===
    if (fragRectSize.x < 0.5 && fragRectSize.y < 0.5) {
        float texAlpha = texture(uAtlas, fragUV).r;
        outColor = vec4(fragColor.rgb, fragColor.a * texAlpha);
        return;
    }

    // Shadow: borderWidth < 0 signals shadow mode
    if (fragBorderWidth < 0.0) {
        float blur = -fragBorderWidth;
        vec2 halfSize = fragRectSize * 0.5;
        vec2 p = fragUV * fragRectSize - halfSize;
        float dist = roundedRectSDF(p, halfSize - vec2(blur), fragCornerRadius);
        float shadowAlpha = 1.0 - smoothstep(-blur, blur * 0.5, dist);
        outColor = vec4(fragColor.rgb, fragColor.a * shadowAlpha);
        return;
    }

    // === SDF path with rectSize: shadow (softness) or rounded rect ===
    if (fragSoftness > 0.5) {
        vec2 hs = fragRectHalfSize;
        float r = min(fragCornerRadius, min(hs.x, hs.y));
        float dist = roundedBoxSDF(fragLocalCoord, hs, r);
        float soft = max(fragSoftness, 1.0);
        float outerMask = 1.0 - smoothstep(-soft, soft * 0.5, dist);
        float shadow = outerMask * outerMask;
        outColor = vec4(fragColor.rgb, fragColor.a * shadow);
        return;
    }

    // === NEW PATH: SDF rounded rectangle (cornerRadius from rectSize + UV) ===
    vec2 localPos = fragUV * fragRectSize;
    vec2 halfSize = fragRectSize * 0.5;
    vec2 p = localPos - halfSize;

    float dist = roundedRectSDF(p, halfSize, fragCornerRadius);
    float edgeSmooth = 1.0;
    float alpha = 1.0 - smoothstep(-edgeSmooth, 0.0, dist);

    if (fragBorderWidth > 0.0) {
        float innerRadius = max(fragCornerRadius - fragBorderWidth, 0.0);
        vec2 innerHalf = halfSize - vec2(fragBorderWidth);
        float innerDist = roundedRectSDF(p, innerHalf, innerRadius);
        float innerAlpha = 1.0 - smoothstep(-edgeSmooth, 0.0, innerDist);
        alpha = alpha;
    }

    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
}
