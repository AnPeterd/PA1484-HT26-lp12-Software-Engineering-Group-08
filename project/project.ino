#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <time.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>
#include <string>

#include "secrets.h"
#include "WiFi.hpp"
#include "HTTP.hpp"
#include "TouchInput.hpp"

#include "memory_saver.hpp"

#include "departures.hpp"

// Swedish font
LV_FONT_DECLARE(montserrat_swedish_14);
LV_FONT_DECLARE(montserrat_swedish_16);

#include <iostream>
#include <algorithm>



MemorySaver saved_settings;

static lv_obj_t* tileview;
static lv_obj_t* t_start;
static lv_obj_t* t_journey;
static lv_obj_t* t_departures;
static lv_obj_t* t_settings;

static lv_obj_t *journey_table = NULL;

static lv_obj_t *dep_list_container = NULL;
static lv_obj_t *dep_table = NULL;

static lv_obj_t *lbl_stopLabel = NULL;

static lv_obj_t *dropdown_stops = NULL;
static lv_obj_t *dropdown_transport = NULL;

static lv_obj_t *btn_set = NULL;
static lv_obj_t *btn_save = NULL;
static lv_obj_t *btn_reset = NULL;
static lv_obj_t *btn_refresh = NULL;


// Status icons
static lv_obj_t *lbl_wifi_status = NULL;
static lv_obj_t *lbl_live_status = NULL;

const String DEFAULT_STOP = "Campus Gräsvik";
const String DEFAULT_TRANSPORT = "All Transport Types";

String current_stop_name = DEFAULT_STOP;
String current_transport_type = DEFAULT_TRANSPORT;


// Saved user settings
String saved_stop_name = DEFAULT_STOP;
String saved_transport_type = DEFAULT_TRANSPORT;

// Stores the latest successfully parsed departures for Campus Gräsvik
StopData currentDepartures{STOPS[0].name, STOPS[0].id};
//Same idea for journeys
JourneyResults currentJourneys;

uint32_t lastRefresh = 0;


LilyGo_Class amoled;



// function that sets unic demodata based on both stop and transport type
void ui_load_demo_data(const char* stopName, const char* transportType) {
    if (!dep_table || current_stop_name.isEmpty()) return;

    char stopLabel[128];
    snprintf(stopLabel, sizeof(stopLabel), "Stop: %s [%s]", stopName, transportType);
    lv_label_set_text(lbl_stopLabel, stopLabel);

    // Reset the table first so that it is empty if no departures match
    for (int r = 1; r <= 4; r++) {
        lv_table_set_cell_value(dep_table, r, 0, "--:--");
        lv_table_set_cell_value(dep_table, r, 1, "-");
        lv_table_set_cell_value(dep_table, r, 2, "No departures");
    }

    int currentRow = 1;

    // --- CAMPUS GRÄSVIK ---
    if (strcmp(stopName, "CAMPUS GRÄSVIK") == 0) {
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:15"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "3"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Wämö");
            currentRow++;
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:22"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "6"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Centralstationen");
            currentRow++;
        }
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Ferry") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:45"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Färja"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Hasslö");
            currentRow++;
        }
    }
    // --- KARLSKRONA CENTRALSTATION ---
    else if (strcmp(stopName, "KARLSKRONA CENTRALSTATION") == 0) {
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Train") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:10"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Malmö C");
            currentRow++;
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:50"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Göteborg C");
            currentRow++;
        }
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:20"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "1"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Bergåsa");
            currentRow++;
        }
    }
    // --- KARLSKRONA CENTRUM ---
    else if (strcmp(stopName, "KARLSKRONA CENTRUM") == 0) {
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:12"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "5"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Hästö");
            currentRow++;
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:25"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "3"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Verkö");
            currentRow++;
        }
    }
    // --- KUNGSPLAN ---
    else if (strcmp(stopName, "KUNGSPLAN") == 0) {
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:18"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "2"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "IKEA");
            currentRow++;
        }
    }
    // --- BERGÅSA STATION ---
    else if (strcmp(stopName, "BERGÅSA STATION") == 0) {
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Train") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:05"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Karlskrona C");
            currentRow++;
        }
        if (strcmp(transportType, "ALL TRANSPORT TYPES") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:20"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "7"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Blekingesjukhuset");
            currentRow++;
        }
    }
} // ^FOR DEMODATA; NOT LIVE DATA^


// declared some functions.

