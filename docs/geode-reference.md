# Geode documentation baseline

Documentation source: https://github.com/geode-sdk/docs

Snapshot examined: `77f291629c7ba56a91233a75d843179c2f213465` (2026-10-08).
The source contains 76 Markdown pages. The generated class/function reference
is separate, built from the SDK's documented headers. A source inventory is in
[geode-pages.md](geode-pages.md); inventory coverage does not mean every page
has been read in full.

The starter uses the stable SDK **v5.10.1**, not the in-progress v6 APIs present
in the documentation's main branch. Its SDK declares GD **2.2081** on all four
platform families. The official example mod provides the same GD targets and
requires **geode.node-ids >=v1.23.3**.

## Guidance applied

- [Creating/building mods](https://docs.geode-sdk.org/getting-started/create-mod/):
  CMake, source directory, metadata, `.geode` packaging, platform builds.
- [Configuration](https://docs.geode-sdk.org/mods/configuring/): exact SDK/GD
  versions, stable mod identity, source link, dependencies, settings.
- [Hooks](https://docs.geode-sdk.org/tutorials/modify/): call the original
  initializer before extending it; avoid recursion and raw address hooks.
- [Nodes](https://docs.geode-sdk.org/tutorials/nodetree/) and
  [layouts](https://docs.geode-sdk.org/tutorials/layouts/): share existing menus,
  use node IDs rather than child indexes, refresh layout after insertion.
- [Buttons](https://docs.geode-sdk.org/tutorials/buttons/): buttons belong in
  a menu, lambda callbacks via `CCMenuItemExt`, scale the display sprite.
- [Popups](https://docs.geode-sdk.org/tutorials/popup/): use native `Popup`,
  allowing Geode to handle close controls, keyboard input, and touch priority.
- [Memory](https://docs.geode-sdk.org/tutorials/memory/): child ownership and
  standard `create`/`autorelease` lifecycle; delete after failed initialization.
- [Settings](https://docs.geode-sdk.org/mods/settings/): declare a bool setting
  and make the restart requirement explicit.
- [v5 migration](https://docs.geode-sdk.org/tutorials/migrate-v5/): C++23,
  non-templated `Popup::init`, new dependency syntax, typed setting listeners.

## Documentation caveats for future work

The older events/tasks/coroutine/settings tutorials include obsolete APIs.
Prefer the v5 migration guide and SDK headers when examples disagree. In v5,
async uses Arc futures; UI updates must return to the main thread. The v6
migration guide is explicitly work in progress and must not be mixed into this
stable v5 project.

For assets, Geode generates quality variants and namespaces resources; use
`_spr` for custom filenames. For persistent internal state, use saved values,
and put files in `Mod::get()->getSaveDir()` or `getConfigDir()`. Do not write
directly into GD's data folders for ordinary mod state.

Before any index submission, reread the current publishing guidelines. This
starter still needs meaningful finished features, a project logo, runtime
verification, and release preparation.
