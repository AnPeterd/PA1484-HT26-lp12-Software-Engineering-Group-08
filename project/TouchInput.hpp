
#pragma once

#include <lvgl.h>

enum InputEvent
{
    INPUT_NONE,
    SETTINGS_BUTTON,
    SAVE_BUTTON,
    RESET_BUTTON,
    REFRESH_BUTTON,
    STOP_DROPDOWN,
    TRANSPORT_DROPDOWN
};

// Connect Touch/Input to existing UI objects
void touch_input_init(
    lv_obj_t *btn_set,
    lv_obj_t *btn_save,
    lv_obj_t *btn_reset,
    lv_obj_t *btn_refresh,
    lv_obj_t *dropdown_stops,
    lv_obj_t *dropdown_transport
);

// Check if there is a new input
bool touch_input_has_event();

// Get latest input
InputEvent touch_input_get_event();

// Get selected stop
const char* touch_input_get_selected_stop();

// Get selected transport type
const char* touch_input_get_selected_transport();
