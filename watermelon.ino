#include <EEPROM.h>
#include <DS1307RTC.h>
#include "datastructures.h"


#define relay_count 8
#define relay_shedule_capacity 16
TimeInterval shedule[relay_count][relay_shedule_capacity];


TimePoint parce_time(String str)
{
  TimePoint res;
  res.hours = str.toInt();
  int sepa = str.indexOf(":");
  if (sepa != -1)
    res.minutes = str.substring(sepa + 1).toInt();
  return res;
}


void put_time_interval(String str)
{
  int sepa = str.indexOf("to");
  if (sepa == -1)
  {
    Serial.println("cannot find a `to` keyword");
    return;
  }

  int relay_index = str.substring(sepa + 2).toInt() - 1;
  if (!((relay_index >= 0) && (relay_index < relay_count)))
  {
    Serial.println("invalid relay index");
    return;
  }
  
  TimeInterval interval;
  str = str.substring(0, sepa);
  sepa = str.indexOf("-");
  if (sepa == -1)
  {
    Serial.println("cannot find `-` to split time interval parts");
    return;
  }
  interval.from = parce_time(str.substring(0, sepa));
  interval.to   = parce_time(str.substring(sepa + 1));
  
  if (!interval.is_correct())
  {
    Serial.println("time interval is incorrect");
    return;
  }

  int i;
  auto& relay_shedule = shedule[relay_index];
  
  if (relay_shedule[15].is_correct())
  {
    Serial.println("shedule for this relay is already owerflowed");
    return;
  }
  
  for (i = 0; i < relay_shedule_capacity; i++)
  {
    if ((!relay_shedule[i].is_correct()) || (relay_shedule[i].from.timestamp() >= interval.to.timestamp()))
      break;
  }

  if ((i > 0) && (relay_shedule[i-1].to.timestamp() > interval.from.timestamp()))
  {
    Serial.println("this time interval intersects with one another");
    return;
  }

  for (int j = 15; j > i; j--)
    relay_shedule[j] = relay_shedule[j-1];
  relay_shedule[i] = interval;
  
  EEPROM.put(0, shedule);
}


void pull_time_interval(String str)
{
  int sepa = str.indexOf("from");
  if (sepa == -1)
  {
    Serial.println("cannot find a `from` keyword");
    return;
  }

  int relay_index = str.substring(sepa + 4).toInt() - 1;
  if (!((relay_index >= 0) && (relay_index <= 7)))
  {
    Serial.println("invalid relay index");
    return;
  }

  TimeInterval interval;
  str = str.substring(0, sepa);
  sepa = str.indexOf("-");
  if (sepa == -1)
  {
    Serial.println("cannot find `-` to split time interval parts");
    return;
  }
  interval.from = parce_time(str.substring(0, sepa));
  interval.to   = parce_time(str.substring(sepa + 1));
  
  if (!interval.is_correct())
  {
    Serial.println("time interval is incorrect");
    return;
  }

  auto& relay_shedule = shedule[relay_index];
  for (int i = 0; i < relay_shedule_capacity; i++)
  {
    if (relay_shedule[i] == interval)
    {
      for (int j = i; j < relay_shedule_capacity - 1; j++)
          relay_shedule[j] = relay_shedule[j+1];
      relay_shedule[relay_shedule_capacity - 1] = TimeInterval();
      break;
    }
  }

  EEPROM.put(0, shedule);
}


