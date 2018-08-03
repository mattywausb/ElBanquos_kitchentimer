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

     /* Give the Timer a new value to count down from, (will enable alert) */
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
     
     /* get the time left over. (will be negative when time is over ) */
     long getTimeLeft();

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

 
  protected:
     unsigned long reference_time;   // Reference time to get own position in time (0=on hold)  
     long original_interval; // the interval set originally 
     long tracked_interval; // the actual interval this timer is tracking (-1 = timer is off)
     bool alert_enabled; // State of the alert: enabled,  disabled
};

#endif
