ogre
====
A simple Ogre Battle clone written in C++ and SFML.  Learning as I go.

Objective
---------
Win by controlling all the towns or causing your opponent to spend all their money.  Don't run of your your money!

Strategy
--------
Units are squads of heroes.  Each hero has a level and some basic stats like HP, strength, defense, and speed.  As they fight, they'll level up and grow in power.  Units start with heroes of random strength.

Units come in different types like knight, monk, ranger, and wizard.  These each move at various speeds through the terrain, attempting to take the shortest route.

Terrain is generated randomly for each game.  There are always 3 types: water, plains, and mountains.  A simple diamond-square algorithm generates elevations and that 

Send your strong units to towns to keep them from your opponent and collect taxes.  Faster units may be good for snatching out of the way towns.  Every 100 steps, you must pay your units, but you also get to collect taxes from towns.  Don't run out of money!

Unit HP is displayed underneath a unit.  Getting close to an enemy unit will cause them to fight, wounding both but sending the loser backwards (this is how you can take over a town with an occupant).

Units have limited vision.  Even weak units can be helpful as scouts to see what the enemy is up to.

If you want to view a unit outside of combat, simply select it and then hit 'I'.  A display will pop up with each hero's hit points and experience just as in combat.

Deploy new units in your towns if you've got the extra funds and need more soldiers.  Your enemy will do the same as her units bounce from town to town.


Controls
--------
* Space Bar - Pause the game
* Left Click - Select unit/issue move command
* Right Click/D - Deploy new unit in a town (when paused)
* T - Build town with selected unit (consumes unit)
* I - Get information on the selected unit
* Esc - Quit

Screenshots
-----------
Here is the basic battlefield when you start the game, with some units and towns.  The typical battlefield is larger, this is just scaled down and has trimmed off an information panel in the upper left.

![Ogre Battlefield](resources/limited_vision.png?raw=true "Initial Battlefield")

This is a fight scene featuring individual heros when units get too close.  No art here yet, but you can see the HP and XP of each hero fighting it out.  As in Flatland, heroes with more sides are of higher status.

![Ogre Fight Scene](resources/fightscene.png?raw=true "Core Fight Scene")

Take care of your towns!  Just as units can suffer too much damage and die, a town takes damage every time it changes hands.  Weaker hands might be ruined by just one capture.  They'll slowly recover over time, so hold onto them when you can.

![Ogre Town Damage](resources/town_destruction.png?raw=true "Town Destruction")


Future Goals
------------
* Truly different hero types (rock paper scissors balanced in combat)
    * give them different shapes in combat
* Actually name the heroes and units!
* parallelize where appropriate (shortest path computation)
* Pause battle
* Menus and options to turn on/off various features?