void ui_apply_user_settings(const char* selectedStop, const char* selectedTransport);
void ui_update_departure_table(const StopData *stopData);
void ui_update_wifi_status_display(const char* statusText);
void ui_update_live_status_display(const char* statusText);



static lv_obj_t *add_refresh_button(lv_obj_t *parent) {
    lv_obj_t *btn_refresh = lv_btn_create(parent);
    lv_obj_set_size(btn_refresh, 50, 50);

    lv_obj_t *lbl_ref_icon = lv_label_create(btn_refresh);
    lv_label_set_text(lbl_ref_icon, LV_SYMBOL_REFRESH);
    lv_obj_center(lbl_ref_icon);

    return btn_refresh;
}

// function for placing Settings-btn on screen
static void add_settings_button(lv_obj_t *parent) {
    btn_set = lv_btn_create(parent);
    lv_obj_set_size(btn_set, 90, 50);
    lv_obj_align(btn_set, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t *lbl_s = lv_label_create(btn_set);
    lv_label_set_text(lbl_s, "Settings");
    lv_obj_center(lbl_s);
}

static void create_departure_table(lv_obj_t* parent) {
    // Container for dep.table
    dep_list_container = lv_obj_create(parent);

    lv_obj_set_size(dep_list_container, 580, 360);
    lv_obj_align(dep_list_container, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_flex_flow(dep_list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(dep_list_container, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(dep_list_container, LV_DIR_VER);
    lv_obj_set_style_pad_all(dep_list_container, 0, LV_PART_MAIN);

    // Refresh btn
    btn_refresh = add_refresh_button(t_departures);
    lv_obj_align(btn_refresh, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_size(btn_refresh, 60, 60);

    dep_table = lv_table_create(dep_list_container);
    lv_obj_set_size(dep_table, lv_pct(100), lv_pct(100));

    lv_table_set_row_cnt(dep_table, 6); // 1 header row + 5 departure rows
   
    // Column-width
    lv_table_set_col_width(dep_table, 0, 180);   // Time
    lv_table_set_col_width(dep_table, 1, 130);   // Line
    lv_table_set_col_width(dep_table, 2, 270);  // Destination

    // Header row
    lv_table_set_cell_value(dep_table, 0, 0, "Time");
    lv_table_set_cell_value(dep_table, 0, 1, "Line");
    lv_table_set_cell_value(dep_table, 0, 2, "Destination");
    lv_obj_set_style_text_align(dep_table, LV_TEXT_ALIGN_CENTER, LV_PART_ITEMS);
}




static lv_obj_t *add_stop_label(lv_obj_t *parent, const char* stopName, const char* transportType) 
{
    lbl_stopLabel = lv_label_create(parent);

    char initStopLabel[128];
    snprintf(initStopLabel, sizeof(initStopLabel), "Stop: %s [%s]", stopName, transportType);
    lv_label_set_text(lbl_stopLabel, initStopLabel);
    lv_obj_set_style_text_font(lbl_stopLabel, &montserrat_swedish_14, 0);
    
    return lbl_stopLabel;
}

void ui_update_stop_label(const char* stopName, const char* transportType)
{
    if (lbl_stopLabel == nullptr || stopName == nullptr || transportType == nullptr) { return; }

    char text[128];

    snprintf(text, sizeof(text), "Stop: %s [%s]", stopName, transportType);

    lv_label_set_text(lbl_stopLabel, text);
    lv_obj_set_style_text_font(lbl_stopLabel, &montserrat_swedish_14, 0);
}


void ui_set_current_stop(const char* stopName)
{
    if (stopName == nullptr) { return; }

    current_stop_name = stopName;

    ui_update_stop_label( current_stop_name.c_str(), current_transport_type.c_str() );

    // TODO: Uppdatering av avgångstabellen. Här behövs StopData-objektet som innehåller avgångarna för den valda hållplatsen.
    // för att: update_departure_table(stopData);


}


void ui_set_current_transport(const char* transportType)
{
    if (transportType == nullptr) {
        return;
    }

    current_transport_type = transportType;

    ui_update_stop_label(current_stop_name.c_str(), current_transport_type.c_str());

    // TODO: Uppdatering av avgångstabellen. Här behövs StopData-objektet som innehåller avgångarna för den valda hållplatsen (current_stop_name).
    // för att: update_departure_table(stopData);
}


void ui_journey_table(lv_obj_t* parent){

    lv_obj_t* journey_label = lv_label_create(parent);
    lv_label_set_text(journey_label, "Journey Details");
    lv_obj_align(journey_label, LV_ALIGN_TOP_LEFT, 20, 50);

    lv_obj_t* journey_container = lv_table_create(parent);
    lv_obj_set_size(journey_container, 560, 310);
    lv_obj_align(journey_container, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_pad_all(journey_container, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(journey_container, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(journey_container, LV_DIR_VER);


    lv_obj_t* journey_table = lv_table_create(journey_container);
    lv_obj_set_size(journey_table, lv_pct(100), lv_pct(100));

    lv_table_set_row_cnt(journey_table, 6);
    lv_table_set_col_cnt(journey_table, 2); 

    lv_table_set_col_width(journey_table, 0, 310);   // Stop
    lv_table_set_col_width(journey_table, 1, 250);   // Time

    // Header row
    lv_table_set_cell_value(journey_table, 0, 0, "Bus Stop");
    lv_table_set_cell_value(journey_table, 0, 1, "Arrival Time");

    lv_obj_set_style_text_align(journey_table, LV_TEXT_ALIGN_CENTER, LV_PART_ITEMS);

     lv_obj_set_style_text_font(journey_table, &montserrat_swedish_14, LV_PART_ITEMS);
}



void ui_update_journey_table(const StopData *stopData, const Departure *departure, int nrStops) {
    if (!journey_table || !stopData) return;

    lv_obj_t* journey_lbl = lv_label_create(t_journey);

    char stopLabel[128];
    snprintf(stopLabel, sizeof(stopLabel), "Stop: %s [%s]", stopData->getName().c_str(), departure->getDestination().c_str());
    lv_label_set_text(journey_lbl, stopLabel);

    for (int r = 1; r <= nrStops; r++) {
        lv_table_set_cell_value(journey_table, r, 0, "");
        lv_table_set_cell_value(journey_table, r, 1, ""); 
    }


    /* example for how the journey table could be updated with stops and arrival times
    for (const auto& stop : departure->getStops()) {
        lv_table_set_cell_value(journey_table, currentRow, 0, stop.getName().c_str());
        lv_table_set_cell_value(journey_table, currentRow, 1, stop.getArrivalTime().c_str());
    }
    */
}


// Function to create the UI
static void create_ui()
{
  // Fullscreen Tileview (tiles for swiping)
  tileview = lv_tileview_create(lv_scr_act());
  lv_obj_set_size(tileview, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

  // horisontal tiles (Kolonn 0, 1 och 2) 
  t_start      = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR); // Tile 0: start
  t_journey    = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR); // Tile 1: Journey
  t_departures = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR); // Tile 2: departures
  t_settings   = lv_tileview_add_tile(tileview, 3, 0, LV_DIR_HOR); // Tile 3: settings



  // --- 1. START SCREEN (Tile 0) ---
  {

    lv_obj_t *title = lv_label_create(t_start);
    lv_label_set_text(title, "Public Transport Info");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    lv_obj_t *lbl_info = lv_label_create(t_start);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "Version: 1.5.0\n\n"
             "Group 08\n\n"
             "- Andrej Petterson\n"
             "- Baqir Ibrahim\n"
             "- Loke Hallin\n"
             "- Maksym Zakariya\n"
             "- Samer Al Jadooa\n");
    lv_label_set_text(lbl_info, buffer);
    lv_obj_align(lbl_info, LV_ALIGN_CENTER, 0, 15);

    lv_obj_t *hint = lv_label_create(t_start);
    lv_label_set_text(hint, "Swipe to navigate ->");
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_28, 0);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -10);
  }


  // --- 2. JOURNEY SCREEN (Tile 1) ---
  {
    lv_obj_t *title = lv_label_create(t_journey);
    lv_label_set_text(title, "Journey: Choose a Destination From Departure Screen");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    // Coming soon US 5.1: Dropdown for stops and destinations? , when clicked - 
    // should show a table (eg. 10 row and column) over all the stops on that departure where the last stop is the destination.

    ui_journey_table(t_journey);



   
  }

  
  // --- 3. DEPARTURES SCREEN (Tile 2) ---
  {
    // add_settings_button(t_departures);

    lv_obj_t *title = lv_label_create(t_departures);
    lv_label_set_text(title, "Departures");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    lbl_stopLabel = add_stop_label(t_departures, DEFAULT_STOP.c_str(), DEFAULT_TRANSPORT.c_str());
    lv_obj_align(lbl_stopLabel, LV_ALIGN_TOP_MID, 0, 40);

    create_departure_table(t_departures);

  }

  
  // --- 4. SETTINGS SCREEN (Tile 3) ---
  {
    lv_obj_t *title = lv_label_create(t_settings);
    lv_label_set_text(title, "Settings");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lbl_wifi_status = lv_label_create(t_settings);
    lv_label_set_text(lbl_wifi_status, "Wi-Fi: Connecting...");
    lv_obj_set_style_text_font(lbl_wifi_status, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_wifi_status, LV_ALIGN_TOP_MID, 0, 35);

    // Use ui_update_live_status_display() to update this label with live status from the network module
    lbl_live_status = lv_label_create(t_settings);
    lv_label_set_text(lbl_live_status, "Live Data: Active (Trafiklab)");
    lv_obj_set_style_text_font(lbl_live_status, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_live_status, LV_ALIGN_TOP_MID, 0, 55);

    dropdown_stops = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_stops, "Campus Gräsvik\nKarlskrona Centralstation\nKarlskrona Centrum\nKungsplan\nBergåsa Station");
    lv_obj_t *list = lv_dropdown_get_list(dropdown_stops);

    lv_obj_set_style_text_font(list, &montserrat_swedish_16, 0);
    // lv_obj_set_style_text_font(list, &lv_font_montserrat_16, 0);

    lv_obj_set_style_text_font(dropdown_stops, &montserrat_swedish_16, LV_PART_MAIN);
    // lv_obj_set_style_text_font(dropdown_stops, &lv_font_montserrat_16, LV_PART_MAIN);

    lv_obj_set_style_text_font(dropdown_stops,LV_FONT_DEFAULT, LV_PART_INDICATOR);
    lv_obj_set_size(dropdown_stops, 230, 55);
    lv_obj_align(dropdown_stops, LV_ALIGN_CENTER, 0, -40);

    // Dropdown for transport types
    dropdown_transport = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_transport, "All Transport Types\nBus\nTrain\nFerry\nTaxi");
    lv_obj_set_size(dropdown_transport, 230, 55);
    lv_obj_align(dropdown_transport, LV_ALIGN_CENTER, 0, 30);

    // Save and Reset buttons
    btn_save = lv_btn_create(t_settings);
    lv_obj_set_size(btn_save, 135, 70);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_MID, -75, -45);
    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Save");
    lv_obj_set_style_text_font(lbl_save, &lv_font_montserrat_34, 0);
    lv_obj_center(lbl_save);

    btn_reset = lv_btn_create(t_settings);
    lv_obj_set_size(btn_reset, 135, 70);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_MID, 75, -45);
    lv_obj_t *lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "Reset");
    lv_obj_set_style_text_font(lbl_reset, &lv_font_montserrat_34, 0);
    lv_obj_center(lbl_reset);


    lv_obj_t *refe = lv_label_create(t_settings);
    lv_label_set_text(refe, "Data source: Trafiklab API");
    lv_obj_align(refe, LV_ALIGN_BOTTOM_MID, 0, -10);
  }
}



