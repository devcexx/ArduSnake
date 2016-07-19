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
 * Este archivo contiene las definiciones del preprocesador que podrán
 * ser utilizadas por el usuario para modificar el comportamiento del
 * programa, asi como la declaración de la clase "Game", que controla
 * el desarrollo del programa; y la de la clase "Context" que proporciona
 * métodos para el dibujo de elementos en la pantalla TFT.
 */

#ifndef snake_hpp
#define snake_hpp


/*
 * Si está definido, el Sketch comprenderá que el usuario utilizará
 * un joystick conectado a la placa para utilizar el programa.
 *
 * Si por otra parte se prefieren utilizar cuatro botones conectados
 * a la misma, la línea de abajo deberá de ser comentada.
 *
 */
#define USE_JOYSTICK

/*
 * NO MODIFICAR ESTA ENTRADA - Si este programa correrá en una Arduino Esplora,
 * el joystick debe de estar habilitado
 */
#ifdef ARDUINO_AVR_ESPLORA
#define USE_JOYSTICK
#endif

/*
 * NO MODIFICAR ESTA ENTRADA - Si el programa detecta que tiene está en una placa
 * diferente a la Arduino Esplora, y tiene el joystick habilitado, significa que
 * el programa utiliza un joystick externo, reconfigurando automáticamente el programa
 * para adaptarse a ello.
 */
#if defined(USE_JOYSTICK) && !defined(ARDUINO_AVR_ESPLORA)
#define EXTERN_JOYSTICK
#endif

/*
 * Si está definido, la comunicación serial se abrirá tras el inicio
 * del juego, para que el usuario pueda depurar o enviar información
 * al ordenador desde el programa.
 */
//#define BEGIN_SERIAL

/*
 * Si está definido, el programa enviará cada segundo la cantidad de
 * ram libre en la placa por el puerto serial. Esta opción puede
 * ser útil para buscar memory leaks en el programa. En el repositorio,
 * está opción está desactivada, pero puede ser activada descomentando
 * la línea de abajo.
 */
//#define DEBUG_MEMORY

#if defined(BEGIN_SERIAL) || defined(DEBUG_MEMORY)
/*
 * La velocidad en baudios del puerto serial, solo aplicable si
 * BEGIN_SERIAL o DEBUG_MEMORY están definidos.
 */
#define SERIAL_BAUD_RATE 57600
#endif

//Controles
#ifdef USE_JOYSTICK
//Controles - Joystick (no aplicable si el sketch corre en una Arduino Esplora)
#ifndef ARDUINO_AVR_ESPLORA
//El pin de entrada de los valores del eje X
#define X_AXIS_INPUT A1
//El pin de entrada de los valores del eje Y
#define Y_AXIS_INPUT A0
#endif

/*
 * La variación de los valores para cuando el joystick se mueve hacia
 * las diferentes direcciones El valor -1 indica que, hacia esa dirección,
 * los valores de entrada del joystick tienden a 0, mientras que 1
 * indica que tienden a 1023.
 *
 * En Arduino Esplora, estos valores, por lo general suelen estar definidos
 * a -1.
 */
#define X_AXIS_LEFT -1
#define Y_AXIS_UP -1

/*
 * Los centros de ambos ejes (esto después puede ser modificado por el
 * usuario durante el juego en el menú de Calibrar joystick).
 *
 * En un joystick externo, los centros pueden estar situados en (512, 512) o (0,0) por lo general.
 * En una Arduino Esplora, la ubicación de los centros está en el (0,0). En una Esplora, es importante
 * definir correctamente estos valores, pues, debido al limitado tamaño de la memoria de la Esplora,
 * la calibración estará deshabilitada.
 */
#define X_AXIS_CENTER 0
#define Y_AXIS_CENTER 0

/*
 * Los valores máximos que puede tomar cada uno de los valores (a menos
 * que se utilicen resistencias externas al joystick, este valor suele
 * ser la mitad de los posibles valores de las entradas analógicas,
 * es decir, 1024 / 2 = 512)
 */
#define X_AXIS_MAX_VALUE 512
#define Y_AXIS_MAX_VALUE 512

/*
 * Los valores mínimos que debe obtener cada eje del joystick para determinar a qué dirección
 * se está moviendo. Si los valores que recibe el joystick son menores que el especificado en
 * esta definición, el estado del Joystick pasa a ser "None", indicando así al programa que no
 * se está haciendo presión sobre el dispositivo (el valor debería de ser un entero entre 0 y
 * X_AXIS_MAX_VALUE o Y_AXIS_MAX_VALUE)
 */
