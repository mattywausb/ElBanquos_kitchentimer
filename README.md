# ElBanquos_kitchentimer
An Arduino based multitimer, optimized for tracking times during food preparation

# Motivation
When preparing food I have to take track of multiple countdowns. The following had been available:
* Have multiple "hardware" timers somewere (probably on the fridge door).
* Use an App on my smartphone (and alway clean your hands before touching it and after touchen it (ieeek))

Both Versions need some improvement, so I build my  optimal timer with the following features:

* Track up to "n" Timers (normally 4, but this limit comes from the amount of buttons and displays to be used)
* Very easy 2 finger handling of all operations:
  * Set and start timer
  * Aknowledge alarm
  * Change timer (necessary when something irregular happened on the kitchen process)
  * put timer on hold
* Timer Intervall
  * 1s - 2 day's
  * Stepping adapted to total interval (1s - 1 hour) 
* Intuitive display of timer state
  * Off/Running/on Hold/Alarm/Time over
  
