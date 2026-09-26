#include "SunPositionEditor.h"
#include "port/enhancements/graphics/ShaderTestLab.h"
#include <libultraship/libultraship.h>
#include <algorithm>
#include <cmath>

namespace PaperboatGui {

SunPositionEditor::SunPositionEditor()
    : GuiWindow("", "Sun Position Editor") {
}

SunPositionEditor::~SunPositionEditor() {
    SetKeyboardNavigation(false);
}

void SunPositionEditor::SetKeyboardNavigation(bool enabled) {
    if (ImGui::GetCurrentContext() == nullptr) {
        return;
    }
    auto& flags = ImGui::GetIO().ConfigFlags;
    if (enabled && !(flags & ImGuiConfigFlags_NavEnableKeyboard)) {
        flags |= ImGuiConfigFlags_NavEnableKeyboard;
        mAddedKeyboardNavigation = true;
    } else if (!enabled && mAddedKeyboardNavigation) {
        flags &= ~ImGuiConfigFlags_NavEnableKeyboard;
        mAddedKeyboardNavigation = false;
    }
}

void SunPositionEditor::InitElement() {
}

void SunPositionEditor::UpdateElement() {
}

void SunPositionEditor::FinishDrag() {
    mDraggingSun = false;
    if (mPositionDirty) {
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        mPositionDirty = false;
    }
}

void SunPositionEditor::Draw() {
    if (!CVarGetInteger(ShaderTestLab::Enabled, 0) || !CVarGetInteger(ShaderTestLab::ShowEditor, 0)) {
        FinishDrag();
        SetKeyboardNavigation(false);
        return;
    }

    const auto* viewport = ImGui::GetMainViewport();
    const float scale = ImGui::GetFontSize() / 13.0f;
    const float margin = 12.0f * scale;
    const ImVec2 maximum(
        (std::max) (1.0f, viewport->WorkSize.x - 2.0f * margin), (std::max) (1.0f, viewport->WorkSize.y - 2.0f * margin)
    );
    ImGui::SetNextWindowPos(
        ImVec2(
            viewport->WorkPos.x + viewport->WorkSize.x - margin, viewport->WorkPos.y + viewport->WorkSize.y - margin
        ),
        ImGuiCond_Always, ImVec2(1.0f, 1.0f)
    );
    ImGui::SetNextWindowSize(
        ImVec2((std::min) (290.0f * scale, maximum.x), (std::min) (355.0f * scale, maximum.y)), ImGuiCond_FirstUseEver
    );
    ImGui::SetNextWindowSizeConstraints(
        ImVec2((std::min) (240.0f * scale, maximum.x), (std::min) (280.0f * scale, maximum.y)), maximum
    );
    ImGui::SetNextWindowBgAlpha(0.85f);

    bool open = true;
    if (ImGui::Begin(
            "Sun Position Editor", &open,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings
        ))
    {
        // LUS does not enable keyboard navigation by default. Scope our opt-in
        // to this focused editor, and leave pre-existing navigation flags alone.
        SetKeyboardNavigation(ImGui::IsWindowFocused());
        DrawElement();
    } else {
        FinishDrag();
        SetKeyboardNavigation(false);
    }
    ImGui::End();
    if (!open) {
        CVarSetInteger(ShaderTestLab::ShowEditor, 0);
        FinishDrag();
        SetKeyboardNavigation(false);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
}

void SunPositionEditor::DrawElement() {
    auto state = ShaderTestLab::GetFakeSunState();
    const float scale = ImGui::GetFontSize() / 13.0f;
    ImGui::TextUnformatted("Experimental / schematic only");
    ImGui::TextUnformatted(state.enabled ? "Fake sun enabled (no lighting effect)" : "Fake sun disabled");

    bool locked = CVarGetInteger(ShaderTestLab::Locked, 1) != 0;
    if (ImGui::Checkbox("Lock editing", &locked)) {
        CVarSetInteger(ShaderTestLab::Locked, locked);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    const auto available = ImGui::GetContentRegionAvail();
    const float side = (std::max) (1.0f, (std::min) (available.x, available.y - 75.0f * scale));
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 end(origin.x + side, origin.y + side);
    auto uv = ShaderTestLab::NormalizedToScreenUV(state.normalizedPosition);
    const ImVec2 marker(origin.x + uv.x * side, origin.y + uv.y * side);
    const float radius = 7.0f * scale;

    ImGui::InvisibleButton("##SunGrid", ImVec2(side, side));
    const auto mouse = ImGui::GetIO().MousePos;
    const bool inside = mouse.x >= origin.x && mouse.x <= end.x && mouse.y >= origin.y && mouse.y <= end.y;
    if (!locked && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const float dx = mouse.x - marker.x;
        const float dy = mouse.y - marker.y;
        mDraggingSun = dx * dx + dy * dy <= radius * radius * 4.0f;
        mDragOffset = ImVec2(dx / side, dy / side);
    }
    // Stop at the panel boundary; never steal mouse mappings outside the window.
    // Include the release frame, so a quick drag between frames keeps its final position.
    if (mDraggingSun && !locked && (ImGui::IsItemActive() || ImGui::IsItemDeactivated()) && inside) {
        ShaderTestLab::SetNormalizedPosition(
            ShaderTestLab::ScreenUVToNormalized(
                { (mouse.x - origin.x) / side - mDragOffset.x, (mouse.y - origin.y) / side - mDragOffset.y }
            ),
            false
        );
        mPositionDirty = true;
        state = ShaderTestLab::GetFakeSunState();
        uv = ShaderTestLab::NormalizedToScreenUV(state.normalizedPosition);
    }
    if (locked || !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        FinishDrag();
    }

    auto* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(origin, end, IM_COL32(18, 25, 35, 210));
    for (int i = 0; i <= 4; i++) {
        const float offset = side * i / 4.0f;
        draw->AddLine(
            ImVec2(origin.x + offset, origin.y), ImVec2(origin.x + offset, end.y),
            IM_COL32(95, 110, 130, i == 2 ? 220 : 90)
        );
        draw->AddLine(
            ImVec2(origin.x, origin.y + offset), ImVec2(end.x, origin.y + offset),
            IM_COL32(95, 110, 130, i == 2 ? 220 : 90)
        );
    }
    const ImVec2 center(origin.x + side * 0.5f, origin.y + side * 0.5f);
    draw->AddCircle(center, 3.0f * scale, IM_COL32(220, 220, 220, 220));
    const ImVec2 sun(origin.x + uv.x * side, origin.y + uv.y * side);
    const auto color = ImGui::ColorConvertFloat4ToU32(ImVec4(state.color[0], state.color[1], state.color[2], 1.0f));
    draw->AddCircleFilled(sun, radius, color);
    draw->AddCircle(sun, radius + 2.0f * scale, locked ? IM_COL32(160, 160, 160, 255) : IM_COL32_WHITE);
    ImGui::Text("X: %.3f   Y: %.3f", state.normalizedPosition.x, state.normalizedPosition.y);
    ImGui::BeginDisabled(locked);
    if (ImGui::Button("Center")) {
        ShaderTestLab::SetNormalizedPosition({ 0.0f, 0.0f });
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset sun")) {
        ShaderTestLab::ResetSun();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        CVarSetInteger(ShaderTestLab::ShowEditor, 0);
        FinishDrag();
        SetKeyboardNavigation(false);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
}

} // namespace PaperboatGui
