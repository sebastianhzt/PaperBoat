#include "ShaderTestLab.h"
#include <libultraship/libultraship.h>

namespace ShaderTestLab {

FakeSunState GetFakeSunState() {
    FakeSunState state;
    if (!CVarGetInteger(Enabled, 0)) {
        return state;
    }
    state.enabled = CVarGetInteger(FakeSunEnabled, 0) != 0;
    state.normalizedPosition.x = ClampFinite(CVarGetFloat(SunX, 0.0f), -1.0f, 1.0f, 0.0f);
    state.normalizedPosition.y = ClampFinite(CVarGetFloat(SunY, 0.5f), -1.0f, 1.0f, 0.5f);
    state.elevation = ClampFinite(CVarGetFloat(Elevation, 0.5f), 0.0f, 1.0f, 0.5f);
    state.intensity = ClampFinite(CVarGetFloat(Intensity, 1.0f), 0.0f, 4.0f, 1.0f);
    const auto color = CVarGetColor(ColorValue, (Color_RGBA8 { 255, 230, 153, 255 }));
    state.color[0] = color.r / 255.0f;
    state.color[1] = color.g / 255.0f;
    state.color[2] = color.b / 255.0f;
    return state;
}

void SetNormalizedPosition(Position2D position, bool persist) {
    if (!CVarGetInteger(Enabled, 0) || CVarGetInteger(Locked, 1)) {
        return;
    }
    CVarSetFloat(SunX, ClampFinite(position.x, -1.0f, 1.0f, 0.0f));
    CVarSetFloat(SunY, ClampFinite(position.y, -1.0f, 1.0f, 0.5f));
    if (persist) {
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
}

void ResetSun() {
    for (const auto* cvar : { SunX, SunY, Elevation, Intensity }) {
        CVarClear(cvar);
    }
    CVarClearBlock(Color);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void ResetToDefaults() {
    ResetSun();
    for (const auto* cvar : { Enabled, FakeSunEnabled, ShowEditor, Locked }) {
        CVarClear(cvar);
    }
}

} // namespace ShaderTestLab