// For DEMO table (not live data): function to apply new user settings (called e.g. via Save/Reset)
void ui_apply_user_settings(const char* selectedStop, const char* selectedTransport) {
    // Update the current stop and transport type based on user selection
    ui_load_demo_data(selectedStop, selectedTransport);
}

void ui_update_departure_table(const StopData *stopData) {

    if (stopData == nullptr || dep_table == nullptr) {
        ui_update_live_status_display("No Departures");
        return;
    }

    // Update stop label
    char stopLabel[128];
    snprintf(stopLabel, sizeof(stopLabel), "Stop: %s [%s]", stopData->getName().c_str(), current_transport_type.c_str());
    lv_label_set_text(lbl_stopLabel, stopLabel);

    ui_update_live_status_display("Live Data: Active (Trafiklab)");

    // Clear previous departure data
    for (int row = 1; row <= 5; row++) {
        lv_table_set_cell_value(dep_table, row, 0, "");
        lv_table_set_cell_value(dep_table, row, 1, "");
        lv_table_set_cell_value(dep_table, row, 2, "");
    }


    for (int i = 0; i < stopData->getCount() && i < 5; i++) {

        const Departure* departure = stopData->getDeparture(i);
        if (departure == nullptr) continue;

        int rowIndex = i + 1;

        String timeStatus = departure->getTime() + " - " + departure->getStatus();

        lv_table_set_cell_value(dep_table, rowIndex, 0, timeStatus.c_str());

        if (current_transport_type.equalsIgnoreCase(DEFAULT_TRANSPORT)) {
            String mode_line = departure->getLine() + String(" / ") + departure->getTransportMode();
            lv_table_set_cell_value(dep_table, rowIndex, 1, mode_line.c_str());
        } 

        else {
                lv_table_set_cell_value(dep_table, rowIndex, 1, departure->getLine().c_str());
        }

        lv_table_set_cell_value(dep_table, rowIndex, 2, departure->getDestination().c_str());
    }


    if (stopData->getCount() == 0) {
        for (int row = 1; row <= 5; row++) {
            lv_table_set_cell_value(dep_table, row, 0, "--:--");
            lv_table_set_cell_value(dep_table, row, 1, "-");
            lv_table_set_cell_value(dep_table, row, 2, "No departures");
        }
    }

    lv_obj_set_style_text_font(dep_table, &montserrat_swedish_14, LV_PART_ITEMS);
}


