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

## Main menu

Mode: Operate. Center the equipped cube in a charcoal circular medallion.
Surround it with five contiguous 72-degree annular sectors, flat dark slate
fills and thin slate outlines. Counterclockwise from the top: Play, Icons,
Stats, Achievements, Browse. Use Lucide SVG glyphs and horizontal labels.
Each sector activates the existing native menu item; hide only the five original
buttons in new mode, restore their original visibility in OG mode.
Touch areas follow the annulus precisely, cancel when dragging out, and defer
to modal dialogs. No frame hook or scheduled radial-menu callback.

## Profile dropdown

The top-right 86-unit trigger shows the equipped cube, player name, and Lucide
chevron inside the existing 15-unit bar. A 132 by 88-unit charcoal panel drops
below it, with a name header and View Profile, Icon Kit, and Account actions.
Use the native GD destinations. Outside clicks dismiss and are consumed.
Modal dialogs take priority; live top-bar rebuilds wait while the dropdown or
a profile touch is active. Reference: local GD Lazer account-card interaction,
with original compact styling and implementation for GD UI.

Profile controls use 4-unit corner radii; the dropdown has an 8-unit radius.
Open with a 10-unit downward slide over 140 ms using exponential ease-out;
close upward over 90 ms with sine ease-in. Cancel old actions before toggling,
disable rows while closing, and pause live rebuilds through the closing action.
