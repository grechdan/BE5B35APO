# Semestral project with MicroZed APO board

## Project : Game "SPACE REMAINDERS"

### Introduction

The aim of this semestral project was to understand how to work with MicroZed APO Kit and to implement several controlling functions. This program performs output on RGB diodes and LED line, output of the text at the LCD display, rotary knob input and UDP data connection. 

### Initialization

To start playing the game you need to initialize connection between PC and MicroZed APO Kit. 
	1. Run GtkTerm and set instructions:
		
		GtkMenu: Configuration -> Port:
 		Port:   /dev/ttyUSB0;  Baud rate: 115200; Parity: none;  Stopbit: 1
 		login: root, password: mzAPO35
	
	2. Open terminal in the folder with the game
	3. Run command:
	
		make run TARGET_IP=192.168.223.xxx

xxx – last three numbers of the board IP-address 

### Game instruction

The game starts in the main menu. To navigate in the menu use the RED knob. To switch between
tabs ROTATE the knob and to choose a tab PRESS on the RED knob.
To begin playing choose the tab ‘PLAY’
The game starts after the first shot. To fire a shot PRESS on the BLUE knob. To move the ship and
to aim ROTATE the BLUE knob.
In the main menu in the tab SETTINGS there is a possibility to change font size, game speed and
difficulty, use RED knob to switch between them. To produce changes ROTATE the GREEN knob.
To finish the game it is nessesary to PRESS the GREEN knob in the main menu.

