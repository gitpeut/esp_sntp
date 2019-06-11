
#include <wificredentials.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()
#ifdef LWIP_IPV6
#include <AddrList.h>
#endif

#endif

#ifdef ESP32
#include "WiFi.h"
#include "time.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#endif


int         ntpAskf    = 3765 * 1000; // every 3765 seconds
int         ntpAsk     = 0; // Threshold above which ask is repeated

void time_is_set (void)
{
  Serial.println("------------------ settimeofday() was called ------------------");
  tellTime();
  
}

/*----------------------------------------------------------------*/

void ntp_refresh(){
 int  now=millis();

 
 if ( ntpAsk < now ){ 
   Serial.println("refreshing ntp");

   #ifdef ESP8266
   configTime(0, 0, ntpServers[0],ntpServers[1],ntpServers[2]);
   #endif
   
  #ifdef ESP32
    sntp_stop(); 
    sntp_setoperatingmode(SNTP_OPMODE_POLL); 

    sntp_setservername(0, ntpServers[0]); 
    sntp_setservername(1, ntpServers[1]); 
    sntp_setservername(2, ntpServers[2]); 

    sntp_init(); 
 #endif
    
    tellTime();
    Serial.println("ntp refreshed");
    
   ntpAsk = now + ntpAskf;
 }

}

/*----------------------------------------------------------------*/


int time_to_jurassic(void )
{
  int            result;
  int            saved_error;

  time_t         new_time;

  struct tm      lassie;

  struct timeval garfield;

  lassie.tm_sec =0;
  lassie.tm_min =0;
  lassie.tm_hour=0;
  lassie.tm_mday=24;
  lassie.tm_mon =0;          
  lassie.tm_year=1979-1900;

  new_time=mktime(&lassie);

  garfield.tv_sec =new_time;
  garfield.tv_usec=0;

  result=settimeofday(&garfield,NULL);

  return result;

}

/*--------------------------------------------------------------*/

void ntp_waitforsync(){
time_t  rawt;
struct tm *tinfo;
int y;
    
   for(;;){
     time( &rawt );
     tinfo = localtime( &rawt );
     
//     Serial.printf( "Waiting for ntp sync (WiFi %s), time is now ", (WiFi.status() == WL_CONNECTED)? "connected":"not connected" );
       Serial.printf( "Waiting for ntp sync, time is now " );
     Serial.print(ctime(&rawt));

     y = tinfo->tm_year + 1900;
     if ( y > 2000 ) break;
     
     delay(500); 
   }

Serial.printf("%s\n", tinfo->tm_isdst?"DST in effect": "DST not in effect"); 
    
}

/*--------------------------------------------------------------*/

void ntp_setup(bool waitforsync){ 

  

  // set timezone

  if ( waitforsync && time_to_jurassic() ) Serial.println( "Error setting time to long long ago");
  
  setenv( "TZ" , ntpTimezone, 1);
  tzset();

  #ifdef ESP8266
  // set function to call when time is set
  // is called by NTP code when NTP is used
  settimeofday_cb(time_is_set);
  #endif
 
  ntp_refresh();  

  if( waitforsync )ntp_waitforsync();
 
}
/*----------------------------------------------------------------*/

void tellTime(){
  time_t now;
  now = time(nullptr);
  Serial.print(ctime(&now));
}
/*----------------------------------------------------------------*/
 
void setup() {
 Serial.begin(115200);
 
  Serial.printf("Connecting to %s ", ssid);
  #ifdef ESP8266
  WiFi.hostname="sntptest";
  #endif
  WiFi.begin(wifiSsid, wifiPassword);

#ifdef ESP32
  WiFi.setHostname("sntptest");
#endif

#ifdef ESP8266 
#ifdef LWIP_IPV6
  Serial.printf("IPV6 is enabled\n");
   // Use this loop instead to wait for an IPv6 routable address

  // addr->isLocal() (meaning "not routable on internet") is true with:
  // - IPV4 DHCP autoconfigured address 169.254.x.x
  //   (false for any other including 192.168./16 and 10./24 since NAT may be in the equation)
  // - IPV6 link-local addresses (fe80::/64)

  for (bool configured = false; !configured;) {
    for (auto addr : addrList)
      if ((configured = !addr.isLocal()
                        // && addr.isV6() // uncomment when IPv6 is mandatory
                        // && addr.ifnumber() == STATION_IF
          )) {
        break;
      }
    Serial.print('.');
    delay(500);
  }
#else
  Serial.printf("IPV6 is not enabled\n");

   for (int wifipoll = 0; WiFi.status() != WL_CONNECTED; ++wifipoll) {
      delay(500);
      Serial.print(".");
      if ( wifipoll == 10 ){
        WiFi.begin(wifiSsid, wifiPassword);
      }
   }
#endif
#endif

   for (int wifipoll = 0; WiFi.status() != WL_CONNECTED; ++wifipoll) {
      delay(500);
      Serial.print(".");
      if ( wifipoll == 10 ){
        WiFi.begin(wifiSsid, wifiPassword);
      }
   }
  
  Serial.println(" CONNECTED ");
  Serial.print("IP address = ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway IP address = ");
  Serial.println(WiFi.gatewayIP());
 
  ntp_setup( true );
}

void loop() {
  delay(2000);
  //ntp_refresh();

  tellTime();
  
  // put your main code here, to run repeatedly:

}
