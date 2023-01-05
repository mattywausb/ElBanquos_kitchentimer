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

     /* ---- Operations ---- */

     /* Give the Timer a new value in seconds to count down from, (will enable alert) */
     void setInterval(long);
     
     /* start the timer,  */
     void startCounting();

     /* stop the timer */
     void stopCounting();

     /* aknowledge alert, and deactivate it */
     void acknowledgeAlert();

     /* Switch the timer off */
     void disable();


     /* ---- State information ----*/
     
     /* get the time left over in seconds. (will be negative when time is over ) */
     long getTimeLeft();

     /* get the interval value in seconds from the last set call */
     long getLastSetTime();

     /* get the interval value in seconds from the last start call */
     long getLastStartTime();

     /* Operation state */
     bool isRunning();
     bool isOver();
     bool isOnHold();
     bool isDisabled();
     bool hasAlert();

 
  protected:
     unsigned long reference_time;   // Reference time to get own position in time in milliseconds(0=on hold)  
     long original_interval; // the interval set originally in seconds
     long tracked_interval; // the actual interval this timer is tracking in seconds (-1 = timer is off)
     bool alert_enabled; // State of the alert: enabled,  disabled
};

#endif
