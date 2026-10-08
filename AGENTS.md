# GD UI development

- Always host any previews locally. Never use ChatGPT Sites.
- Develop one interface component at a time, as requested by the user.
- Use Lucide for all new UI icons. Keep official SVG source and license under
  resources/icons. Render the original SVG at the actual framebuffer pixel size
  using the native NanoSVG renderer. Never approximate icons or shrink a fixed
  large texture. Pixel-align the icon and retain its native antialiased coverage.
- Prefer resources/ui.json for supported style, layout, and icon edits. Publish
  live changes with scripts/publish-ui.py; keep its watch process active during
  local UI iteration. Native C++ and hook changes still require restarting GD.
- Build and install updated native packages into the local Geode mods folder;
  distinguish compilation checks from confirmed in-game testing.