// Function for updating Wi-Fi status on the screen
void ui_update_wifi_status_display(const char* statusText) {
    if (lbl_wifi_status) {
        lv_label_set_text(lbl_wifi_status, statusText);
    }
}

// Function for updating live status on the screen
void ui_update_live_status_display(const char* statusText) {
    if (lbl_live_status) {
        lv_label_set_text(lbl_live_status, statusText);
    }
}




// Function: Connects to WIFI
static int8_t connect_wifi()
{
  IPAddress ipv4; //empty template IPs

  Serial.printf("Connecting to WiFi SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < 15000) {
    delay(250);
  }
  Serial.println();

  wl_status_t wifi_status = WiFi.status();

  switch (wifi_status) // a bit nore advanced troubleshooting info
  {
    case (WL_CONNECTED): 
    Serial.println("WiFi connected succesfully.");
    ipv4 = WiFi.localIP();
    Serial.println(ipv4);
    ui_update_wifi_status_display("Wi-Fi: Connected (Online)");
    return 1;
    case (WL_IDLE_STATUS): 
    Serial.println ("Pending WiFi status (timeout WL_IDLE_STATUS).");
    ui_update_wifi_status_display("Wi-Fi: Idle / Connecting");
    return -1;
    case (WL_CONNECT_FAILED):
    Serial.println ("WiFi connection failed (WL_CONNECT_FAILED).");
    ui_update_wifi_status_display("Wi-Fi: Connection Failed");
    return -1;
    case (WL_NO_SSID_AVAIL):
    Serial.println ("WiFi network not found (WL_NO_SSID_AVAIL)");
    ui_update_wifi_status_display("Wi-Fi: Network Not Found");
    return -1;
    case (WL_DISCONNECTED):
    Serial.println ("WiFi not active (WL_DISCONNECTED)");
    ui_update_wifi_status_display("Wi-Fi: Disconnected");
    return -1;
    case (WL_CONNECTION_LOST):
    Serial.println ("WiFi connected, but connection lost (WL_CONNECTION_LOST)");
    ui_update_wifi_status_display("Wi-Fi: Connection Lost");
    return -1;
    default:
    Serial.println ("Unexpected WiFi error:");
    Serial.println (wifi_status);
    ui_update_wifi_status_display("Wi-Fi: Unexpected Error");
    return 0;

  }
}


