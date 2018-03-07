#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#include <XBee.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as client.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
char msg[50];
int numdata;
char inSerial[50];
int i=0;
boolean started=false;
int gsm_connected=0;

Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();




void setup()
{
     //Serial connection.
     Serial.begin(57600); //57600
       xbee.setSerial(Serial);
       
     Serial.println("GSM Shield testing.");
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(57600)) { //2400
          Serial.println("\nstatus=READY");
          started=true;
     } else Serial.println("\nstatus=IDLE");

     if(started) {
          //GPRS attach, put in order APN, username and password.
          //If no needed auth let them blank.
          if (inet.attachGPRS("epc.t-mobile.com", "", ""))// internet.wind
               Serial.println("status=ATTACHED");
          else Serial.println("status=ERROR");
          delay(1000);

          //Read IP address.
          gsm.SimpleWriteln("AT+CIFSR");
          delay(5000);
          //Read until serial buffer is empty.
          gsm.WhileSimpleRead();

          //TCP Client GET, send a GET request to the server and
          //save the reply.

         // numdata=inet.httpGET("api.thingspeak.com", 80, "/apps/thinghttp/send_request?api_key=AO5M7JNQOKLE8APZ&message={x:109,y:102,z:189}", msg, 50); //
          //numdata=inet.httpGET("api.pushingbox.com", 80, "/pushingbox?devid=v1F144A54D0DFAA3&angle1=10&angle2=23&angle3=88", msg, 50); //
          
         // numdata=inet.httpPOST("api.thingspeak.com", 80, "/apps/thinghttp/send_request?api_key=AP8E1PLZRULD25X8","message={x:552,y:102,z:189}", msg, 50);
          
          //numdata=inet.httpPOST("api.pushingbox.com", 80, "/pushingbox?devid=v30FF66B8882D936&status=open","", msg, 50);//api.pushingbox.com", 80, "/pushingbox?devid=v30FF66B8882D936&status=open","", msg, 50
          //Print the results.
          
          
         // Serial.println("\nNumber of data received:");
         // Serial.println(numdata);
         // Serial.println("\nData received:");
         // Serial.println(msg);
     }
};

void loop()
{
  

     XbeeHttpSet(); 
    //Read for new byte on serial hardware,
     //and write them on NewSoftSerial.
     serialhwread();
     //Read for new byte on NewSoftSerial.
     serialswread();
     
     HTTPSet();
    

}

void serialhwread()
{
     i=0;
     if (Serial.available() > 0) {
          while (Serial.available() > 0) {
               inSerial[i]=(Serial.read());
               delay(10);
               i++;
          }

          inSerial[i]='\0';
          if(!strcmp(inSerial,"/END")) {
               Serial.println("_");
               inSerial[0]=0x1a;
               inSerial[1]='\0';
               gsm.SimpleWriteln(inSerial);
          }
          //Send a saved AT command using serial port.
          if(!strcmp(inSerial,"TEST")) {
               Serial.println("SIGNAL QUALITY");
               gsm.SimpleWriteln("AT+CSQ");
          }
          //Read last message saved.
          if(!strcmp(inSerial,"MSG")) {
               Serial.println(msg);
          } else {
               Serial.println(inSerial);
               gsm.SimpleWriteln(inSerial);
          }
          inSerial[0]='\0';
     }
}

void serialswread()
{
     gsm.SimpleRead();
}

void XbeeHttpSet()
{
  Serial.println("Entered Xbee HTTP method");
  uint8_t data = 0;
  char parameters[50];
  
  xbee.readPacket(1000);

     //Serial.println("Yo read packet");
    if (xbee.getResponse().isAvailable()) {
      // got something
     Serial.println("yo ");
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                xbee.getResponse().getRx16Response(rx16);
        	//option = rx16.getOption();
        	data = rx16.getData(0);
               
                  Serial.print("Node Id ");
                  Serial.print(data);
                  //Serial.print(" is down");
                 // Serial.println(" jus test ");
               
                 /* Serial.print(rx16.getData(1));
                  Serial.print(rx16.getData(2));*/
                  Serial.println(" ");
                  uint8_t x_l=rx16.getData(1); 
                  uint8_t x_h=rx16.getData(2);
                  
                  uint16_t x = ((x_h << 8) | x_l);
                  Serial.print("Value in X Direction : ");
                  Serial.println(x);                  
                  uint8_t y_l=rx16.getData(3); 
                  uint8_t y_h=rx16.getData(4); 
                  uint16_t y = ((y_h << 8) | y_l);
                  Serial.print("Value in Y Direction : ");
                  Serial.println(y);  
// HTTPSet();
                  //numdata=inet.httpPOST("api.thingspeak.com", 80, "/apps/thinghttp/send_request?api_key=AP8E1PLZRULD25X8","message={x:"+x2, msg, 50);
                  gsm_connected=1;
                  sprintf(parameters,"message={x:%d,y:%d}",x,y);
                  Serial.println(parameters);
                  numdata=inet.httpPOST("api.thingspeak.com", 80, "/apps/thinghttp/send_request?api_key=AO5M7JNQOKLE8APZ",parameters, msg, 50);//api.pushingbox.com", 80, "/pushingbox?devid=v30FF66B8882D936&status=open","", msg, 50
                  //Print the results.
                  
               
                  Serial.println("\nNumber of data received:");
                  Serial.println(numdata);
                  Serial.println("\nData received:");
                  Serial.println(msg);
                  

                  }
            } 
       }     
}


void HTTPSet(){

     if(started) 
     {
//        if(gsm_connected==1)
 //        {   
           gsm_connected=0;
       //GPRS attach, put in order APN, username and password.
          //If no needed auth let them blank.
            if (inet.attachGPRS("epc.t-mobile.com", "", ""))// internet.wind
                 Serial.println("status=ATTACHED");
            else 
              Serial.println("status=ERROR");
            delay(1000);

            //Read IP address.
            gsm.SimpleWriteln("AT+CIFSR");
            delay(5000);
          //Read until serial buffer is empty.
            gsm.WhileSimpleRead();
       //   }
     }
}


