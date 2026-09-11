# Devlog

## 2026-09-11 — PC13 blink working

**What was built:** Basic STM32F103C8T6 (Blue Pill) firmware toggling the onboard PC13 LED every 250ms, using the `stm32cube` HAL framework via PlatformIO.

**What failed:** Initial flash reported SUCCESS and the chip reset normally, but the LED never blinked — it stayed in its initial off state.

**What fixed it:** Traced the hang to the CMSIS startup file (`startup_stm32f103xb.s`), which weak-aliases `SysTick_Handler` to `Default_Handler` (an infinite loop). The project had no `stm32f1xx_it.c`, so nothing overrode `SysTick_Handler`. As soon as `HAL_Init()` enabled the SysTick interrupt, the first tick trapped the CPU in that infinite loop before the main blink loop could ever run. Fixed by adding a `SysTick_Handler` in `src/main.c` that calls `HAL_IncTick()`. Rebuilt with `pio run` → SUCCESS. The rest of the code (HAL_Init/SystemClock_Config ordering, GPIOC clock enable, active-low PC13 logic, HSE+PLL clock config) was already correct.

**What is next:** Reflash and visually confirm the LED blinks at the expected ~500ms on/off cycle; then move on to the next experiment (TBD).

## 2026-09-11 — Live Watch (Cortex-Debug) working

**What was built:** Real-time variable watching for `blink_counter` (a `volatile uint32_t` global incremented on every LED toggle in `src/main.c`), using Cortex-Debug's Live Watch — polls SRAM while the CPU runs, no breakpoint needed. Added a standalone `"type": "cortex-debug"` config in `.vscode/launch.json` (name "Cortex Debug (Live Watch)") so it survives PlatformIO's automatic `launch.json` regeneration.

**What failed, in order:**
1. `liveWatch` key added to the `platformio-debug` configs kept getting silently stripped on regeneration, and turned out to do nothing anyway — PlatformIO's own debug adapter doesn't implement Live Watch at all (that's a `marus25.cortex-debug`-extension-only feature, which wasn't installed).
2. After installing the extension and adding a proper `cortex-debug` config, the session flashed a `1` badge and died instantly with no output — extension wasn't activated yet.
3. After a window reload, got as far as a VS Code error dialog: `GDB could not start... Must be >= 9` — the config was pointing at a toolchain with GDB 8.0 (2017).
4. After repointing to a newer installed toolchain (GDB 13.2), GDB connected but `print blink_counter` returned "unknown type" — the build had no `-g`/DWARF debug info at all.
5. After fixing the build, the user added `blink_counter` to the plain **Watch** panel and saw "not available" — turned out to be the wrong panel (plain Watch only evaluates while halted); the **Cortex Live Watch** panel was the right one all along.

**What fixed it:**
- New `cortex-debug`-typed config in `launch.json` (immune to PlatformIO's regeneration, unlike edits to its own `platformio-debug` configs).
- `armToolchainPath`/`gdbPath` repointed to the unversioned `toolchain-gccarmnoneeabi` package (GDB 13.2) instead of the versioned one used for building (GDB 8.0).
- Added `build_type = debug` to `platformio.ini` so the ELF carries full DWARF info.
- Used the **Cortex Live Watch** panel, not plain Watch, to add `blink_counter`.
- Full step-by-step writeup: [LIVE_WATCH.md](LIVE_WATCH.md).

**What is next:** Add more live-watched variables as needed for future experiments (e.g. timer/ADC values); consider whether `build_type = debug` should be swapped back for release-size measurements before any final flash.
