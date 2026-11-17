/*
 * Copyright (c) 2026 Engin Al
 *
 * SPDX-License-Identifier: MIT
 *
 * Media key indicator: a short LED animation for media and screen-brightness keys, from whatever
 * key or layer sends them. The keyboard can't know whether music plays or how loud it is, so the
 * animations show what was pressed, by direction:
 *   next track      a soft band sweeps left to right
 *   previous track  a soft band sweeps right to left
 *   volume up       a band grows from the center outwards
 *   volume down     a band shrinks from the edges inwards
 *   play/pause      `pulse-leds` fade in and out once
 *   brightness +/-  like volume, in `brightness-color-hsb`
 * Drawn through zmk-rgb-led-overlay.
 */

#define DT_DRV_COMPAT zmk_rgb_media_indicator

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

#include <zmk_rgb_led_overlay/led_overlay.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

BUILD_ASSERT(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 1,
             "Exactly one zmk,rgb-media-indicator node is supported");

#define NODE DT_DRV_INST(0)
#define SLOT DT_PROP(NODE, slot)
#define DURATION_MS DT_PROP(NODE, duration_ms)

#define HSB(prop)                                                                                  \
    zmk_led_overlay_hsb(DT_PROP_BY_IDX(NODE, prop, 0), DT_PROP_BY_IDX(NODE, prop, 1),              \
                        DT_PROP_BY_IDX(NODE, prop, 2))

#if DT_NODE_HAS_PROP(NODE, pulse_leds)
static const uint8_t pulse_leds[] = DT_PROP(NODE, pulse_leds);
#endif

static uint32_t pulse_mask(void) {
    uint32_t mask = 0;
#if DT_NODE_HAS_PROP(NODE, pulse_leds)
    for (size_t i = 0; i < ARRAY_SIZE(pulse_leds); i++) {
        mask |= BIT(pulse_leds[i]);
    }
#endif
    return mask;
}

static void play(enum zmk_led_overlay_anim_type type, struct led_rgb color) {
    struct zmk_led_overlay_anim anim = {
        .type = type,
        .color = color,
        .duration_ms = DURATION_MS,
        .leds = type == ZMK_LED_OVERLAY_ANIM_PULSE ? pulse_mask() : 0,
    };

    zmk_led_overlay_animate(SLOT, &anim);
}

static int rgb_media_indicator_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);

    if (ev == NULL || !ev->state || ev->usage_page != HID_USAGE_CONSUMER) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    switch (ev->keycode) {
    case HID_USAGE_CONSUMER_SCAN_NEXT_TRACK:
        play(ZMK_LED_OVERLAY_ANIM_SWEEP_RIGHT, HSB(color_hsb));
        break;
    case HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK:
        play(ZMK_LED_OVERLAY_ANIM_SWEEP_LEFT, HSB(color_hsb));
        break;
    case HID_USAGE_CONSUMER_VOLUME_INCREMENT:
        play(ZMK_LED_OVERLAY_ANIM_RING_OUT, HSB(color_hsb));
        break;
    case HID_USAGE_CONSUMER_VOLUME_DECREMENT:
        play(ZMK_LED_OVERLAY_ANIM_RING_IN, HSB(color_hsb));
        break;
    case HID_USAGE_CONSUMER_PLAY_PAUSE:
    case HID_USAGE_CONSUMER_PLAY:
    case HID_USAGE_CONSUMER_PAUSE:
        play(ZMK_LED_OVERLAY_ANIM_PULSE, HSB(color_hsb));
        break;
    case HID_USAGE_CONSUMER_DISPLAY_BRIGHTNESS_INCREMENT:
        play(ZMK_LED_OVERLAY_ANIM_RING_OUT, HSB(brightness_color_hsb));
        break;
    case HID_USAGE_CONSUMER_DISPLAY_BRIGHTNESS_DECREMENT:
        play(ZMK_LED_OVERLAY_ANIM_RING_IN, HSB(brightness_color_hsb));
        break;
    default:
        break;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_media_indicator, rgb_media_indicator_listener);
ZMK_SUBSCRIPTION(rgb_media_indicator, zmk_keycode_state_changed);
