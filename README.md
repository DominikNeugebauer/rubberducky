# rubberducky

STM32C071 USB HID keyboard (USBX, standalone). Currently types `BEANS ` 10x/s as a test.

## Layout

| Path | Role |
|------|------|
| `USBX/App/ux_device_keyboard.c` | Keyboard logic. `KEYBOARD_SendReport`, `KEYBOARD_TypeChar`, `KEYBOARD_TypeString`. `USER CODE 1` block. |
| `Core/Src/main.c` | USB startup (`USER CODE 2`) and main loop (`USER CODE 3`). |
| `USBX/App/app_usbx_device.c` | Registers the HID class, links callbacks. |
| `USBX/App/ux_device_descriptors.c` | USB descriptors + report descriptor. |
| `Core/Src/stm32c0xx_it.c` | USB interrupt handler. |
| `Core/Src/stm32c0xx_hal_msp.c` | USB clock + NVIC enable. |
| `Middlewares/` | ST USBX stack. Do not edit. |
| `rubberducky.ioc` | CubeMX project. |

Flow: `main()` -> `MX_USBX_Device_Init()` -> USB hardware start -> `while(1)`: `ux_system_tasks_run()` (sends queued reports) + type timer.

## Build & flash

```bash
cmake --preset Debug            # once, or after CubeMX changes
cmake --build build/Debug
arm-none-eabi-objcopy -O binary build/Debug/rubberducky.elf build/Debug/rubberducky.bin
st-flash write build/Debug/rubberducky.bin 0x08000000
```

## HID report

8 bytes: `[0]` modifiers, `[1]` 0, `[2..7]` up to 6 keycodes.
Modifiers: ctrl `0x01`, shift `0x02`, alt `0x04`, gui `0x08`.
One keypress = key report + empty report (release).

Keycodes: `a`–`z` = `0x04`–`0x1D`, `1`–`9` = `0x1E`–`0x26`, `0` = `0x27`, space `0x2C`, enter `0x28`, backspace `0x2A`, tab `0x2B`. Uppercase/symbols: same code + shift.

## After CubeMX regenerate

Re-apply these non-`USER CODE` defines:
- `USBX/App/app_usbx_device.h`: both memory sizes = `4096`
- `USBX/App/ux_device_descriptors.h`: `USBD_HID_KEYBOARD_EPIN_FS_MPS` = `8`