#define JOY_DETECT_THRESHOLD 300

#else
#ifndef ARDUINO_AVR_ESPLORA
//Controles - botones de dirección (no aplicable si el sketch corre en una Arduino Esplora)
#define BUTTON_LEFT_PIN 3
#define BUTTON_RIGHT_PIN 4
#define BUTTON_UP_PIN 5
#define BUTTON_DOWN_PIN 6
#endif
#endif

//Controles - Botón de start (no aplicable si el sketch corre en una Arduino Esplora)
#ifndef ARDUINO_AVR_ESPLORA
#define BUTTON_START_PIN 2
#endif

/*
 * Pantalla
 * Estos son los valores de configuración de la pantalla. Para que una pantalla funcione
 * con este Sketch, es necesario que sea compatible con la librería TFT de Arduino.
 * Además, se recomienda que su resolución no sea menor a 160x128 px
 *
 * (no aplicable si el sketch corre en una Arduino Esplora, se configura automáticamente)
 */
#ifndef ARDUINO_AVR_ESPLORA
#define TFT_WIDTH 160     //Ancho
#define TFT_HEIGHT 128    //Alto
#define TFT_LCD 10    	  //Pin LCD
#define TFT_DC 9          //Pin D/C
#define TFT_RST 8         //Pin RESET
#endif

//EEPROM
//El desplazamiento desde el inicio de la EEPROM donde se comenzarán a guardar los datos del juego
#define EEPROM_SAVE_OFFSET 0

//Random
/*Un pin analógico desconectado, cuyo ruido se utilizará para inicializar el generador
 * de números aleatorios.
 *
 * Si el Sketch se ejecuta en una Arduino Esplora, el programa obtendrá una semilla aleatoria a partir
 * del ruido de los sensores de la placa.
*/
#ifndef ARDUINO_AVR_ESPLORA
#define RANDOM_ANALOG_PIN A2
#endif

//Colores
#define RED {255,0,0}
#define DARK_RED {128,0,0}
#define GREEN {0,255,0}
#define DARK_GREEN {0,128,0}
#define BLUE {0,0,255}
#define DARK_BLUE {0,0,128}
#define WHITE {255,255,255}
#define BLACK {0,0,0}
#define AQUA {0,255,255}
#define GREY {128,128,128}
#define LIGHT_GRAY {200,200,200}
#define ORANGE {255,140,0}

//Macros
#define CENTER(rect, sz) {rect.x + (rect.w - sz.w) / 2, rect.y + (rect.h - sz.h) / 2}

//Comprobación de definiciones
#ifdef USE_JOYSTICK
#if X_AXIS_LEFT != -1 && X_AXIS_LEFT != 1
#error "X_AXIS_LEFT must be 1 or -1"
#endif
#if Y_AXIS_UP != -1 && Y_AXIS_UP != 1
#error "Y_AXIS_UP must be 1 or -1"
#endif
#if JOY_DETECT_THRESHOLD >= X_AXIS_MAX_VALUE || JOY_DETECT_THRESHOLD >= Y_AXIS_MAX_VALUE
#warning "JOY_DETECT_THRESHOLD seems to be too high (possible joystick device malfunction)"
#endif
#endif

//Incluir las cabeceras necesarias después de las definiciones del preprocesador
#include "Arduino.h"
#include "screens.hpp"
#include "types.hpp"
#include <TFT.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#ifdef ARDUINO_AVR_ESPLORA
#include <Esplora.h>
#endif

/*
 * Aquí se declaran todas las strings del programa. Todas ellas se declaran
 * como PROGMEM, para garantizar que se guardarán en la memoria flash en vez
 * de en la RAM. Con esto nos ahorraremos una cantidad importante de bytes :P
 */
const char STR_APPTITLE[] PROGMEM = "ArduSnake!";
const char STR_YES[] PROGMEM = "Zi";
const char STR_NO[] PROGMEM = "No";

//Strings - Menu
const char STR_MENU_MAX_SCORE_PREFIX[] PROGMEM = "Puntuacion Max: ";
const char STR_MENU_PLAY[] PROGMEM = "Jugar";
#ifdef USE_JOYSTICK
const char STR_MENU_CALIBRATE[] PROGMEM = "Calibrar joystick";
#endif
const char STR_MENU_RESET_SCORE[] PROGMEM = "Resetear puntuacion";
const char STR_MENU_REBOOT[] PROGMEM = "Reiniciar";

