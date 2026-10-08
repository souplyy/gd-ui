# GD UI: first component

Scope: the main-menu top bar only. Build the interface one component at a time.

- Full-width, 44-point charcoal surface (#16191F), flush to the screen's top edge.
- A 1-point lower divider (#2B313B).
- Single top-left settings control: 34-point target, center 27 points from the left.
- Authored flat pale gear, with a lighter charcoal pressed surface (#303742).
- No vanilla glossy textures, outlines, labels, or additional toolbar content.
- Opens the existing GD settings; blocks taps on controls obscured by the bar.
- The enabled setting controls the entire bar after restart.

Native Cocos/Geode implementation. Width derives from the active logical window
size when MenuLayer initializes. In-game visual/touch verification remains pending.
