# stm32-experiments

Bare-metal experiments on the STM32F103C8T6 "Blue Pill", using the STM32 HAL and PlatformIO without CubeIDE.

The current firmware blinks the onboard LED (PC13) every 250 ms, with the clock running at 72 MHz from the external crystal through the PLL. Each blink increments a counter that you can watch **live from VS Code while the chip keeps running**, with no breakpoints.

## What's here

### 1. A blink that wouldn't blink

The first build flashed fine, but the LED never moved. The cause was in the CMSIS startup file. `SysTick_Handler` falls back to `Default_Handler`, an infinite loop, unless the project defines its own. Without it, the first timer tick after `HAL_Init()` froze the CPU before `main()`'s loop ever ran. Defining `SysTick_Handler` to call `HAL_IncTick()` fixed it.

### 2. Live Watch in VS Code

This is the equivalent of STM32CubeIDE's "Live Expressions": the `blink_counter` variable updates in VS Code about 4 times a second while the CPU runs. Getting there took several separate fixes:

- PlatformIO rewrites its own debug settings and drops custom keys, so the project uses a separate `cortex-debug` launch configuration.
- PlatformIO's default GDB was version 8, and Live Watch needs 9 or newer, so the debug config points at a newer GDB.
- `build_type = debug` was added so the firmware includes the type information the debugger needs.
- The value has to be added in the **Cortex Live Watch** panel, not the normal Watch panel.

[LIVE_WATCH.md](LIVE_WATCH.md) has the full problem-by-problem write-up.

## Build, flash and debug

Requires [PlatformIO](https://platformio.org/) and an ST-Link. For Live Watch you also need the [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) VS Code extension.

```bash
pio run                 # build
pio run -t upload       # flash
```

To debug, choose **Cortex Debug (Live Watch)** in the Run & Debug panel, then add `blink_counter` under **Cortex Live Watch**.

The Blue Pill is a clone, so `platformio.ini` disables OpenOCD's CPUTAPID check.
