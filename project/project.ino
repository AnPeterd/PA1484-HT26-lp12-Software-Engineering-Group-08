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

#include "departures.hpp"


static lv_obj_t* tileview;
static lv_obj_t* t_start;
static lv_obj_t* t_departures;
static lv_obj_t* t_settings;

static lv_obj_t *dep_list_container = NULL;
static lv_obj_t *dep_table = NULL;

static lv_obj_t *lbl_stopLabel = NULL;

// static lv_obj_t *lbl_current_stop = NULL;        //unnecessary, replaced by lbl_stopLabel
// static lv_obj_t *lbl_current_transport = NULL;

static lv_obj_t *dropdown_stops = NULL;
static lv_obj_t *dropdown_transport = NULL;

static lv_obj_t *btn_set = NULL;
static lv_obj_t *btn_save = NULL;
static lv_obj_t *btn_reset = NULL;
static lv_obj_t *btn_refresh = NULL;


// Statusikoner
static lv_obj_t *lbl_wifi_status = NULL;
static lv_obj_t *lbl_live_status = NULL;

const char* DEFAULT_STOP = "Campus Gräsvik";
const char* DEFAULT_TRANSPORT = "All Transport Types";

std::string current_stop_name = DEFAULT_STOP;
std::string current_transport_type = DEFAULT_TRANSPORT;


// Saved user settings
std::string saved_stop_name = DEFAULT_STOP;
std::string saved_transport_type = DEFAULT_TRANSPORT;


LilyGo_Class amoled;



// Funktion som sätter unik demodata baserat på både hållplats och transporttyp
void ui_load_demo_data(const char* stopName, const char* transportType) {
    if (!dep_table || current_stop_name.empty()) return;

    char stopLabel[128];
    snprintf(stopLabel, sizeof(stopLabel), "Stop: %s [%s]", stopName, transportType);
    lv_label_set_text(lbl_stopLabel, stopLabel);

    // Nollställer tabellen först så att den är tom om inga avgångar matchar
    for (int r = 1; r <= 4; r++) {
        lv_table_set_cell_value(dep_table, r, 0, "--:--");
        lv_table_set_cell_value(dep_table, r, 1, "-");
        lv_table_set_cell_value(dep_table, r, 2, "No departures");
    }

    int currentRow = 1;

    // --- CAMPUS GRÄSVIK ---
    if (strcmp(stopName, "Campus Gräsvik") == 0) {
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:15"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "3"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Wämö");
            currentRow++;
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:22"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "6"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Centralstationen");
            currentRow++;
        }
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Ferry") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:45"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Färja"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Hasslö");
            currentRow++;
        }
    }
    // --- KARLSKRONA CENTRALSTATION ---
    else if (strcmp(stopName, "Karlskrona Centralstation") == 0) {
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Train") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:10"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Malmö C");
            currentRow++;
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:50"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Göteborg C");
            currentRow++;
        }
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:20"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "1"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Bergåsa");
            currentRow++;
        }
    }
    // --- KARLSKRONA CENTRUM ---
    else if (strcmp(stopName, "Karlskrona centrum") == 0) {
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Bus") == 0) {
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
    else if (strcmp(stopName, "Kungsplan") == 0) {
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:18"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "2"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "IKEA");
            currentRow++;
        }
    }
    // --- BERGÅSA STATION ---
    else if (strcmp(stopName, "Bergåsa station") == 0) {
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Train") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:05"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "Tåg"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Karlskrona C");
            currentRow++;
        }
        if (strcmp(transportType, "All Transport Types") == 0 || strcmp(transportType, "Bus") == 0) {
            lv_table_set_cell_value(dep_table, currentRow, 0, "12:20"); 
            lv_table_set_cell_value(dep_table, currentRow, 1, "7"); 
            lv_table_set_cell_value(dep_table, currentRow, 2, "Blekingesjukhuset");
            currentRow++;
        }
    }
} // ^FÖR DEMODATA EJ LIVE DATA^


//deklarerat vissa funktioner.

void ui_apply_user_settings(const char* selectedStop, const char* selectedTransport);
void ui_update_departure_table(const StopData *stopData);
void ui_update_wifi_status_display(const char* statusText);
void ui_update_live_status_display(const char* statusText);



static lv_obj_t *add_refresh_button(lv_obj_t *parent) {
    lv_obj_t *btn_refresh = lv_btn_create(parent);
    lv_obj_set_size(btn_refresh, 40, 40);

    lv_obj_t *lbl_ref_icon = lv_label_create(btn_refresh);
    lv_label_set_text(lbl_ref_icon, LV_SYMBOL_REFRESH);
    lv_obj_center(lbl_ref_icon);

    return btn_refresh;
}

