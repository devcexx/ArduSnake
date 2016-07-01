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
 * Este archivo contiene la implementación de la clase "Game", que controla
 * el desarrollo del programa; y la de la clase "Context" que proporciona
 * métodos para el dibujo de elementos en la pantalla TFT.
 */

#include "snake.hpp"

//Context
Context::Context(Game* game, TFT* scr, int w, int h) {
	this->game = game;
	this->screen = scr;
	this->width = w;
	this->height = h;

	screen->begin();
}

void Context::clear(Color c) {
	screen->background(c.r, c.g, c.b);
}

void Context::drawText(char* text, int size, Point p, Color color) {
	screen->stroke(color.r, color.g, color.b);
	screen->textSize(size);
	screen->text(text, p.x, p.y);
}

void Context::drawLines(char** lines, int count, int size, Point p, Color color) {
	screen->stroke(color.r, color.g, color.b);
	screen->textSize(size);
	int y = p.y;
	for (int i = 0; i < count; i++){
		screen->text(lines[i], p.x, y);
		y += 10 * size + size;
	}
}

void Context::drawRect(Rect rect, Color color){
	screen->stroke(color.r, color.g, color.b);
	screen->noFill();
	screen->rect(rect.x,rect.y,rect.w,rect.h);
}

void Context::fillRect(Rect rect, Color color){
	screen->noStroke();
	screen->fill(color.r,color.g,color.b);
	screen->rect(rect.x,rect.y,rect.w,rect.h);
}

void Context::fillRect(Rect rect, Color stroke, Color fill){
	screen->stroke(stroke.r, stroke.g, stroke.b);
	screen->fill(fill.r,fill.g,fill.b);
	screen->rect(rect.x,rect.y,rect.w,rect.h);
}

int Context::getNumberLength(long long n){
	if (n == 0) return 1;
	int l = n < 0 ? 1 : 0;
	n = abs(n);
	do{
		l++;
	}while((n /= 10) != 0);
	return l;
}

Size Context::getTextSize(int len, int size) {
	return {len * (5 * size) + (len - 1) * size, size * 10};
}
Size Context::getTextSize(char* text, int size) {
	return this->getTextSize(strlen(text), size);
}

//Game
//Algunas funciones útiles para la EEPROM
int eeprom_read_int(int offset){
	return ((int)EEPROM.read(offset)) << 8 | EEPROM.read(offset + 1);
}
long eeprom_read_long(int offset){
	long l = 0;
	for (int i = 0; i < 4; i++){
		l <<= 8;
		l |= EEPROM.read(offset + i);
	}
	return l;
}
void eeprom_write_int(int offset, int value){
	byte mostSignificantBits = (value >> 8);
	byte leastSignificantBits = (value & 0xFF);
	EEPROM.write(offset, mostSignificantBits);
	EEPROM.write(offset + 1, leastSignificantBits);
}
void eeprom_write_long(int offset, long value){
	EEPROM.write(offset, value >> 24);
	EEPROM.write(offset + 1, (value >> 16) & 0xFF);
	EEPROM.write(offset + 2, (value >> 8) & 0xFF);
	EEPROM.write(offset + 3, (value) & 0xFF);
}

void Game::init(){
	randomSeed(analogRead(RANDOM_ANALOG_PIN));

	context = new Context(this, new TFT(TFT_LCD, TFT_DC, TFT_RST), TFT_WIDTH, TFT_HEIGHT);
	input = new Input();

#ifdef USE_JOYSTICK
	pinMode(X_AXIS_INPUT, INPUT);
	pinMode(Y_AXIS_INPUT, INPUT);
#else
	pinMode(BUTTON_LEFT_PIN, INPUT);
	pinMode(BUTTON_RIGHT_PIN, INPUT);
	pinMode(BUTTON_UP_PIN, INPUT);
	pinMode(BUTTON_DOWN_PIN, INPUT);
#endif
	pinMode(BUTTON_START_PIN, INPUT);

#if defined(BEGIN_SERIAL) || defined(DEBUG_MEMORY)
	Serial.begin(SERIAL_BAUD_RATE);
#endif

	//Cargar datos de la EEPROM
#ifdef USE_JOYSTICK
	if (EEPROM.read(EEPROM_SAVE_OFFSET) == 1) { //Los datos de calibración están presentes en la EEPROM
		joyCenterX = eeprom_read_int(EEPROM_SAVE_OFFSET + 1);
		joyCenterY =  eeprom_read_int(EEPROM_SAVE_OFFSET + 3);
	}
#endif

	if (EEPROM.read(EEPROM_SAVE_OFFSET + 5) == 1){ //La score está presente en la EEPROM
		maxScore = (unsigned long)eeprom_read_long(EEPROM_SAVE_OFFSET + 6);
	}else{
		maxScore = 0;
	}
	initScreen(new SplashScreen(context));
}

