# Shader Test Lab (experimental)

Open **Enhancements > Advanced Graphics > Shader Test Lab**. Enable the lab,
show the editor and unlock editing. Fake Sun is a separate opt-in flag for
future consumers; this version does not change scene lighting or draw a sun in
the game. The editor is a schematic, not a map or world-space camera.

Drag the sun marker to set X/Y in [-1, 1], with positive Y upward. Center sets
both coordinates to zero; Reset sun restores the sun parameters without hiding
the editor. Reset to defaults in the menu also disables the lab, fake sun and
editor, and restores the default editing lock. All settings persist as
`gEnhancements.ShaderTestLab.*` CVars (the standard color picker stores RGB in
`Color.Value`). Dragging updates memory live and requests persistence on release,
close or disable, rather than writing config every frame. No save-game data is modified.

The panel is anchored to the lower-right of the ImGui main viewport work area,
with scaled margins and constrained, user-adjustable size. It uses the same
logical screen coordinates for the mouse and grid, not the internal render
resolution. SSAA/MSAA/SMAA do not enter the conversion. Tiny windows may require
scrolling. Lock and Close are normal ImGui navigation items: use keyboard
navigation (Tab/Shift-Tab, Space/Enter), or the existing controller menu navigation
while the port menu is open. Keyboard navigation is enabled only while this editor
is focused and its previous disabled state is restored on hide/unfocus/destruction.
No new global shortcuts or input blockers are registered. Existing LUS
mouse mapping suppression applies inside the panel; dragging updates only
inside its grid and does not modify the game camera or player state. Hiding the
editor or disabling the lab submits no panel or input items.

## Shared effect integration

`ShaderTestLab::GetFakeSunState()` is an on-demand, backend-independent query.
When the lab is off it returns a disabled default state after one CVar read.
Consumers must honor `state.enabled`. Elevation is normalized metadata [0,1],
not a world height or linear depth; intensity is [0,4], and RGB is [0,1].
Non-finite/out-of-range numeric settings are sanitized at the query boundary.
There is no per-frame effect hook, GPU allocation or extra scene pass.

Keep these spaces separate:

- Widget points: `(mouse - gridOrigin) / gridSize` produces widget UV.
- Normalized fake sun: `ScreenUVToNormalized` clamps to [-1,1].
- Screen UV: `NormalizedToScreenUV` maps to top-left (0,0). It deliberately
  does not clamp, so `IsOnScreen` can reject future off-screen positions.
- Future world positions: obtain the active camera's correctly converted
  view-projection matrix without changing it. Supply homogeneous clip X/Y/Z/W
  to `ProjectClipToScreenUV`, explicitly choosing the matrix's actual depth
  convention (not merely the selected backend). Negative/near-zero W and
  off-frustum points are rejected. Convert the resulting UV to the renderer's
  actual scene viewport before sampling a texture. The schematic position is
  not automatically a world position.

The math API includes no DX11, OpenGL, ImGui or matrix-library types. The menu
only configures state; the editor draws ordinary ImGui primitives. Renderer
code, shaders and dependency/submodule sources are unchanged.

## Why a real top-down preview is deferred

`src/model.c::render_models` uses `gCameras[gCurrentCameraID]`, shared model
tables and render-task queues. Billboard rendering also reads the active camera.
A second call with a swapped global camera is not a safely isolated static-map
pass. It would need explicit ownership/restoration of task queues, viewport,
targets, matrices and depth, plus static-only selection and backend resource
lifecycle handling. The MVP creates no framebuffer or secondary camera.

The next renderer contribution should define a preview provider that accepts
an independent camera and static geometry list, returns a backend-owned small
texture plus status/timing, and explicitly releases it on disable/device reset.
Only expose a real-map option once such a provider exists; unavailable providers
must retain the schematic fallback. The current shared sun/projection API is
usable without that future provider. No placeholder GPU resources are created.

## Validation

The portable, dependency-free math checks can be built separately:

```sh
cmake -S tests/shader-test-lab -B build/shader-test-lab-tests
cmake --build build/shader-test-lab-tests
ctest --test-dir build/shader-test-lab-tests --output-on-failure
```

Use the normal Windows build documented in BUILDING.md for the application.
Windows includes both DX11 and OpenGL backend code. Formatting uses the pinned
clang-format 21.1.8 and the project's run-clang-format script.

Manual acceptance checklist (not implied by math/build success):

- Fresh config: normal startup, lab/editor off, unchanged visuals.
- DX11 and OpenGL: repeatedly open/close, lock/unlock, drag; mouse mappings
  do not move Mario/camera inside the panel and work normally outside it.
- Keyboard/controller navigation can lock/close; hiding leaves gameplay input
  unchanged. Controller navigation follows the existing port-menu behavior.
- Resize, fullscreen, 4:3/16:9/ultrawide, DPI/UI scale, internal resolution
  multipliers x1/x2/x3/x4 where supported, and available antialiasing modes.
- Map changes, load/save and cutscenes with the editor active; no unintended
  input. Restart confirms persistence and reset defaults.
- Disable during interaction: no panel/resources remain active.
- macOS/Linux CI and controller hardware checks remain necessary.
