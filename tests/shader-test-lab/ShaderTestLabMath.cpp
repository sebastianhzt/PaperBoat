#include "port/enhancements/graphics/ShaderTestLab.h"
#include <cstdlib>
#include <iostream>
#include <limits>

static void Check(bool condition) {
    if (!condition) {
        std::cerr << "Shader Test Lab math check failed\n";
        std::exit(EXIT_FAILURE);
    }
}

int main() {
    using namespace ShaderTestLab;
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto infinity = std::numeric_limits<float>::infinity();
    Check(!FakeSunState {}.enabled);
    Check(FakeSunState {}.intensity == 1.0f);
    Check(ClampFinite(nan, -1, 1, 0.5f) == 0.5f);
    Check(ClampFinite(infinity, -1, 1, 0) == 0);
    Check(ClampFinite(-3, -1, 1, 0) == -1);
    Check(ClampFinite(3, -1, 1, 0) == 1);

    for (float x : { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f }) {
        for (float y : { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f }) {
            const auto uv = NormalizedToScreenUV({ x, y });
            const auto restored = ScreenUVToNormalized(uv);
            Check(restored.x == x && restored.y == y);
            Check(IsOnScreen({ x, y }));
            // UI logical points, regardless of SSAA multiplier or window aspect.
            for (float width : { 240.0f, 320.0f, 640.0f }) {
                for (float scale : { 1.0f, 1.5f, 2.0f, 3.0f, 4.0f }) {
                    const float side = width * scale;
                    const float origin = 12.0f * scale;
                    const auto dragged = ScreenUVToNormalized(
                        { ((origin + uv.x * side) - origin) / side, ((origin + uv.y * side) - origin) / side }
                    );
                    Check(std::abs(dragged.x - x) < 0.00001f && std::abs(dragged.y - y) < 0.00001f);
                }
            }
        }
    }
    Check(NormalizedToScreenUV({ -1, 1 }).x == 0 && NormalizedToScreenUV({ -1, 1 }).y == 0);
    Check(ScreenUVToNormalized({ -4, 5 }).x == -1 && ScreenUVToNormalized({ -4, 5 }).y == -1);
    Check(!IsOnScreen({ 1.01f, 0 }) && !IsOnScreen({ 0, nan }));
    Check(ProjectClipToScreenUV(0, 0, 0.5f, 1, ClipDepthRange::ZeroToOne).visible);
    Check(ProjectClipToScreenUV(0, 0, -0.5f, 1, ClipDepthRange::MinusOneToOne).visible);
    Check(!ProjectClipToScreenUV(0, 0, -0.5f, 1, ClipDepthRange::ZeroToOne).visible);
    Check(!ProjectClipToScreenUV(0, 0, 0, -1, ClipDepthRange::ZeroToOne).visible);
    Check(!ProjectClipToScreenUV(0, 0, 0, 0, ClipDepthRange::ZeroToOne).visible);
    Check(!ProjectClipToScreenUV(2, 0, 0, 1, ClipDepthRange::ZeroToOne).visible);
    Check(!ProjectClipToScreenUV(0, 0, 2, 1, ClipDepthRange::ZeroToOne).visible);
    Check(!ProjectClipToScreenUV(nan, 0, 0, 1, ClipDepthRange::ZeroToOne).visible);
    std::cout << "Shader Test Lab math checks passed\n";
}
