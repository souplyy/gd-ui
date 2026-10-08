# GD UI top bar

15 logical units tall, charcoal #16191F, 0.5-unit divider #2B313B.
One 14-unit settings target, 6-unit left padding, 11-unit Lucide Settings icon.
Original Lucide SVG is parsed and rasterized in the game using NanoSVG at the
actual framebuffer pixel size. Pixel-align the glyph, use its native antialiased
coverage, and avoid filtering a larger texture down. Refresh on resolution
changes as well as UI/icon edits. Keep controls blocked while popups are open.

## UI switch

A single Geode OverlayManager control stays at the bottom right across scenes.
It is 108 by 32 units with an opaque dark fill, cyan border, and white
USE OG UI / USE NEW UI label naming the destination. It stores the chosen mode
and switches the top bar immediately. No per-frame hook or polling is used.
