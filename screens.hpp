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
 * Este archivo contendrá las definición de las clases que llevarán a cabo el desarrollo
 * del programa y la renderización del mismo en la pantalla TFT de Arduino.
 * Este sistema se organiza en clases derivadas de la clase Screen, de tal
 * manera que éstas podrán definir nuevos comportamientos para las diferentes señales
 * que reciban de las clases controladoras del juego, permitiendo así
 * la creación de fases diferentes e independientes entre sí. (ej pantalla
 * de menú, de calibración, de juego, etc...)
 */

#ifndef screens_hpp
#define screens_hpp

#include "snake.hpp"
#include "types.hpp"

class Context; 
class Game;

/*
 * Representa un elemento de una pantalla que soporte la renderización de listas.
 */
class ListItem {
public:
	~ListItem();
	static ListItem* createAt(Context* ctx, char* text, char textSize, Point pochar, char left, char right, char up, char down, bool stroke = true);
	static ListItem* create(Context* ctx, char* text, char textSize, Rect rect, char left, char right, char up, char down, bool stroke = true);
	char* text;
	Rect rect;
	Point txtPos;
	Context* ctx;
	char left;
	char right;
	char up;
	char down;
	char textSize;
	bool stroke;
	void render(ListItemState state);
private:
	ListItem(Context* ctx, char* text, Rect rect, Point txtPos, char left, char right, char up, char down, char textSize, bool stroke);
};

/*
 * Superclase que representa una pantalla o fase dentro del programa.
 * Ésta clase define unos constructores y destructores básicos así
 * como una serie de métodos virtuales que todas las clases derivadas
 * de ésta deberían de invalidar.
 */
class Screen {
public:
	Screen(Context* ctx);
	virtual ~Screen();
	virtual void onInit();
	virtual void onEnd();
	virtual void render();
	Context* getContext();
protected:
	Context* ctx;
};

/*
 * Representa una pantalla capaz de administrar y renderizar elementos
 * seleccionables y clickables a través del hardware de entrada conectado
 * al Arduino
 */
class ListScreen : public Screen {
public:
	ListScreen(Context* ctx);
	~ListScreen();
	void moveListCursor(Direction dir, bool startButtonPressed);
	void buildItemMatrix();
	void onInit();
	void onEnd();
	void render();
	void deleteItems();
protected:
	ListItem** items;
	int defaultItem = 0;
	bool itemPressed = false;
	int itemCount;
	int chosenItem = 0;
};

/*
 * Clase derivada de la clase Screen capaz de renderizar la pantalla de
 * bienvenida del programa. Esta clase es la primera Screen cargada por el programa y,
 * por tanto, la que deberá pasar el control a la siguiente pantalla (el menú principal)
 */
class SplashScreen : public Screen {
public:
	SplashScreen(Context* ctx);
	void onInit();
	void onEnd();
	void render();
private:
	unsigned long initmillis;
};

/*
 * Clase derivada de la clase ListScreen capaz de renderizar el menú
 * principal del programa, incluyendo una lista de opciones seleccionables
 * y el título del programa en la zona superior de la pantalla.
 */
class MainMenuScreen : public ListScreen {
public:
	MainMenuScreen(Context* ctx);
	void onInit();
	void onEnd();
	void render();
};
/*
 * Clase derivada de la clase ListScreen capaz de renderizar una pantalla que
 * permite al usuario calibrar el centro del joystick.
 */

#ifdef EXTERN_JOYSTICK
class CalibrationScreen : public ListScreen {
public:
	CalibrationScreen(Context* ctx);
	void onInit();
	void onEnd();
	void render();

	bool calibrated = false;
	//Vista previa del calibrado
	Rect controlPoints[9];
	Rect cPos = {0,0,0,0};
	Point cRawPos = {-1,-1};
	Point center;

	int maxValue;
	const int ptSize = 4;
	const int YAxisMaxVal = Y_AXIS_MAX_VALUE;
	const int XAxisMaxVal = X_AXIS_MAX_VALUE;
};
#endif

/*
 * Clase derivada de la clase ListScreen capaz de renderizar un mensaje
 * de confirmación que aparecerá justo antes de tratar de borrar la
 * puntuación máxima.
 */
class DeleteMaxScoreConfirmScreen : public ListScreen {
public:
	DeleteMaxScoreConfirmScreen(Context* ctx);
	void onInit();
	void onEnd();
	void render();
};

/*
 * Clase derivada de Screen que controla la dinámica del juego en sí.
 */
class PauseScreen;

class GameScreen : public Screen {
public:
	GameScreen(Context* ctx);
	~GameScreen();
	void onInit();
	void onEnd();
	void render();
	void fullRender(bool clean = true, bool drawBorder = true, bool drawSnake = true);
	void renderScoreValue();
	void regenCoin();
	void callGameOver(bool win);
	void pauseGame();
	void resumeGame();

	int matrixIndexAtCoord(int x, int y);
	Point pointForMatrixIndex(int index);
	Rect rectForMatrixIndex(int index);

	//InGameItemType getItemAtCoord(int x, int y);
	//void setItemAtCoord(int x, int y, InGameItemType t);

	//Constantes
	const int blockSize = 8;
	const int coinSize = blockSize - 2;
	const int minMovementDelay = 50;
	const int maxMovementDelay = 500;
	const float movementDelta = -10.0f;

	//Tiempo (multiusos :P)
	unsigned long lastMillis = 0;

	//Área de juego
	Rect gameRect;
	int horizontalBlockCount;
	int verticalBlockCount;
	int totalBlockCount;

	InGameItemType* itemMatrix;

	//Inicio del juego
	int countdown = 3;
	Rect countdownRect;

	//Pausa
	PauseScreen* pauseScreen = null;

	//Moneda
	Rect coinPos;
	bool coinVisible = false;
	unsigned long coinLastTilt = 0;

	//Serpiente
	Direction nextDir = Right;
	Direction lastDir = Right;
	BufferedQueue<int>* snakeBlocks;

	float movementDelay = maxMovementDelay;

	//Puntuación
	unsigned long score = 0;
	Rect scoreRenderRect;
};
/*
 * Clase derivada de ListScreen que controla la pantalla de pausa
 * durante el juego.
 */
class PauseScreen : public ListScreen {
public:
	PauseScreen(GameScreen* game);
	void onInit();
	void onEnd();
	void render();

	bool confirm = false;

	void initScreen(char* title, char* button1, char* button2);

private:
	float ledPower = 0.0f;
	char ledDir = 1;
	GameScreen* game;
};

/*
 * Clase derivada de ListScreen que controla la pantalla de pausa
 * durante el juego.
 */
class GameEndScreen : public ListScreen {
public:
	GameEndScreen(Context* ctx, bool win, unsigned long score);
	void onInit();
	void onEnd();
	void render();
private:
	bool win;
	unsigned long score;
};

#endif