// Hjälpfunktion för att lägga till Settings-knappen på skärm
static void add_settings_button(lv_obj_t *parent) {
    btn_set = lv_btn_create(parent);
    lv_obj_set_size(btn_set, 90, 50);
    lv_obj_align(btn_set, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t *lbl_s = lv_label_create(btn_set);
    lv_label_set_text(lbl_s, "Settings");
    lv_obj_center(lbl_s);
}

static void create_departure_table(lv_obj_t* parent) {
    // Container för avgångstabellen
    dep_list_container = lv_obj_create(parent);

    lv_obj_set_size(dep_list_container, 400, 300);
    lv_obj_align(dep_list_container, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_flex_flow(dep_list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(dep_list_container, LV_SCROLLBAR_MODE_AUTO);

    // Uppdateringsknapp
    btn_refresh = add_refresh_button(dep_list_container);
    lv_obj_align(btn_refresh, LV_ALIGN_TOP_RIGHT, -5, -45);

    dep_table = lv_table_create(dep_list_container);
    lv_obj_set_size(dep_table, 380, 280);

    lv_table_set_row_cnt(dep_table, 6); // 1 header row + 5 departure rows
    // Kolumnbredder
    lv_table_set_col_width(dep_table, 0, 110);   // Time
    lv_table_set_col_width(dep_table, 1, 80);   // Line
    lv_table_set_col_width(dep_table, 2, 190);  // Destination

    // Rubriker
    lv_table_set_cell_value(dep_table, 0, 0, "Time");
    lv_table_set_cell_value(dep_table, 0, 1, "Line");
    lv_table_set_cell_value(dep_table, 0, 2, "Destination");

}




static lv_obj_t *add_stop_label(lv_obj_t *parent, const char* stopName, const char* transportType) {
    lbl_stopLabel = lv_label_create(parent);

    char initStopLabel[128];
    snprintf(initStopLabel, sizeof(initStopLabel), "Stop: %s [%s]", stopName, transportType);
    lv_label_set_text(lbl_stopLabel, initStopLabel);
    
    return lbl_stopLabel;
}

void ui_update_stop_label(const char* stopName, const char* transportType)
{
    if (lbl_stopLabel == nullptr || stopName == nullptr || transportType == nullptr) { return; }

    char text[128];

    snprintf(text, sizeof(text), "Stop: %s [%s]", stopName, transportType);

    lv_label_set_text(lbl_stopLabel, text);
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

    // lbl_stopLabel ska ändras dynamiskt baserat på användarens val i inställningarna (dropdown_stops)
    lbl_stopLabel = add_stop_label(t_departures, DEFAULT_STOP, DEFAULT_TRANSPORT);
    lv_obj_align(lbl_stopLabel, LV_ALIGN_TOP_MID, 0, 40);

    create_departure_table(t_departures);

    // Ladda data (avgångar) i tabellen härifrån:

  }

  
  // --- 3. SETTINGS SCREEN (Tile 2) ---
  {
    lv_obj_t *title = lv_label_create(t_settings);
    lv_label_set_text(title, "Settings");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lbl_wifi_status = lv_label_create(t_settings);
    lv_label_set_text(lbl_wifi_status, "Wi-Fi: Connecting...");
    lv_obj_set_style_text_font(lbl_wifi_status, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_wifi_status, LV_ALIGN_TOP_MID, 0, 35);

    // Använd ui_update_live_status_display() för att uppdatera denna label med live status från nätverksmodulen
    lbl_live_status = lv_label_create(t_settings);
    lv_label_set_text(lbl_live_status, "Live Data: Active (Trafiklab)");
    lv_obj_set_style_text_font(lbl_live_status, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_live_status, LV_ALIGN_TOP_MID, 0, 55);

    // Dropdown för hållplatser
    dropdown_stops = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_stops, "Campus Gräsvik\nKarlskrona Centralstation\nKarlskrona centrum\nKungsplan\nBergåsa station");
    lv_obj_set_size(dropdown_stops, 230, 55);
    lv_obj_align(dropdown_stops, LV_ALIGN_CENTER, 0, -35);

    // Dropdown för transporttyper
    dropdown_transport = lv_dropdown_create(t_settings);
    lv_dropdown_set_options(dropdown_transport, "All Transport Types\nBus\nTrain\nFerry");
    lv_obj_set_size(dropdown_transport, 230, 55);
    lv_obj_align(dropdown_transport, LV_ALIGN_CENTER, 0, 25);

    // Spara- och återställningsknappar
    btn_save = lv_btn_create(t_settings);
    lv_obj_set_size(btn_save, 115, 50);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_MID, -55, -35);
    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Save");
    lv_obj_center(lbl_save);

    btn_reset = lv_btn_create(t_settings);
    lv_obj_set_size(btn_reset, 115, 50);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_MID, 55, -35);
    lv_obj_t *lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "Reset");
    lv_obj_center(lbl_reset);


    lv_obj_t *refe = lv_label_create(t_settings);
    lv_label_set_text(refe, "Data source: Trafiklab API");
    lv_obj_align(refe, LV_ALIGN_BOTTOM_MID, 0, -10);
  }
}