//Data updating function connected to manual update button and automatic data
void refreshNow()
{
    int8_t status = refresh_departures(currentStop.id, currentDepartures, String(current_transport_type.c_str()));

    if (status == 2)
        ui_update_departure_table(&currentDepartures);
    else
        ui_update_live_status_display("Update failed");

    lastRefresh = millis();
}

void applySettings()
{
    currentDepartures = StopData{currentStop.name,currentStop.id};
    ui_update_departure_table(&currentDepartures);
    refreshNow();
    
    return;
}

void restoreSettings()
{
    /*Old version, restoring to Campus Grasvik and All transport types*/
    // selectStop(0);
    // ui_set_current_stop(currentStop.name);
    
    // //current_transport_type = DEFAULT_TRANSPORT;
    // ui_set_current_transport(DEFAULT_TRANSPORT);
    
    // lv_dropdown_set_selected(dropdown_stops, 0);
    // lv_dropdown_set_selected(dropdown_transport, 0);

    //current_transport_type = DEFAULT_TRANSPORT;
    ui_set_current_transport(DEFAULT_TRANSPORT.c_str());
    lv_dropdown_set_selected(dropdown_transport, 0);

    /*Restore settings from built-in memory*/
    loadSettings();
    
    return;
}

void loadSettings()
{
    const char* labels[] = {
        "All Transport Types",
        "Bus",
        "Train",
        "Ferry"
    };

    uint32_t latestStop = saved_settings.loadUInt("latestStop", 0);
    uint32_t latestTransport = saved_settings.loadUInt("latestTransport", 0);

    if (latestStop >= 5) {
        latestStop = 0;
    }
    selectStop(latestStop);
    if (latestStop >= 5) {latestStop = 0;}
    ui_set_current_stop(currentStop.name);
    lv_dropdown_set_selected(dropdown_stops, latestStop);
    
    if (latestTransport >= 4) {latestTransport = 0;}

    // current_transport_type = labels[latestTransport];
    ui_set_current_transport(labels[latestTransport]);

    lv_dropdown_set_selected(dropdown_stops, latestStop);
    lv_dropdown_set_selected(dropdown_transport, latestTransport);

    applySettings();
    
    return;
}