#ifdef USE_JOYSTICK
void Game::calibrate(int centerX, int centerY){
	joyCenterX = centerX;
	joyCenterY = centerY;

	EEPROM.write(EEPROM_SAVE_OFFSET, 1); //Establece que los datos de calibración están presentes en la EEPROM
	eeprom_write_int(EEPROM_SAVE_OFFSET + 1, centerX);
	eeprom_write_int(EEPROM_SAVE_OFFSET + 3, centerY);
}
#endif

void Game::notifyScore(unsigned long score){
	if (maxScore < score){
		EEPROM.write(EEPROM_SAVE_OFFSET + 5, 1); //Establece que hay una puntuación guardada en la EEPROM
		eeprom_write_long(EEPROM_SAVE_OFFSET + 6, score);
		maxScore = score;
	}
}
void Game::resetMaxScore(){
	EEPROM.write(EEPROM_SAVE_OFFSET + 5, 0);
	maxScore = 0;
}

void Game::initScreen(Screen* scr) {
	if (cscreen != null) {
		cscreen->onEnd();
		delete cscreen;
	}
	cscreen = scr;
	cscreen->onInit();
}

void Game::tick(){
	readInput();
	if (cscreen != null){
		cscreen->render();
	}

#ifdef DEBUG_MEMORY
	if (millis() - lastMemReport > 1000){
		lastMemReport = millis();
		Serial.print("Available memory on board (bytes): ");
		Serial.println(freeRam());
	}
#endif
}


Input* Game::readInput() {
	bool btnstate = digitalRead(BUTTON_START_PIN);
	input->currentStart = btnstate;
	if (btnstate) { //Si el botón está presionado
		if (input->prevstart) {
			//Si ya estaba presionado, y ya ha sido recibido por el loop principal, establecer que no está presionado hasta que se levante y se vuelva a pulsar
			input->start = false;
		} else {
			//Si no estaba presionado, establecemos en true el estado del botón para que el loop principal pueda actuar ante ello.
			input->start = true;
			input->prevstart = true;
		}
	} else {
		//Si el botón no está presionado, establecemos todo en false
		input->prevstart = false;
		input->start = false;
	}

#ifdef USE_JOYSTICK
	input->rawX = analogRead(X_AXIS_INPUT);
	input->rawY = analogRead(Y_AXIS_INPUT);
	input->x = (input->rawX - joyCenterX) * X_AXIS_LEFT;
	input->y = (input->rawY - joyCenterY) * Y_AXIS_UP;

	if (input->x > X_AXIS_MAX_VALUE){
		input->x = X_AXIS_MAX_VALUE;
	}else if (input->x < -X_AXIS_MAX_VALUE){
		input->x = -X_AXIS_MAX_VALUE;
	}

	if (input->y > Y_AXIS_MAX_VALUE){
		input->y = Y_AXIS_MAX_VALUE;
	}else if (input->y < -Y_AXIS_MAX_VALUE){
		input->y = -Y_AXIS_MAX_VALUE;
	}

	int xabs = abs(input->x);
	int yabs = abs(input->y);
	if (xabs > yabs && xabs >= JOY_DETECT_THRESHOLD) {
		if (input->x > 0) input->currentDir = Right; else input->currentDir = Left;
	} else if (yabs > xabs && yabs > JOY_DETECT_THRESHOLD) {
		if (input->y > 0) input->currentDir = Up; else input->currentDir = Down;
	} else {
		input->currentDir = None;
	}
#else
	if (digitalRead(BUTTON_LEFT_PIN) == HIGH){
		input->x = -512;
		input->y = 0;
		input->currentDir = Left;
	}else if (digitalRead(BUTTON_RIGHT_PIN) == HIGH){
		input->x = 512;
		input->y = 0;
		input->currentDir = Right;
	}else if (digitalRead(BUTTON_UP_PIN) == HIGH){
		input->x = 0;
		input->y = 512;
		input->currentDir = Up;
	}else if (digitalRead(BUTTON_DOWN_PIN) == HIGH){
		input->x = 0;
		input->y = -512;
		input->currentDir = Down;
	}else{
		input->x = 0;
		input->y = 0;
		input->currentDir = None;
	}
	input->rawX = input->x;
	input->rawY = input->y;
#endif

	if (input->lastDir == input->currentDir){
		input->dir = None;
	}else{
		input->dir = input->currentDir;
		input->lastDir = input->currentDir;
	}

	return input;
}


