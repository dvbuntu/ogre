ogre
====
A simple Ogre Battle clone written in C++ and SFML.  Learning as I go.

Objective
---------
Win by controlling all the towns or causing your opponent to spend all their money.  Don't run of your your money!

Strategy
--------
Send your strong units to towns to keep them from your opponent and collect taxes.  Faster units may be good for snatching out of the way towns.  Every 100 steps, you must pay your units, but you also get to collect taxes from towns.  Don't run out of money!

Unit strength is listed next to the unit.  Getting close to an enemy unit will cause them to fight, wounding both but sending the loser backwards (this is how you can take over a town with an occupant).

Deploy new units in your towns if you've got the extra funds and need more soldiers.  Your enemy will do the same as her units bounce from town to town.

Controls
--------
* Space Bar - Pause the game
* Left Click - Select unit/issue move command
* Right Click - Deploy unit (when paused)
* D - Deploy unit (when paused)
* Esc - Quit

Future Goals
------------
* Actual images for things (maybe interpret terrain from image)
  ** Align the images a little better, and proper transparency
* Truly different hero types (rock paper scissors balanced in combat)
  ** give them different shapes in combat
* Actually name the heroes and units!
* parallelize where appropriate (shortest path computation)
