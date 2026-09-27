#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <time.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>

#include "secrets.h"




// Data structures to hold departure information
// DepartureItem: Lagrar info om en enskild avgång (tid, linje, destination, status).
struct DepartureItem {
    char time[16];
    char line[16];
    char destination[64];
    char status[32];
};


// StopData: Lagrar hållplatsens namn och en lista med upp till 5 avgångar.
struct StopData {
    char stopName[64];
    DepartureItem departures[5];
    int count;
};


static lv_obj_t* tileview;
static lv_obj_t* t_start;
static lv_obj_t* t_departures;
static lv_obj_t* t_settings;

static lv_obj_t *dep_list_container = NULL;
static lv_obj_t *dropdown_stops = NULL;
static lv_obj_t *dropdown_transport = NULL;


LilyGo_Class amoled;


// Lärarens kod
/*
static lv_obj_t* tileview;
static lv_obj_t* t1;
static lv_obj_t* t2;
static lv_obj_t* t1_label;
static lv_obj_t* t2_label;
static bool t2_dark = false;  // start tile #2 in light mode

// Function: Tile #2 Color change
static void apply_tile_colors(lv_obj_t* tile, lv_obj_t* label, bool dark)
{
  // Background
  lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(tile, dark ? lv_color_black() : lv_color_white(), 0);

  // Text
  lv_obj_set_style_text_color(label, dark ? lv_color_white() : lv_color_black(), 0);
}

static void on_tile2_clicked(lv_event_t* e)
{
  LV_UNUSED(e);
  t2_dark = !t2_dark;
  apply_tile_colors(t2, t2_label, t2_dark);
}

// Function: Creates UI
static void create_ui()
{
  // Fullscreen Tileview
  tileview = lv_tileview_create(lv_scr_act());
  lv_obj_set_size(tileview, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

  // Add two horizontal tiles
  t1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR);
  t2 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
*/
  // // Tile #1, Screen1
  // {
  //   t1_label = lv_label_create(t1);
  //   lv_label_set_text(t1_label, "Hello Students");
  //   lv_obj_set_style_text_font(t1_label, &lv_font_montserrat_28, 0);
  //   lv_obj_center(t1_label);
//     apply_tile_colors(t1, t1_label, /*dark=*/false);
//   }


//   // Tile #2, Screen2
//   {
//     t2_label = lv_label_create(t2);
//     lv_label_set_text(t2_label, "Welcome to the workshop");
//     lv_obj_set_style_text_font(t2_label, &lv_font_montserrat_28, 0);
//     lv_obj_center(t2_label);

//     apply_tile_colors(t2, t2_label, /*dark=*/false);
//     lv_obj_add_flag(t2, LV_OBJ_FLAG_CLICKABLE);
//     lv_obj_add_event_cb(t2, on_tile2_clicked, LV_EVENT_CLICKED, NULL);
//   }
// }
// 



// Hjälpfunktion för att lägga till Settings-knappen på skärm
static void add_settings_button(lv_obj_t *parent) {
    lv_obj_t *btn_set = lv_btn_create(parent);
    lv_obj_set_size(btn_set, 70, 30);
    lv_obj_align(btn_set, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t *lbl_s = lv_label_create(btn_set);
    lv_label_set_text(lbl_s, "Settings");
    lv_obj_center(lbl_s);
}


// Funktion: Skapar UI med inbyggd swiping via Tileview
static void create_ui()
{
  // Fullscreen Tileview (Skapar ytan som hanterar swiping)
  tileview = lv_tileview_create(lv_scr_act());
  lv_obj_set_size(tileview, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

  // Lägg till 3 horisontella tiles efter varandra (Kolonn 0, 1 och 2)
  t_start      = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR); // Skärm 1: Start
  t_departures = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR); // Skärm 2: Avgångar
  t_settings   = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR); // Skärm 3: Inställningar

  
  // --- 1. START SCREEN (Tile 0) ---
  {
    add_settings_button(t_start);

    lv_obj_t *title = lv_label_create(t_start);
    lv_label_set_text(title, "Public Transport Info");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    lv_obj_t *lbl_info = lv_label_create(t_start);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "Version: 1.0.0\n\n"
             "Group 08\n\n"
             "- Andrej Petterson\n"
             "- Baqir Ibrahim\n"
             "- Loke Hallin\n"
             "- Maksym Zakariya\n"
             "- Samer Al Jadooa\n");
    lv_label_set_text(lbl_info, buffer);
    lv_obj_align(lbl_info, LV_ALIGN_CENTER, 0, 15);

    lv_obj_t *hint = lv_label_create(t_start);
    lv_label_set_text(hint, "<- Swipe to navigate ->");
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_28, 0);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -10);
  }

  
  // --- 2. DEPARTURES SCREEN (Tile 1) ---
  {
    add_settings_button(t_departures);

    lv_obj_t *title = lv_label_create(t_departures);
    lv_label_set_text(title, "Departures");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    dep_list_container = lv_obj_create(t_departures);
    lv_obj_set_size(dep_list_container, 220, 175);
    lv_obj_align(dep_list_container, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_flex_flow(dep_list_container, LV_FLEX_FLOW_COLUMN);
  }

  
  // --- 3. SETTINGS SCREEN (Tile 2) ---
  {

    lv_obj_t *title = lv_label_create(t_settings);
    lv_label_set_text(title, "Settings");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    // Dropdown för hållplatser
    dropdown_stops = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_stops, "Campus Grasvik\nKarlskrona Centralstation\nBergasa Station\nHultvagen\nLango");
    lv_obj_set_size(dropdown_stops, 210, 35);
    lv_obj_align(dropdown_stops, LV_ALIGN_CENTER, 0, -45);

    // Dropdown för transporttyper
    dropdown_transport = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_transport, "All\nBus\nTrain\nFerry");
    lv_obj_set_size(dropdown_transport, 210, 35);
    lv_obj_align(dropdown_transport, LV_ALIGN_CENTER, 0, 0);

    // Spara- och återställningsknappar
    lv_obj_t *btn_save = lv_btn_create(t_settings);
    lv_obj_set_size(btn_save, 95, 30);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_MID, -55, -15);
    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Save");
    lv_obj_center(lbl_save);

    lv_obj_t *btn_reset = lv_btn_create(t_settings);
    lv_obj_set_size(btn_reset, 95, 30);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_MID, 55, -15);
    lv_obj_t *lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "Reset");
    lv_obj_center(lbl_reset);
  }
}



// Function: Connects to WIFI
static void connect_wifi()
{
  Serial.printf("Connecting to WiFi SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < 15000) {
    delay(250);
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected.");
  } else {
    Serial.println("WiFi could not connect (timeout).");
  }
}


// Must have function: Setup is run once on startup
void setup()
{
  Serial.begin(115200);
  delay(200);

  if (!amoled.begin()) {
    Serial.println("Failed to init LilyGO AMOLED.");
    while (true) delay(1000);
  }

  beginLvglHelper(amoled);   // init LVGL for this board

  create_ui();
  connect_wifi();
}

// Must have function: Loop runs continously on device after setup
void loop()
{
  lv_timer_handler();
  delay(5);
}