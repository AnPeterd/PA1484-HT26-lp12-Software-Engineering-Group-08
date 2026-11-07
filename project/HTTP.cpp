#include "HTTP.hpp"
#include "secrets.h"
#include "departure_parser.hpp"

int8_t request_send (String url, StopData& result, const String& transport, StopData& result_route, int8_t request_type)
{

  HTTPClient http;

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

    bool parsed = false;

    if (request_type ==1) //departure type
    {
      parsed = parseDepartures(http.getStream(), result, transport);
    }

    else if (request_type == 0)
    {
      parsed = true; //Parcer function here
    }

        http.end(); //closing the HTTP connection

    if (!parsed) {
    Serial.println("Parsing failed");
    return 7; // parsing faliure 
    }


    return 2;
}



int8_t refresh_departures(String stop_id, StopData& result, const String& transport)
{
  StopData dummy{"", ""};
  String url = String("https://realtime-api.trafiklab.se/v1/departures/") + stop_id + "?key=" + API_KEY;
    int8_t refresh_status = request_send(url, result, transport, dummy, 1);
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
           refresh_status = request_send(url, result, transport, dummy, 1);
           
          // No further recovery attempts are made if the retry fails
        }
      }

      //Rework of HTTP Errors is comming soon
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

          case 7:
          // parsing problem
          break;
        }
      }
    }

    return refresh_status;
}



int8_t route_refresh (String start_stop_id, String end_stop_id, StopData& result)
{
  StopData dummy{"", ""};//Object used for the irrelevent data type
  String url = String("https://api.resrobot.se/v2.1/trip?originId=") + start_stop_id + "&destId=" + end_stop_id + "&format=json&numF=3&passlist=0&poly=0&accessId=" + RESROBOT_API_KEY; // creation of URL for the upcoming request
  int8_t refresh_status = request_send(url, dummy, "ALL", result, 0);
  return refresh_status;

}