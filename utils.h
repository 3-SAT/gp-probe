#ifndef __UTILS_h
#define __UTILS_h

#include "TinyGPS++.h"
#include <SoftwareSerial.h>
#include <SD.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;

SoftwareSerial ss(RXPin, TXPin);

TinyGPSPlus gps;




// This custom version of delay() ensures that the gps object
// is being "fed". + send raw data to serial
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available()){
      char c = ss.read();
      gps.encode(c);
      Serial.print(c);//send raw data over serial
    }
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}


//Classes
class DSerial
{
public:
    
    static void enable()
    {
        enabled = 1;
    }
    static void disable()
    {
        enabled = 0;
    }
    static void println(const char *str)
    {
        if(!enabled)return;

        Serial.println(str);
    }
    static void print(const char *str)
    {}
    static void print(char c)
    {
        if(!enabled)return;

        Serial.print(c);
    }

private:
    static bool enabled;

};

bool DSerial::enabled = 0;

class filePrinter
{
  File *internalFile;

public:
  filePrinter(File *writeDestination)
  {
    internalFile = writeDestination;
  }
  filePrinter()
  {}
  void printFloat(float val, bool valid, int len, int prec)
  {
    if (!valid)
    {
      while (len-- > 1)
        internalFile->print('*');
      internalFile->print(' ');
    }
    else
    {
      internalFile->print(val, prec);
      int vi = abs((int)val);
      int flen = prec + (val < 0.0 ? 2 : 1); // . and -
      flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
      for (int i=flen; i<len; ++i)
        internalFile->print(' ');
      smartDelay(0);
    }
  }
  void printInt(unsigned long val, bool valid, int len)
  {
    char sz[32] = "*****************";
    if (valid)
      sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i=strlen(sz); i<len; ++i)
      sz[i] = ' ';
    if (len > 0) 
      sz[len-1] = ' ';
    internalFile->print(sz);
    smartDelay(0);
  }
  void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
  {
    if (!d.isValid())
    {
      internalFile->print(F("**********,"));
    }
    else
    {
      char sz[32];
      sprintf(sz, "%02d/%02d/%02d,", d.month(), d.day(), d.year());
      internalFile->print(sz);
    }
    
    if (!t.isValid())
    {
      internalFile->print(F("********"));
    }
    else
    {
      char sz[32];
      sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
      internalFile->print(sz);
    }
  
    //printInt(d.age(), d.isValid(), 5);//AGEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    smartDelay(0);
  }

};

#endif