void setup()
{
  // read saved
  EEPROM.get(0, shedule);
  
  // terminal
  Serial.begin(9600);

  // relay
  for (int i = 2; i < relay_count + 2; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  // rain detector
  pinMode(10, INPUT);
}


void loop()
{
  String cmd = Serial.readStringUntil('\n');
  if (cmd.startsWith("manual mode")) // >> manual mode
  {
    for (int i = 2; i < relay_count + 2; i++)
      digitalWrite(i, HIGH);
    
    while (true)
    {
      cmd = Serial.readStringUntil('\n');
      if (cmd.startsWith("exit manual mode")) // >> exit manual mode
        break;
      else if (cmd.startsWith("open")) // >> open 1, 2, 3
      {
        cmd = cmd.substring(4);
        int sepa;
        do
        {
          digitalWrite(cmd.toInt() + 1, LOW);
          sepa = cmd.indexOf(",");
          cmd = cmd.substring(sepa + 1);
        }
        while (sepa != -1);
      }
      else if (cmd.startsWith("close")) // >> close 1, 2, 3
      {
        cmd = cmd.substring(5);
        int sepa;
        do
        {
          digitalWrite(cmd.toInt() + 1, HIGH);
          sepa = cmd.indexOf(",");
          cmd = cmd.substring(sepa + 1);
        }
        while (sepa != -1);
      }
    }
  }

  if (cmd.startsWith("get shedule")) // >> get shedule
  {
    for (int i = 0; i < relay_count; i++)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      for (int j = 0; j < relay_shedule_capacity; j++)
      {
        if (!shedule[i][j].is_correct())
          break;
        else
        {
          TimeInterval intrvl = shedule[i][j];
          
          Serial.print(intrvl.from.hours);
          Serial.print(":");
          if (intrvl.from.minutes < 10)
            Serial.print("0");
          Serial.print(intrvl.from.minutes);

          Serial.print(" - ");

          Serial.print(intrvl.to.hours);
          Serial.print(":");
          if (intrvl.to.minutes < 10)
            Serial.print("0");
          Serial.print(intrvl.to.minutes);

          if (j < 15)
          {
            if (shedule[i][j+1].is_correct())
              Serial.print(", ");
          }       
        }
      }
      Serial.print('\n');
    }
  }

  if (cmd.startsWith("put")) // >> put 1:00 - 2:00 to 1
    put_time_interval(cmd.substring(4));

  if (cmd.startsWith("pull")) // >> pull 1:00 - 2:00 from 1
    pull_time_interval(cmd.substring(5));

  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet) 
  {
    Serial.println("Error: clock not found");
    return;
  }

  if (cmd.startsWith("set time to")) // >> set time to 11:00
  {
    int sepa;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    
    cmd = cmd.substring(11);
    hours = cmd.toInt();

    sepa = cmd.indexOf(":");
    if (sepa != -1)
    {
      cmd = cmd.substring(sepa + 1);
      minutes = cmd.toInt();
    }

    sepa = cmd.indexOf(":");
    if (sepa != -1)
    {
      cmd = cmd.substring(sepa + 1);
      seconds = cmd.toInt();
    }
    
    setTime(hours,minutes,seconds, 1,1,1971);
    RTC.set(now());
  }

  if (cmd.startsWith("get time")) // >> get time
  {
    Serial.print(hour());
    Serial.print(":");
    {
      int minutes = minute();
      if(minutes < 10)
        Serial.print('0');
      Serial.print(minutes);
    }
    Serial.print(":");
    {
      int seconds = second();
      if(seconds < 10)
        Serial.print('0');
      Serial.print(seconds);
    }
    Serial.print('\n');
  }

  if (digitalRead(10)) // if it's rainy
  {
    for (int i = 2; i < relay_count + 2; i++)
      digitalWrite(i, HIGH);
  }
  else
  {
    int curr_timestamp = (hour() * 60) + minute();
    for (int i = 0; i < relay_count; i++)
    {
      int relay_pin = i + 2;
      bool is_active = false;
      for (int j = 0; j < relay_shedule_capacity; j++)
      {
        if (!shedule[i][j].is_correct())
          break;

        is_active = (shedule[i][j].from.timestamp() <= curr_timestamp) && (shedule[i][j].to.timestamp() >= curr_timestamp);
        if (is_active)
          break;
      }
      digitalWrite(relay_pin, is_active ? LOW : HIGH);
    }
  }
}