//Strings - Reinicio
const char STR_REBOOT_TITLE[] PROGMEM = "Reiniciando!";
const char STR_REBOOT_SUBTITLE[] PROGMEM = "Ahora te vemos!";

//String - Calibrar
const char STR_CALIBRATE_NOW[] PROGMEM = "Calibrar";
const char STR_CALIBRATE_RETURN[] PROGMEM = "Volver";

//String - Reset Score
const char STR_RESET_TITLE[] PROGMEM = "Cuidado!";
const char STR_RESET_L0[] PROGMEM = "Esta operacion eliminara";
const char STR_RESET_L1[] PROGMEM = "la puntuacion maxima";
const char STR_RESET_L2[] PROGMEM = "que esta guardada en la";
const char STR_RESET_L3[] PROGMEM = "memoria de la placa.";
const char STR_RESET_L4[] PROGMEM = "";
const char STR_RESET_L5[] PROGMEM = "Deseas continuar?";
const char STR_RESET_DONE[] PROGMEM = "Borrado!";
//Strings - Juego
const char STR_GAME_3[] PROGMEM = "3";
const char STR_GAME_2[] PROGMEM = "2";
const char STR_GAME_1[] PROGMEM = "1";
const char STR_GAME_GO[] PROGMEM = "Go!";
const char STR_GAME_SCORE[] PROGMEM = "Puntos: ";
const char STR_GAME_PAUSE[] PROGMEM = "Pausa";
const char STR_GAME_RESUME[] PROGMEM = "Reanudar";
const char STR_GAME_EXIT_CONFIRM[] PROGMEM = "Seguro?";
const char STR_GAME_EXIT_GAME[] PROGMEM = "Salir del juego";
const char STR_GAME_WIN[] PROGMEM = "Has Ganado!";
const char STR_GAME_OVER[] PROGMEM = "Game Over";
const char STR_GAME_RETRY[] PROGMEM = "Reintentar";
const char STR_GAME_BACK_MENU[] PROGMEM = "Volver al menu";

/*
 * Devuelve la cadena cargada en memoria contenida en el puntero
 * a la memoria flash especificado. Este método asigna un bloque
 * de memoria en el heap para la cadena devuelta, por lo que debe
 * de ser correctamente borrado tras su creación.
 */
extern inline char* lstr(const char* ptr){
	int len = strlen_P(ptr) + 1;
	char* buffer = new char[len]();
	memcpy_P(buffer, ptr, len);
	return buffer;
}

//Clases
class Game;
class Screen;

/*
 * Contiene métodos que funcionan como un "Wrapper" para algunas funciones de la
 * librería de dibujo en las pantallas TFT incluída por Arduino, extendiendo
 * la funcionalidad de algunas de las mismas.
 */
class Context {
public:
	Context(Game* game, TFT* scr, int w, int h);
	TFT* screen;
	int width;
	int height;
	Game* game;

	//background
	void clear(Color c);
	//text
	int getNumberLength(long long n);
	void drawText(char* text, int size, Point p, Color color);
	void drawLines(char** text,int count, int size, Point p, Color color);
	void drawRect(Rect rect, Color color);
	void fillRect(Rect rect, Color color);
	void fillRect(Rect rect, Color stroke, Color fill);
	Size getTextSize(int length, int size);
	Size getTextSize(char* text, int size);
};

/*
 * Representa una clase que posee los métodos y funciones necesarios
 * para controlar la dinámica del programa durante toda su ejecución.
 */
class Game {
public:
	unsigned long maxScore = 0;

#ifdef USE_JOYSTICK
	int joyCenterX = X_AXIS_CENTER;
	int joyCenterY = Y_AXIS_CENTER;
#endif

	Screen* cscreen = null;
	Input* input;
	Context* context;

#ifdef EXTERN_JOYSTICK
	void calibrate(int centerX, int centerY);
#endif
	void notifyScore(unsigned long score);
	void resetMaxScore();

	void init();
	void tick();
	void (*reset)(void) = 0x0;

	Input* readInput();
	void initScreen(Screen* screen);

#ifdef DEBUG_MEMORY
private:
	unsigned long lastMemReport = 0;
	inline int freeRam () {
	  extern int __heap_start, *__brkval;
	  int v;
	  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
	}
#endif
};

#endif
