#pragma once

#include <ship/window/gui/GuiWindow.h>

namespace PaperboatGui {

class SunPositionEditor : public Ship::GuiWindow {
public:
    SunPositionEditor();
    ~SunPositionEditor() override;
    void Draw() override;

protected:
    void InitElement() override;
    void UpdateElement() override;
    void DrawElement() override;

private:
    void FinishDrag();
    void SetKeyboardNavigation(bool enabled);
    bool mAddedKeyboardNavigation = false;
    bool mDraggingSun = false;
    bool mPositionDirty = false;
    ImVec2 mDragOffset = { 0.0f, 0.0f };
};

} // namespace PaperboatGui
