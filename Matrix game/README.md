# Matrix game project
## Name of the game: Snake
## Description
The main purpose of the game is to make the snake eat as many food points as possible, represented by lighting pixels on the matrix. The snake can eat only one food point at the time, and can't go further without eating the current one. 
## Tehnical features
### Hardware
In this project, the hardware used are:
8x8 LED matrix - the game itself is displayed here
LCD - the game menu, that displays the score, the number of available lives, and some messages
Joystick - to control how the snake is moving
MAX7219 Driver - to connect all the matrix pins in a simpler way
LED - to be lighten up when the snake eats something

### Game specifications
The score is incremented each time the snake eats a food point. In the first level, the score is incremented by 1 each time the snake eats a food point. In the next level, the speed is increasing so it's harder and harder to eat the food points.
On level 3, the last level, the food points are changing its positions, at every 2 seconds. Having the points teleporting makes it even harder for the snake to catch them.
There is only way to lose one life: the snake eats himself, this means that the head of the snake meets one part of his body. The player has 3 lives.

### How to play
The goal of this game is to make the snake happier and bigger by helping him eat as many food points as possible. A food point is a lit point, randomly displayed on the matrix and it needs to be touched by the head of the snake in order to be eaten. As long as the snake gets bigger, it will be harder for him to eat the points without his head touching one part of his body. In case this happens, the game ends. The player has 3 lives. One life is lost if the snakes dies.

### The lcd menu
The lcd screen will display a game menu, with 3 available options that can be selected: Play, Settings, Highscore and Info.
Play- if selected, it will start the game
Highscore- the highscore is updated when the game is done
Settings- the player can set the starting difficulty of the game, or the contrast value for the lcd display
Info- contains the name of the game, the name of the creator, and the github link
While playing, the lcd display will show the current level, the score, updated when it needs, and the number of remaining lives.
When the game is over, the lcd display will show the following options: Play again(in order to play again the game), Home(it goes back to the main menu), Info(it shows the relevant information about the last game played: score/highscore, level).
