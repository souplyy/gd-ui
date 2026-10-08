# GD UI: first component

Scope: the main-menu top bar only. Build the interface one component at a time.

- Full-width, 20-point charcoal surface (#16191F), flush to the screen's top edge.
- A 0.5-point lower divider (#2B313B).
- Single top-left settings control: 18-point target, center 16 points from the left.
- Lucide settings icon, pale 12-point glyph, with a lighter charcoal pressed surface (#303742).
- No vanilla glossy textures, outlines, labels, or additional toolbar content.
- Opens the existing GD settings; blocks taps on controls obscured by the bar.
- The enabled setting controls the entire bar after restart.

Native Cocos/Geode implementation. Width derives from the active logical window
size when MenuLayer initializes. In-game visual/touch verification remains pending.

Icon system: Lucide exclusively for future interface icons. Vendor original SVGs
and license under resources/icons; sample curves to native strokes with the
import script. No network is needed to render icons.

Live UI reads resources/ui.json defaults and polls the mod config override every
0.5 seconds. Apply validated complete files on the main thread, outside active
presses and popups. Keep the last valid UI after invalid edits. Native hook or
C++ changes require restarting GD.
