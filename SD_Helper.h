#ifndef __SD_Helper_h
#define __SD_Helper_h

#include "utils.h"
#include <SD.h>

#include "KAmodTEM.h"
#include "TWI.h"

#include "OneWire.h"


const int chipSelect = 53;

unsigned int temperature = 0;

char fract[] = {'0','1','1','2','3','3','4','4','5','6','6','7','8','8','9','9'};
char str[8];

File logFile;

filePrinter logPrinter;

OneWire ds(10);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void initSD()
{
  
  DSerial::println("Beginning SD init");
  if(!SD.begin(chipSelect))
  {
    DSerial::println("Error initializing SD");
  }
  DSerial::println("Init finished");

  DSerial::println("Creating log file");

  if(SD.exists("log.txt"))
  {
    DSerial::println("File exists");
    SD.remove("log.txt");
  }
  else
  {
    DSerial::println("File doesn't exist, creating");
  }
  logFile = SD.open("log.txt", FILE_WRITE);

  logPrinter = filePrinter(&logFile);

  logFile.println("DATE,TIME,ALT,LON,LAT,TEMP[deg_C],TEMP_Inside[deg_C],Vbat[V]");
}

void exitSD()
{
  DSerial::println("Closing log file");
  logFile.close();
}

/*void setup() {
  // put your setup code here, to run once:
  KAmodTEM_Init();  // 
  KAmodTEM_WriteReg(0, CONFIGURATION, 0b01100000); // 12-bit resolution
  Serial.begin(9600);

  initSD();
}*/

void flushLog()
{
  logFile.print("\n");
  logFile.flush();
}

void initTEM()
{
  KAmodTEM_Init();
  KAmodTEM_WriteReg(0, CONFIGURATION, 0b01100000); // 12-bit resolution
}
//to-do: work out how temperature conversion works and change flushing
void TemToSD()
{
  temperature = KAmodTEM_ReadReg(0, TEMPERATURE);
  //logFile.print("Temp 1 : ");
  logFile.print(itoa(temperature >> 8, str, 10));
  logFile.print('.');
  temperature >>= 4 ;
  temperature &= 0x0F;
  logFile.print(fract[temperature]);
  logFile.print("0,");//Quick hack -- proper: fix precision
  //logFile.flush();
}

void internalTemToSD()//requires optimizing
{
  byte i;
  byte present = 0;
  byte type_s=0;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    DSerial::println("No more addresses.");
    //DSerial::println();
    ds.reset_search();
    delay(250);
    return;
  }
  

  if (OneWire::crc8(addr, 7) != addr[7]) {
      DSerial::println("CRC is not valid!");
      return;
  }

  ds.reset(); // rest 1-Wire
  ds.select(addr); // select DS18B20
  ds.write(0x4E); // write on scratchPad
  ds.write(0x00); // User byte 0 - Unused
  ds.write(0x00); // User byte 1 - Unused
  ds.write(0x1F); // set up en 9 bits (0x1F)
  delay(15);//delay to avoid corruption -- fix it later
 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(120);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  //logFile.print("Temp 2 : ");
  logFile.print(celsius);
  logFile.print(",");
  //logFile.flush();
}

void voltToSD()
{
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (10.0 / 1023.0);
  // print out the value you read:
  logFile.print(voltage);
  logFile.println(",");
}

void gpsToSD()
{
    //logFile.print("TIME: ");
    logPrinter.printDateTime(gps.date, gps.time);
     logFile.print(",");

    //logFile.print("ALTITUDE: ");
    logPrinter.printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
     logFile.print(",");

    //logFile.print("COORDINATES: ");
    logPrinter.printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    logFile.print(",");
    logPrinter.printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
     logFile.print(",");
}

#endif