void saveSettings()
{
    saved_settings.saveUInt("latestStop", lv_dropdown_get_selected(dropdown_stops));
    saved_settings.saveUInt("latestTransport", lv_dropdown_get_selected(dropdown_transport));
    
    return;
}


// Must have function: Setup is run once on startup
void setup()
{
    Serial.begin(115200);
    delay(1000);

    if (!saved_settings.begin()) {    // open map with settings
        Serial.println("Failed to init Saved Settings");
    }
    
    if (!amoled.begin()) {
        Serial.println("Failed to init LilyGO AMOLED.");
        while (true) delay(1000);
    }
        
    beginLvglHelper(amoled);   // init LVGL for this board
        
    create_ui();
    touch_input_init(
        btn_set,
        btn_save,
        btn_reset,
        btn_refresh,
        dropdown_stops,
        dropdown_transport
    );
        
    
    int8_t WiFi_code = connect_wifi(); // if WiFi_code != 1 -> WiFi not connected (for 4.3)
    
    loadSettings();

    //testing journey request
    int8_t route_status = route_refresh(STOPS[6].id, STOPS[5].id, currentJourneys);
    Serial.println( route_status); 
    Serial.println(currentJourneys.getCount());

    if (route_status == 2)
    {
        //Test print of the first found journey
        const Journey* journey = currentJourneys.getJourney(0);

        Serial.println(journey->getOriginName());
        Serial.println(journey->getDestinationName());
        Serial.println(journey->getOriginTime());
        Serial.println(journey->getDestinationTime());
        Serial.println(journey->getLegCount());
    }
    }

// Must have function: Loop runs continously on device after setup
void loop()
{
lv_timer_handler();

    //Implementing TouchInput, revorking loop.
    if (touch_input_has_event())
    {

        //Getting current input event
        InputEvent event = touch_input_get_event();

        switch (event)
        {
            case SETTINGS_BUTTON:
                lv_obj_set_tile_id(tileview, 2, 0, LV_ANIM_ON); //Changing to the settings screen

                break;

            case REFRESH_BUTTON:
                if (millis() - lastRefresh >= 5000){
                    refreshNow();
                }

                break;

            case STOP_DROPDOWN:
            {
                //Avaliable stops
                uint16_t index = lv_dropdown_get_selected(dropdown_stops); //get stop index from dropdown
                
                if (index < STOP_COUNT && selectStop(index))
                {
                    //Changing current stop data
                    ui_set_current_stop(currentStop.name);

                    applySettings();
                }

                break;
            }

            case TRANSPORT_DROPDOWN:
            {
                //Avaliable transport types
                uint16_t index = lv_dropdown_get_selected(dropdown_transport); //get transport index from dropdown
                
                const char* labels[] = {
                    "All Transport Types",
                    "Bus",
                    "Train",
                    "Ferry"
                };
                
                if (index < 4)
                {
                    //Changing current transport type data
                    ui_set_current_transport(labels[index]);
                    applySettings();
                }

                break;
            }

            case SAVE_BUTTON:
            {
                saveSettings();
                
                break;
            }

            case RESET_BUTTON:
            {
                restoreSettings();

                break;
            }

            default:
                break;
        }
    }

    if (millis() - lastRefresh >= 60000)
        refreshNow();

    delay(5);
}