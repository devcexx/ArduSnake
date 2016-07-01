/*
 * Copyright © 2016 Roberto Guillén
 *
 * This file is part of ArduSnake.
 *
 * ArduSnake is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArduSnake is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArduSnake.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Archivo principal del Sketch de Arduino. Este archivo simplemente
 * tendrá la función de crear el objeto de juego y delegar en él cada
 * loop de Arduino.
 */

#include "snake.hpp"
Game* game;
void setup()
{
	(game = new Game())->init();
}

void loop()
{
	game->tick();
}
