
#include "TouchInput.hpp"
#include <Arduino.h>

static InputEvent current_event = INPUT_NONE;

static char selected_stop[64] = "";
static char selected_transport[32] = "";


// Reads touch coordinates and checks the button area
static void button_pressed(
    lv_event_t *e,
    InputEvent event,
    const char *event_name
)
{
    lv_obj_t *object = lv_event_get_target(e);
    lv_indev_t *touch = lv_indev_get_act();

    if (touch == NULL)
    {
        return;
    }

    lv_point_t point;
    lv_indev_get_point(touch, &point);

    lv_area_t area;
    lv_obj_get_coords(object, &area);

    if (point.x >= area.x1 &&
        point.x <= area.x2 &&
        point.y >= area.y1 &&
        point.y <= area.y2)
    {
        current_event = event;

        Serial.print("Touch X: ");
        Serial.print(point.x);
        Serial.print(" Y: ");
        Serial.print(point.y);
        Serial.print(" Input: ");
        Serial.println(event_name);
    }
}


// Button events
static void settings_pressed(lv_event_t *e)
{
    button_pressed(e, SETTINGS_BUTTON, "SETTINGS_BUTTON");
}

static void save_pressed(lv_event_t *e)
{
    button_pressed(e, SAVE_BUTTON, "SAVE_BUTTON");
}

static void reset_pressed(lv_event_t *e)
{
    button_pressed(e, RESET_BUTTON, "RESET_BUTTON");
}

static void refresh_pressed(lv_event_t *e)
{
    button_pressed(e, REFRESH_BUTTON, "REFRESH_BUTTON");
}


// Stop dropdown
static void stop_changed(lv_event_t *e)
{
    lv_obj_t *object = lv_event_get_target(e);

    current_event = STOP_DROPDOWN;

    lv_dropdown_get_selected_str(
        object,
        selected_stop,
        sizeof(selected_stop)
    );

    Serial.print("Input: STOP_DROPDOWN | Selected: ");
    Serial.println(selected_stop);
}


// Transport dropdown
static void transport_changed(lv_event_t *e)
{
    lv_obj_t *object = lv_event_get_target(e);

    current_event = TRANSPORT_DROPDOWN;

    lv_dropdown_get_selected_str(
        object,
        selected_transport,
        sizeof(selected_transport)
    );

    Serial.print("Input: TRANSPORT_DROPDOWN | Selected: ");
    Serial.println(selected_transport);
}


// Connect Touch/Input to existing UI objects
void touch_input_init(
    lv_obj_t *btn_set,
    lv_obj_t *btn_save,
    lv_obj_t *btn_reset,
    lv_obj_t *btn_refresh,
    lv_obj_t *dropdown_stops,
    lv_obj_t *dropdown_transport
)
{
    current_event = INPUT_NONE;

    if (btn_set != NULL)
    {
        lv_obj_add_event_cb(
            btn_set,
            settings_pressed,
            LV_EVENT_PRESSED,
            NULL
        );
    }

    if (btn_save != NULL)
    {
        lv_obj_add_event_cb(
            btn_save,
            save_pressed,
            LV_EVENT_PRESSED,
            NULL
        );
    }

    if (btn_reset != NULL)
    {
        lv_obj_add_event_cb(
            btn_reset,
            reset_pressed,
            LV_EVENT_PRESSED,
            NULL
        );
    }

    if (btn_refresh != NULL)
    {
        lv_obj_add_event_cb(
            btn_refresh,
            refresh_pressed,
            LV_EVENT_PRESSED,
            NULL
        );
    }

    if (dropdown_stops != NULL)
    {
        lv_obj_add_event_cb(
            dropdown_stops,
            stop_changed,
            LV_EVENT_VALUE_CHANGED,
            NULL
        );

        lv_dropdown_get_selected_str(
            dropdown_stops,
            selected_stop,
            sizeof(selected_stop)
        );
    }

    if (dropdown_transport != NULL)
    {
        lv_obj_add_event_cb(
            dropdown_transport,
            transport_changed,
            LV_EVENT_VALUE_CHANGED,
            NULL
        );

        lv_dropdown_get_selected_str(
            dropdown_transport,
            selected_transport,
            sizeof(selected_transport)
        );
    }

    Serial.println("Touch input ready");
}


// Check if there is a new input
bool touch_input_has_event()
{
    return current_event != INPUT_NONE;
}


// Get latest input
InputEvent touch_input_get_event()
{
    InputEvent event = current_event;
    current_event = INPUT_NONE;

    return event;
}


// Get selected stop
const char* touch_input_get_selected_stop()
{
    return selected_stop;
}


// Get selected transport type
const char* touch_input_get_selected_transport()
{
    return selected_transport;
}