// För DEMO tabellen (ej live data): funktion för att applicera nya användarinställningar (anropas t.ex. vid Save/Reset) 
void ui_apply_user_settings(const char* selectedStop, const char* selectedTransport) {
    // Ladda om demodatan i tabellen (och uppdatera titeln) med de nya valen
    ui_load_demo_data(selectedStop, selectedTransport);
}


// Funktion för att uppdatera tabellen med ny live data från nätverks-/logikmodulen
void ui_update_departure_table(const StopData *stopData) {
    if (!stopData || !dep_table && current_stop_name.c_str()) { // kontrollera att current_stop_name inte är tom.
        ui_update_live_status_display("No Departures");
        return;   
    }  

    // Uppdatera titeln med den aktiva hållplatsen (den användaren valt)
    char stopLabel[128];
    snprintf(stopLabel, sizeof(stopLabel), "Stop: %s [%s]", stopData->getName().c_str(), current_transport_type);
    lv_label_set_text(lbl_stopLabel, stopLabel);

    ui_update_live_status_display("Active"); // Uppdatera live status

    for (int i = 0; i < stopData->getCount() && i < 5; i++) {
        const Departure* departure = stopData->getDeparture(i);
        int rowIndex = i + 1;

        bool matches = strcmp(departure->getTransportMode().c_str(), current_transport_type.c_str()) == 0 || strcmp(current_transport_type.c_str(), DEFAULT_TRANSPORT) == 0;

        if(!matches) {
            // Om transporttypen inte matchar, hoppa över denna avgång
            continue;
        }  
        
        if (departure != nullptr && matches) 
        {
            std::string timeStatus = std::string(departure->getTime().c_str()) + " - " + (departure->getStatus().c_str());

            lv_table_set_cell_value( dep_table, rowIndex, 0, timeStatus.c_str() );

            lv_table_set_cell_value( dep_table, rowIndex, 1, departure->getLine().c_str() );

            lv_table_set_cell_value( dep_table, rowIndex, 2, departure->getDestination().c_str() );
        }

        else {
            lv_table_set_cell_value(dep_table, rowIndex, 0, "--:--");
            lv_table_set_cell_value(dep_table, rowIndex, 1, "-");
            lv_table_set_cell_value(dep_table, rowIndex, 2, "No departures");
        }
    }
}

// Funktion för att uppdatera Wi-Fi status på skärmen
void ui_update_wifi_status_display(const char* statusText) {
    if (lbl_wifi_status) {
        lv_label_set_text(lbl_wifi_status, statusText);
    }
}

// Funktion för att uppdatera live status på skärmen
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
    Serial.println ("Unexpectrd WiFi error:");
    Serial.println (wifi_status);
    ui_update_wifi_status_display("Wi-Fi: Unexpected Error");
    return 0;

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

touch_input_init(
    btn_set,
    btn_save,
    btn_reset,
    btn_refresh,
    dropdown_stops,
    dropdown_transport
);

  int8_t WiFi_code = connect_wifi(); // if WiFi_code != 1 -> WiFi not connected (for 4.3)
  
  //Testing requests
  int8_t request_result = 6;
  if (WiFi_code==1)
  {
    request_result = request_send("740032188");
  }
  if (request_result >= 4)
  {
    Serial.println ("Request failed");
    //Possible event here
  }
}

// Must have function: Loop runs continously on device after setup
void loop()
{
  //when ready - current_stop variable should be here 
  const Stop& current_stop = STOPS[0]; //Placeholder; When proper objects are ready - should be replaced

  //US4.1: As a user, I want the live departure data to update automatically in 60 seconds

  static uint32_t lastRefresh = 0;

if (millis() - lastRefresh >= 60000) {

    int8_t request_status = refresh_departures(current_stop.id);
    
    //If request_status >=3 - error, else OK
    
    lastRefresh = millis();
}


  lv_timer_handler();
  delay(5);
}