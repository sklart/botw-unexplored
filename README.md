# [Tears of the Kingdom Unexplored is now released](https://github.com/lud99/totk-unexplored)

# BotW Unexplored
Easily view what hasn't been discovered in your Breath of the Wild savefile, on your Nintendo Switch.

The korok seeds that haven't been collected and the locations that are left to visit are shown on a map, where it's easy to see koroks or locations you've missed. You can also see shrines, hinoxes, taluses and moldugas too. 

<img src="https://github.com/sklart/botw-unexplored/blob/master/map3_2.0.jpg?raw=true" width=600>

## Why?
There are other websites and tools to display and even edit your BotW savefile, but none of them run on your Switch. You would have to backup your savefile and then transfer it to a computer, which is unnecessarily complicated. That's why I created this homebrew app.

## Usage
Download `botw-unexplored.nro` from the [latest release](https://github.com/sklart/botw-unexplored/releases/latest), transfer it to your SD card, and launch it.

* Use the analog sticks or the touch screen to move around.
* Press X to open the legend. Use the touch screen or the D-pad to navigate it; the analog sticks do not work in this menu.
* Press ZL while the legend is open to switch between English, Russian, and Spanish. The selected language is saved to `sdmc:/switch/botw-unexplored/language.txt` and restored the next time the app starts.
* Tap a korok to view a guide. Press B to mark it as complete manually. This is useful while BotW is running, when the app cannot load the newest save. Manual marks are cleared after a save can be loaded again.

## Localization

The interface supports English, Russian, and Spanish. Russian location names are generated from the official Nintendo Switch English and Russian message archives. The generator and its instructions are in [`tools/`](tools/README.md); the extracted game archives are intentionally excluded from Git.

## Version History

### 1.0.0
* Added Russian localization, including korok guides and location names.
* Added English, Russian, and Spanish language switching with persisted selection.
* Generated all 187 location names from official Nintendo Switch English and Russian localization data.

### 2.0
* Added Shrines (+ DLC if it's present), Taluses, Hinoxes and Moldugas.
* Added a legend where you can toggle which of the collectibles you want to see.
* It's now possible to use the app while playing the game. This is achived by making backups of your savefile, which are then loaded while the game is running.
* Added guides for finding the koroks. The text and images are taken directly from Zeldadungeons interactive map. Simply tap a korok and a guide will pop up. Very helpful for certain korok seeds that are hard to find with only a location on a map.
* When a korok is selected, press B to manually mark it as complete. Very helpful if you're using the app while playing as the latest savedata can't be read. Otherwise you would have to remember all the koroks you've found during this play session, which could become difficult. The correct korok progress will be restored once the game has been closed.
* "Korok paths" have also been added to help finding some koroks. If you've used Zeldadungeons interactive map, then you know what they are.
* Added support for Master Mode. Press Y to toggle it.
* Drastically improved performance when a lot of objects are displayed. Can easily run at 60fps now.
* The app remebers your last camera position and zoom. This makes it easy to get back to the korok you looked at last time, if you for example switch between BotW and the app. (You can also press X in the Homebrew menu to star it for quicker access)
* Now tries to load the last used user so you won't have to use the dialog picker every time you start the app.
* Added more error messages and all logs are saved to a text file in case the app doesn't work
### 1.0
* Initial release

## Building
`switch-mesa`, `switch-glad`, `switch-freetype`, and `switch-glm` are required. Install them with:

```
pacman -S switch-mesa switch-glad switch-glm switch-freetype
```

Then run `make` to build the `.nro` file.

### Credits
Huge thanks to these kind people for making this project possible:

* https://github.com/marcrobledo/savegame-editors For most of the data and my primary inspiration.  
* https://github.com/MrCheeze/botw-waypoint-map For all the location data. 
* https://zeldadungeon.net for their amazing korok guides and images that i "borrowed" :) 
* https://github.com/d4mation/botw-unexplored-viewer For the savefile parsing

Extra thanks to d4mation for all their suggestions and issues on github and helping me figure out other things about BotW (you're awesome!)
