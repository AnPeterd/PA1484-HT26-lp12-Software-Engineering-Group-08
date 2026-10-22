#include "HTTP.hpp"
#include "secrets.h"
#include "departure_parser.hpp"


int8_t request_send(String stop_id, StopData& result)
{
    if (WiFi.status()!= WL_CONNECTED)
    {
        Serial.println("No WiFi connection - unable to send request");
        return 6;
    }

    HTTPClient http; 

    String url = String("https://realtime-api.trafiklab.se/v1/departures/") + stop_id + "?key=" + API_KEY; // creation of URL for the upcoming request
    
    http.useHTTP10(true); //for smoother stream data - no chunked transfer encoding
    http.setConnectTimeout(5000);
    http.setTimeout(10000); //Limiting connection time

    bool begin_result = http.begin(url);

    if (begin_result == false) 
    {
        Serial.println ("Request not sent - bad url");
        return 6;
    }

    int http_code = http.GET();


    if (http_code != 200) //reworked HTTP faliure logic
    {
      Serial.println("Unexpected HTTP error:");
      Serial.println (http_code);

      http.end();

      if (http_code >= 300 && http_code < 600)
      {
        return http_code / 100; //HTTP faliures
      }
      else
      {
        return 6; //Other faliures
      }

    }

    bool parsed = parseDepartures(http.getStream(), result);

    http.end(); //closing the HTTP connection

    if (!parsed) {
    Serial.println("Parsing failed");
    return 7; // parsing faliure 
    }


    return 2;
}



int8_t refresh_departures(String stop_id, StopData& result)
{
    int8_t refresh_status = request_send(stop_id, result);
    if (refresh_status >= 3)
    {
      wl_status_t wifi_status = WiFi.status();
      if (wifi_status != WL_CONNECTED)
      {
        bool troubleshoot_status = troubleshoot_network();
        if (troubleshoot_status ==false)
        {
          Serial.println("Fatal WiFi error");
        }
        else
        {
          // Wi-Fi recovered - retry request
           refresh_status = request_send(stop_id, result);
           
          // No further recovery attempts are made if the retry fails
        }
      }
      else
      {
        switch (refresh_status)
        {
          case 3:
          Serial.println("HTTP redirect");
          break;

          case 4:
          Serial.println("HTTP client error");
          break;

          case 5:
          Serial.println("API/server error");
          break;
          
          case 6:
          // transport/network handling
          break;
        }
      }
    }

    return refresh_status;
}