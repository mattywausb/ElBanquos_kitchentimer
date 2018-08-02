#ifndef KITCHENTIMER_h
#define KITCHENTIMER_h


/*
 * The following class treats all time values as seconds in a long (int was to short to even keep a day)
 * It keeps track of the time by comparing reference values to system time, so no polling or interrupt is necessary
 */


class KitchenTimer
{
  public:
    /**
     * Instansiate a kitchen timeer
     */
     KitchenTimer(void);

     /* Give the Timer a new value to count down from, (will reset alert) */
     void setInterval(long);

     /* get the time left over. (will be negative when time is over ) */
     long getTimeLeft();

     /* start the timer,  */
     void startCounting();

     /* stop the timer */
     void stopCounting();

     /* get the interval value from the last set call */
     void getLastSetTime();

     /* get the interval value from the last start call */
     void getLastStartTime();

     /* check if timer has something to do */
     bool isActive();

     /* check if timer is over */
     bool isOver();

     /* check if timer in Hold mode */
     bool isOnHold();


     /* check if alert is active */
     bool hasAlert();

     /* aknowledge alert, and deactivate it */
     void acknowledgeAlert();

  protected:
     unsigned long reference_time;   // Reference time to get own position in time (0=on hold)  
     long original_interval; // the interval set originally 
     long tracked_interval; // the actual interval this timer is tracking (-1 = timer is off)
     bool alert_enabled; // State of the alert: enabled,  disabled
};

#endif
