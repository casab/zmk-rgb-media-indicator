# zmk-rgb-media-indicator

A [ZMK](https://zmk.dev) module that plays a short LED animation when a media or screen-brightness
key is pressed, from any key or layer that sends it. No keymap changes are needed: it watches the
keycodes themselves.

The keyboard can't know whether music is playing or how loud it is, so the animations show what
you pressed, by direction:

| Key | Animation |
| --- | --- |
| Next track | a soft band sweeps left to right |
| Previous track | a soft band sweeps right to left |
| Volume up | a band grows from the center outwards |
| Volume down | a band shrinks from the edges inwards |
| Play/pause (and play, pause) | `pulse-leds` fade in and out once |
| Screen brightness up / down | like volume up / down, in `brightness-color-hsb` |

Pressing again restarts the animation, so quick presses never lag behind. It's drawn on top of
the normal underglow and needs [zmk-rgb-led-overlay](https://github.com/casab/zmk-rgb-led-overlay);
give that module `led-x` so the motion follows the LEDs' real positions.

## Add it to your zmk-config

1. `config/west.yml`: add `zmk-rgb-led-overlay` and this module.

   ```yaml
       - name: zmk-rgb-led-overlay
         remote: casab          # url-base: https://github.com/casab
         revision: main
       - name: zmk-rgb-media-indicator
         remote: casab
         revision: main
   ```

2. Set up zmk-rgb-led-overlay (see its README), then add to your keymap:

   ```dts
   / {
       rgb_media_indicator {
           compatible = "zmk,rgb-media-indicator";
           color-hsb = <175 100 50>;             // teal
           brightness-color-hsb = <35 100 50>;   // amber
           pulse-leds = <10>;                    // the center LED
       };
   };
   ```

## Properties

| Property | Type | Default | Meaning |
| --- | --- | --- | --- |
| `color-hsb` | array | `<175 100 50>` | Media key color (teal). |
| `brightness-color-hsb` | array | `<35 100 50>` | Screen-brightness key color (amber). |
| `pulse-leds` | array | none | LEDs that pulse for play/pause. |
| `duration-ms` | int | 350 | Length of each animation. |
| `slot` | int | 4 | zmk-rgb-led-overlay slot; higher slots are drawn over lower ones. |

## License

MIT
