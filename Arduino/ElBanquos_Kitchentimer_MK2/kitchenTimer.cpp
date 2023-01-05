 #include "Arduino.h"
 #include "kitchenTimer.h"
 #include "mainSettings.h"
 
#ifdef  TRACE_ON
  //#define TRACE_KITCHENTIMER
#endif

#define ALERT_ENABLED 1
#define ALERT_ACTIVE 2
#define ALERT_DISABLED 0

/* ------------- Creation  -------------- */


KitchenTimer::KitchenTimer(void)
{  
       disable();
}

/* ------------- Operations  -------------- */

void KitchenTimer::disable(void)
{  
       reference_time=0;   
       original_interval=0; 
       tracked_interval=0; 
       alert_enabled=true; 
}

/* Give the Timer a new value to count down from, (will reset alert) */
void KitchenTimer::setInterval(long new_interval)
{
  original_interval=new_interval;
  tracked_interval=new_interval;
  if(reference_time!=0) reference_time=(millis()/1000)*1000+1000; // Froce value to 1 second bounds;
  alert_enabled=true;
  #ifdef TRACE_KITCHENTIMER
    Serial.print(F("KT::setInterval:"));Serial.println(new_interval);
  #endif
}

/* start the timer,  */
void KitchenTimer::startCounting()
{
  reference_time=millis();  
  #ifdef TRACE_KITCHENTIMER
    Serial.print(F("KT::startCounting:"));
    Serial.println(tracked_interval);
  #endif
}

/* stop the timer */
void KitchenTimer::stopCounting()
{
  tracked_interval=tracked_interval-(millis()-reference_time)/1000;
  reference_time=0;

  #ifdef TRACE_KITCHENTIMER
    Serial.print(F("KT::StopCounting:"));
    Serial.println(tracked_interval);
  #endif
}

/* aknowledge alert, and deactivate it */
void KitchenTimer::acknowledgeAlert() 
{
  if(hasAlert()) 
  {
    alert_enabled=false;
    #ifdef TRACE_KITCHENTIMER
      Serial.print(F("KT::acknowledgeAlert"));
      Serial.println(tracked_interval);
    #endif
  }
}

/* ------------- State information -------------- */

/* get the time left over. (will be negative when time is over, will be 0 if timer is off ) */
long KitchenTimer::getTimeLeft()
{
  if(tracked_interval==0) return 0;
  if(reference_time==0) return tracked_interval;
  return tracked_interval-(millis()-reference_time)/1000;
}


/* get the interval value from the last set call */
long KitchenTimer::getLastSetTime()
{
  return original_interval;
}

/* get the interval value from the last start call */
long KitchenTimer::getLastStartTime()
{
  return tracked_interval;
}

/* check if timer has something to do */
bool KitchenTimer::isRunning()
{
  return (tracked_interval!=0 && getTimeLeft()>0 && reference_time!=0);
}

/* check if timer in Hold mode */
bool KitchenTimer::isOnHold()
{
  return (tracked_interval!=0  && getTimeLeft()>0 && reference_time==0) ;
}

/* check if timer is over */
bool KitchenTimer::isOver()
{
  return (tracked_interval!=0 && getTimeLeft()<=0);
}

/* check if timer is disabled */
bool KitchenTimer::isDisabled()
{
  return (tracked_interval==0);
}

/* check if alert is active */
bool KitchenTimer::hasAlert()
{
  if(alert_enabled) return isOver();
  return false;
}


