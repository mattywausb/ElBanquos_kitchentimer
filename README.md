# ElBanquos Kitchentimer
An Arduino based "multichannel" Timer, optimized for tracking times during food preparation

# Motivation
When preparing food I have to keep track of multiple countdowns. Even though one could solve this by putting multiple cheap timers on the fridge door or use a smartphone app there are some benefits with an extra device.
* better control when setting the time (cheap timers normally just go up, higher settings are a pain, correcting after start is nearly impossible)
* no need to clean all your fingers to change a timer
* Smartphone normally switches of the screen, so you can't see the current state (or they keep screen on and drain smart phone battery for neraly nothing)
* Smartphone somtimes is needed to check recepies, problem solutions or communicating (is that so?). Again you cannot see the current state of the timers just by looking at the device

# Features
* Track up to 4 timers (Well... limited by the number of display and interaction elements, you can extend this, with little modification)
* Main display mode shows all timers (no need to push a button to see the timers)
* wide range of intervals (1 second to 6 days (and thats only a coded limit) )

## Easy and intuitive 2 Finger Operations
* Set and start timer
* Granularity of stepping adapts to total interval (there is normally no need to scroll over every minute, when the interval is more than an hour)
* Acknowledge alarm
* Change an alreay running timer (necessary when something irregular happened in the cooking process)
* put timer on hold
* dismiss timer
* check the inital time, the timer started from
* measure time when timer is over
  
# What you will find in the repository
* Arduino sketch for the timer
* Fritzing sketch for the hardware used in the implementation
* Manuals
    * [English](https://mattywausb.github.io/ElBanquos_kitchentimer/docs/manual-en.html)
    * [German](https://mattywausb.github.io/ElBanquos_kitchentimer/docs/manual-de.html)

# How can you use it
* You can just build the timer by yourself using the same parts (check the fritzing sketch)
* The module architecture allows you to excahnge the elements of the interface
  * Display type
  * Kind of buttons input methods
  * sound generation

# More Details 
* check the wiki
