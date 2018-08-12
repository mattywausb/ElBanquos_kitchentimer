# ElBanquos Kitchentimer
An Arduino based "multichannel" Timer, optimized for tracking times during food preparation

# Motivation
When preparing food I have to take track of multiple countdowns. Even though one could solve this by putting multiple cheap timers on the fridge door or use a smartphone app there are some benefits with an extra device.
* better control over the time entry (cheap timers normally just go up, higher settings are a pain, correcting after start is nearly impossible)
* no need to clean your hands very properly 
* Smartphone is free for checking recepies and solutions

When you can't buy it, you can build it, so this is what I implement:

* Track up to "n" Timers (normally 4, but this limit comes from the amount of buttons and displays to be used)
* Very easy 2 finger operations:
  * Set and start timer
  * Acknowledge alarm
  * Change timer (necessary when something irregular happened on the kitchen process)
  * put timer on hold
  * dismiss timer
* Timer Intervall
  * 1s - 2 day's
  * Stepping adapted to total interval (1s - 1 hour) 
* Intuitive and widly readable display of timer state
  * Off/Running/on Hold/Alarm/Time over
  
# What you will find in the repository
* Arduino sketch for the timer
* Fritzing sketch for the hardware used in the implementation
* Manuals
    * [English](docs/manual-en.md)

# How can you use it
* You can just build the timer by yourself using the same parts (check the fritzing sketch)
* The module architecture allows you to excahnge the elements of the interface
  * Display type
  * Kind of buttons input methods
  * sound generation

# Mode Details and manual 
* check the wiki
