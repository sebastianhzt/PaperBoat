#include "PaperboatMenu.h"
#include "port/enhancements/graphics/ShaderTestLab.h"

namespace PaperboatGui {

extern std::shared_ptr<PaperboatMenu> mPaperboatMenu;

using namespace UIWidgets;

static const std::unordered_map<int32_t, const char*> blockWindowOptions = {
    { 0, "Original (3 frames)" },
    { 1, "Forgiving (7 frames)" },
    { 2, "Very Forgiving (10 frames)" },
};

static const std::unordered_map<int32_t, const char*> actionCommandDifficultyOptions = {
    { 0, "Original" },
    { 1, "Forgiving (-1 level)" },
    { 2, "Very Forgiving (-2 levels)" },
    { 3, "Extremely Forgiving (-3 levels)" },
};

void PaperboatMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", CVAR_SETTING("Menu.EnhancementsSidebarSection"));

    // Enhancements > Cheats
    WidgetPath path = { "Enhancements", "Cheats", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Infinite Health", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("InfiniteHealth"))
        .Options(CheckboxOptions().Tooltip("Mario's HP won't decrease during battle."));

    AddWidget(path, "Infinite Flower Points", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("InfiniteFlowerPoints"))
        .Options(CheckboxOptions().Tooltip("Mario's FP won't decrease during battle."));

    AddWidget(path, "No Badge Cost", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoBPCost"))
        .Options(CheckboxOptions().Tooltip("Equip any badge regardless of BP cost."));

    AddWidget(path, "Max Star Power", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("MaxStarPower"))
        .Options(CheckboxOptions().Tooltip("Star Power stays full and won't decrease."));

    // Enhancements > Gameplay
    path = { "Enhancements", "Gameplay", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 2);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "DX: Prevent Loading Zone Storage", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PreventLoadingZoneStorage"))
        .Options(
            CheckboxOptions().Tooltip(
                "Locks out player input the moment a loading zone is triggered, which patches "
                "out the Loading Zone Storage glitch. Off by default to match the original "
                "game. Note that most loading zones also trigger while you are airborne above "
                "them, so enabling this can freeze Mario in midair until he lands."
            )
        );

    AddWidget(path, "Sprint Button", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SprintButton"))
        .Options(CheckboxOptions().Tooltip("Hold R to move at double speed in the overworld."));

    AddWidget(path, "Action Command Difficulty", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("ActionCommandDifficulty"))
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Lowers the difficulty of attack action commands, which widens their input "
                    "windows. Stacks with the Dodge Master badge."
                )
                .ComboMap(actionCommandDifficultyOptions)
        );

    AddWidget(path, "Block Window", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("BlockWindowMode"))
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Sets the window for timed defensive blocks. The default is 3 frames. Anything "
                    "wider overrides the Dodge Master badge."
                )
                .ComboMap(blockWindowOptions)
        );

    // Enhancements > Graphics
    path = { "Enhancements", "Graphics", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Graphics", 1);

    AddWidget(path, "Full Height View", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Graphics.FullHeightView"))
        .Options(
            CheckboxOptions().Tooltip("Removes the letterboxing bars on the top and bottom of the screen in gameplay.")
        );

    AddWidget(path, "Rounded Projector Reel", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Graphics.RoundedReel"))
        .Options(
            CheckboxOptions().Tooltip("Flips the battle projector reel to appear rounded for widescreen resolutions.")
        );

    path = { "Enhancements", "Advanced Graphics", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Shader Test Lab (Experimental)", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Configuration only; no god rays or lighting changes.", WIDGET_TEXT);
    AddWidget(path, "Enable Shader Test Lab", WIDGET_CVAR_CHECKBOX).CVar(ShaderTestLab::Enabled);

    auto requireLab = [](WidgetInfo& info) {
        info.options->disabled = !CVarGetInteger(ShaderTestLab::Enabled, 0);
        info.options->disabledTooltip = "Enable Shader Test Lab first.";
    };
    auto requireEditing = [](WidgetInfo& info) {
        info.options->disabled = !CVarGetInteger(ShaderTestLab::Enabled, 0) || CVarGetInteger(ShaderTestLab::Locked, 1);
        info.options->disabledTooltip = "Enable Shader Test Lab and unlock editing first.";
    };
    AddWidget(path, "Enable Fake Sun", WIDGET_CVAR_CHECKBOX).CVar(ShaderTestLab::FakeSunEnabled).PreFunc(requireLab);
    AddWidget(path, "Show Sun Position Editor", WIDGET_CVAR_CHECKBOX)
        .CVar(ShaderTestLab::ShowEditor)
        .PreFunc(requireLab);
    AddWidget(path, "Lock editing", WIDGET_CVAR_CHECKBOX)
        .CVar(ShaderTestLab::Locked)
        .Options(CheckboxOptions().DefaultValue(true))
        .PreFunc(requireLab);
    AddWidget(path, "Sun X", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(ShaderTestLab::SunX)
        .Options(FloatSliderOptions().Min(-1.0f).Max(1.0f).DefaultValue(0.0f).Format("%.3f"))
        .PreFunc(requireEditing);
    AddWidget(path, "Sun Y", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(ShaderTestLab::SunY)
        .Options(FloatSliderOptions().Min(-1.0f).Max(1.0f).DefaultValue(0.5f).Format("%.3f"))
        .PreFunc(requireEditing);
    AddWidget(path, "Sun Elevation", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(ShaderTestLab::Elevation)
        .Options(
            FloatSliderOptions().Min(0.0f).Max(1.0f).DefaultValue(0.5f).Format("%.3f").Tooltip(
                "Normalized metadata for future effects, not a world-space height."
            )
        )
        .PreFunc(requireEditing);
    AddWidget(path, "Sun Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(ShaderTestLab::Intensity)
        .Options(FloatSliderOptions().Min(0.0f).Max(4.0f).DefaultValue(1.0f).Format("%.2f"))
        .PreFunc(requireEditing);
    // The generic color-picker widget does not forward its disabled option.
    AddWidget(path, "Sun Color", WIDGET_CUSTOM)
        .CVar(ShaderTestLab::Color)
        .PreFunc(requireEditing)
        .CustomFunction([](WidgetInfo& info) {
            ImGui::BeginDisabled(info.options->disabled);
            CVarColorPicker(
                info.name.c_str(), info.cVar, Color_RGBA8 { 255, 230, 153, 255 }, false, ColorPickerResetButton,
                mPaperboatMenu->GetMenuThemeColor()
            );
            ImGui::EndDisabled();
        });
    AddWidget(path, "Normalized sun coordinates", WIDGET_CUSTOM).CustomFunction([](WidgetInfo&) {
        const auto sun = ShaderTestLab::GetFakeSunState();
        ImGui::Text("Normalized: X %.3f / Y %.3f", sun.normalizedPosition.x, sun.normalizedPosition.y);
    });
    AddWidget(path, "Reset to defaults", WIDGET_BUTTON)
        .Callback([](WidgetInfo&) { ShaderTestLab::ResetToDefaults(); })
        .PreFunc(requireEditing);
}

} // namespace PaperboatGui
