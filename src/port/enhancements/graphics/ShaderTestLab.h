#pragma once

#include <algorithm>
#include <cmath>
#include "port/ui/cvar_prefixes.h"

namespace ShaderTestLab {

inline constexpr const char* Enabled = CVAR_ENHANCEMENT("ShaderTestLab.Enabled");
inline constexpr const char* FakeSunEnabled = CVAR_ENHANCEMENT("ShaderTestLab.FakeSunEnabled");
inline constexpr const char* ShowEditor = CVAR_ENHANCEMENT("ShaderTestLab.ShowEditor");
inline constexpr const char* Locked = CVAR_ENHANCEMENT("ShaderTestLab.Locked");
inline constexpr const char* SunX = CVAR_ENHANCEMENT("ShaderTestLab.SunX");
inline constexpr const char* SunY = CVAR_ENHANCEMENT("ShaderTestLab.SunY");
inline constexpr const char* Elevation = CVAR_ENHANCEMENT("ShaderTestLab.Elevation");
inline constexpr const char* Intensity = CVAR_ENHANCEMENT("ShaderTestLab.Intensity");
inline constexpr const char* Color = CVAR_ENHANCEMENT("ShaderTestLab.Color");
inline constexpr const char* ColorValue = CVAR_ENHANCEMENT("ShaderTestLab.Color.Value");

struct Position2D {
    float x;
    float y;
};

struct FakeSunState {
    bool enabled = false;
    Position2D normalizedPosition = { 0.0f, 0.5f };
    // Future effect metadata, not a world-space height or camera-space depth.
    float elevation = 0.5f;
    float intensity = 1.0f;
    float color[3] = { 1.0f, 230.0f / 255.0f, 153.0f / 255.0f };
};

inline float ClampFinite(float value, float minimum, float maximum, float fallback) {
    return std::isfinite(value) ? std::clamp(value, minimum, maximum) : fallback;
}

// Screen UVs: top-left (0,0), bottom-right (1,1). No render-resolution multiplier.
inline Position2D NormalizedToScreenUV(Position2D position) {
    return { (position.x + 1.0f) * 0.5f, (1.0f - position.y) * 0.5f };
}

inline Position2D ScreenUVToNormalized(Position2D uv) {
    return { ClampFinite(uv.x * 2.0f - 1.0f, -1.0f, 1.0f, 0.0f), ClampFinite(1.0f - uv.y * 2.0f, -1.0f, 1.0f, 0.0f) };
}

inline bool IsOnScreen(Position2D position) {
    return std::isfinite(position.x) && std::isfinite(position.y) && position.x >= -1.0f && position.x <= 1.0f
        && position.y >= -1.0f && position.y <= 1.0f;
}

enum class ClipDepthRange {
    ZeroToOne,
    MinusOneToOne,
};

struct ScreenProjection {
    Position2D uv = { 0.0f, 0.0f };
    bool visible = false;
};

// Future world-space caller supplies view-projection * worldPosition, with positive
// W in front of the camera. Explicit depth convention avoids assuming a backend.
inline ScreenProjection ProjectClipToScreenUV(float x, float y, float z, float w, ClipDepthRange depthRange) {
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) || !std::isfinite(w) || w <= 0.00001f) {
        return {};
    }
    const Position2D normalized = { x / w, y / w };
    const float minimumZ = depthRange == ClipDepthRange::ZeroToOne ? 0.0f : -w;
    return { NormalizedToScreenUV(normalized), IsOnScreen(normalized) && z >= minimumZ && z <= w };
}

// On-demand API for future effects. No render hooks, camera writes or GPU resources.
FakeSunState GetFakeSunState();
void SetNormalizedPosition(Position2D position, bool persist = true);
void ResetSun();
void ResetToDefaults();

} // namespace ShaderTestLab
