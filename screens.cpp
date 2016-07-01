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
 * Este archivo contendrá las clases que llevarán a cabo el desarrollo
 * del programa y la renderización del mismo en la pantalla TFT de Arduino.
 * Este sistema se organiza en clases derivadas de la clase Screen, de tal
 * manera que éstas podrán definir nuevos comportamientos para las diferentes señales
 * que reciban de las clases controladoras del juego, permitiendo así
 * la creación de fases diferentes e independientes entre sí. (ej pantalla
 * de menú, de calibración, de juego, etc...)
 */

#include "screens.hpp"

//ListItem
ListItem::~ListItem(){
	delete[] text;
}
ListItem::ListItem(Context* ctx, char* text, Rect rect, Point textPos, char left, char right, char up, char down, char textSize, bool stroke){
	this->ctx = ctx;
	this->text = text;
	this->rect = rect;
	this->left = left;
	this->right = right;
	this->up = up;
	this->down = down;
	this->textSize = textSize;
	this->txtPos = textPos;
	this->stroke = stroke;
}
ListItem* ListItem::createAt(Context* ctx, char* text,char textSize, Point point, char left, char right, char up, char down, bool stroke){
	Size sz = ctx->getTextSize(text, textSize);

	//textSize * 8 => (textSize * 4) * 2 -> aumentamos el tamaño del rectángulo para definir un margen con el rectángulo y el texto. Se multiplica por dos para tener en cuenta los ambos lados del rectángulo
	// -2 -> Se le resta el tamaño del borde horizontal (no se aplica al vertical)
	return ListItem::create(ctx, text, textSize,{point.x, point.y, sz.w + textSize * 8, sz.h + textSize * 8 - 2}, left, right, up, down, stroke);
}
ListItem* ListItem::create(Context* ctx, char* text, char textSize, Rect rect, char left, char right, char up, char down, bool stroke){
	Size sz = ctx->getTextSize(text, textSize);
	Point pos = CENTER(rect, sz);
	pos.y += 1; //Tener en cuenta el borde (Solo uno de ellos claro)
	return new ListItem(ctx, text, rect, pos, left, right, up, down, textSize, stroke);
}

void ListItem::render(ListItemState state){
	Color fore;
	Color back;
	Color stroke;
	switch(state){
	case Pressed: back = GREEN; stroke = GREEN; fore = BLACK; break;
	case Selected: back = WHITE; stroke = WHITE; fore = BLACK; break;
	case Unselected: back = BLACK; stroke = WHITE; fore = WHITE; break;
	}

	if (this->stroke){
		this->ctx->fillRect(this->rect, stroke, back);
	}else{
		this->ctx->fillRect(this->rect, back);
	}

	this->ctx->drawText(text, textSize, txtPos, fore);
}

//Screen
Screen::Screen(Context* ctx) {
	this->ctx = ctx;
}
Screen::~Screen(){}
void Screen::onInit(){}
void Screen::render(){}
void Screen::onEnd(){}
Context* Screen::getContext() {
	return ctx;
}

//ListScreen
ListScreen::ListScreen(Context* ctx) : Screen(ctx) {}
ListScreen::~ListScreen(){
	deleteItems();
}
void ListScreen::moveListCursor(Direction dir, bool startButtonPressed){

	ListItem* item = items[chosenItem];
	int newIndex;
	switch(dir){
	case Left:
		newIndex = item->left;
		break;
	case Right:
		newIndex = item->right;
		break;
	case Up:
		newIndex = item->up;
		break;
	case Down:
		newIndex = item->down;
		break;
	default: newIndex = -1; break;
	}

	if (newIndex != -1){
		item->render(Unselected);
		items[newIndex]->render(startButtonPressed ? Pressed : Selected);
		chosenItem = newIndex;
		itemPressed = startButtonPressed;
	}else if (startButtonPressed != itemPressed){
		item->render(startButtonPressed ? Pressed : Selected);
		itemPressed = startButtonPressed;
	}
}

void ListScreen::buildItemMatrix(){
	items = new ListItem*[itemCount];
}

void ListScreen::deleteItems(){
	for (int i = 0; i < itemCount; i++){
		delete items[i];
	}
	delete[] items;
}

void ListScreen::onInit(){
	chosenItem = defaultItem;
	for (int i = 0; i < itemCount; i++){
		if (i == defaultItem) items[i]->render(Selected);
		else items[i]->render(Unselected);
	}
}

void ListScreen::render(){
	this->moveListCursor(this->ctx->game->input->dir, this->ctx->game->input->currentStart);
}

void ListScreen::onEnd(){}

//SplashScreen
SplashScreen::SplashScreen(Context* ctx) : Screen(ctx) {}
void SplashScreen::onInit() {
	initmillis = millis();
	ctx->clear(BLACK);
	char* text = lstr(STR_APPTITLE);
	Size sz = ctx->getTextSize(text, 2);
	ctx->drawText(text, 2, {(ctx->width - sz.w) / 2 , (ctx->height - sz.h) / 2}, {255, 255, 255});
	delete[] text;
}

void SplashScreen::render() {
	if (millis() - initmillis >= 500){
		ctx->game->initScreen(new MainMenuScreen(ctx));
	}else delay(500);
}

void SplashScreen::onEnd() {}


//MainMenu
MainMenuScreen::MainMenuScreen(Context* ctx) : ListScreen(ctx) {}
void MainMenuScreen::onInit() {
	//Como el número de botones disminuye si el joystick está deshabilitado,
	//cambiamos el tamaño del título en función de ello.
#ifdef USE_JOYSTICK
	Rect titleRect = {0,0,ctx->width, (int)(ctx->height * 0.5)};
#else
	Rect titleRect = {0,0,ctx->width, (int)(ctx->height * 0.6)};
#endif

	char* title = lstr(STR_APPTITLE);
	Point txtPoint = CENTER(titleRect, ctx->getTextSize(title, 2));
	ctx->fillRect(titleRect, BLACK);
	ctx->drawText(title, 2, txtPoint, WHITE);
	ctx->fillRect({0,titleRect.h, ctx->width, ctx->height - titleRect.h}, BLACK);
	delete[] title;

	char* maxScorePrefix = lstr(STR_MENU_MAX_SCORE_PREFIX);
	int prefixLen = strlen(maxScorePrefix);

	char* scoreText = new char[prefixLen + ctx->getNumberLength(ctx->game->maxScore) + 1]();
	memcpy(scoreText, maxScorePrefix, prefixLen);
	ultoa(ctx->game->maxScore, scoreText + prefixLen, 10);
	Size scoreSize = ctx->getTextSize(scoreText, 1);
	ctx->drawText(scoreText, 1, {(ctx->width - scoreSize.w) / 2, txtPoint.y + 21}, AQUA);
	delete[] scoreText;
	delete[] maxScorePrefix;

#ifdef USE_JOYSTICK
	itemCount = 4;
#else
	itemCount = 3;
#endif

	buildItemMatrix();
	int buttonHeight = (ctx->height - titleRect.h) / itemCount;
	int buttonY = titleRect.h;

#ifdef USE_JOYSTICK
	items[0] = ListItem::create(ctx, lstr(STR_MENU_PLAY), 1, {0, buttonY, ctx->width, buttonHeight}, 3, 1, 3, 1, false);
	items[1] = ListItem::create(ctx, lstr(STR_MENU_CALIBRATE), 1, {0, buttonY + buttonHeight, ctx->width, buttonHeight}, 0, 2, 0, 2, false);
	items[2] = ListItem::create(ctx, lstr(STR_MENU_RESET_SCORE), 1, {0, buttonY + buttonHeight * 2, ctx->width, buttonHeight}, 1, 3, 1, 3, false);
	items[3] = ListItem::create(ctx, lstr(STR_MENU_REBOOT), 1, {0, buttonY + buttonHeight * 3, ctx->width, buttonHeight}, 2, 0, 2, 0, false);
#else
	items[0] = ListItem::create(ctx, lstr(STR_MENU_PLAY), 1, {0, buttonY, ctx->width, buttonHeight}, 2, 1, 2, 1, false);
	items[1] = ListItem::create(ctx, lstr(STR_MENU_RESET_SCORE), 1, {0, buttonY + buttonHeight, ctx->width, buttonHeight}, 0, 2, 0, 2, false);
	items[2] = ListItem::create(ctx, lstr(STR_MENU_REBOOT), 1, {0, buttonY + buttonHeight * 2, ctx->width, buttonHeight}, 1, 0, 1, 0, false);
#endif
	this->ListScreen::onInit();
}

void MainMenuScreen::render() {
	this->ListScreen::render();
	if (ctx->game->input->start){
		delay(100);

		switch(chosenItem){
		case 0: //Botón de inicio
			ctx->game->initScreen(new GameScreen(ctx));
			break;
		case 1:
#ifdef USE_JOYSTICK
			//Si el joystick está habilitado, este botón sirve para calibrarlo
			ctx->game->initScreen(new CalibrationScreen(ctx));
#else
			//Si no, este botón resetea la puntuasión máxima
			ctx->game->initScreen(new DeleteMaxScoreConfirmScreen(ctx));
#endif
			break;
		case 2:
#ifdef USE_JOYSTICK
			//Si el joystick está habilitado, este botón resetea la puntuasión máxima
			ctx->game->initScreen(new DeleteMaxScoreConfirmScreen(ctx));
			break;
#else
			//Si no, se reincia la placa. (no se pone ningún break y ale)
#endif
		case 3: //Reiniciar placa
			ctx->clear({0, 0, 0});
			char* title = lstr(STR_REBOOT_TITLE);
			char* subtext = lstr(STR_REBOOT_SUBTITLE);

			Size sz = ctx->getTextSize(title, 2);
			ctx->drawText(title, 2, {(ctx->width - sz.w) / 2 , (int)((ctx->height - sz.h) / 2.5)}, AQUA);


			sz = ctx->getTextSize(subtext, 1);
			ctx->drawText(subtext, 1, {(ctx->width - sz.w) / 2 , (int)((ctx->height - sz.h) / 1.5)}, AQUA);
			delay(750);
			ctx->clear({255, 255, 255});

			delete[] title;
			delete[] subtext;

			ctx->game->reset();
			break;
		}
	}else{
		delay(50);
	}
}
void MainMenuScreen::onEnd() {}

//CalibrationScreen
#ifdef USE_JOYSTICK
CalibrationScreen::CalibrationScreen(Context* ctx) : ListScreen(ctx) {}
void CalibrationScreen::onInit() {
	ctx->clear({0, 0, 0});
	Rect buttonRect = {0, ctx->height - 20, ctx->width, 20};

	int minCoord = ctx->width > (ctx->height - 20) ? (ctx->height - 20) : ctx->width; // -20, el botón
	Rect screenRect = {0,0,ctx->width, ctx->height};
	Size previewSize = {(int)(minCoord * 0.8), (int)(minCoord * 0.8)};
	Point previewPos = CENTER(screenRect, previewSize);
	previewPos.y -= 10; //Tener en cuenta el botón

	int halfPt = ptSize / 2;
	float xMargin = previewSize.w / 2.0f;
	float yMargin = previewSize.h / 2.0f;

	int i = 0;
	for (int x = 0; x < 3; x++){
		for (int y = 0; y < 3; y++){
			controlPoints[i] = {(int)(previewPos.x + xMargin * x) - halfPt, (int)(previewPos.y + yMargin * y) - halfPt, ptSize, ptSize};
			ctx->fillRect(controlPoints[i++], WHITE);
		}
	}
	center.x = previewPos.x + xMargin;
	center.y = previewPos.y + xMargin;
	maxValue = xMargin;

	itemCount = 1;
	buildItemMatrix();
	items[0] = ListItem::create(ctx, lstr(STR_CALIBRATE_NOW), 1, buttonRect, -1,-1,-1,-1, false);
	this->ListScreen::onInit();
}

void CalibrationScreen::render() {
	this->ListScreen::render();
	Input* cInput = ctx->game->input;

	if (cRawPos.x != cInput->x || cRawPos.y != cInput->y){
		ctx->fillRect(cPos, BLACK);

		for (int i = 0; i < 9; i++){
			Rect controlPoint = controlPoints[i];
			int diffx = (cPos.x - controlPoint.x);
			int diffy = (cPos.y - controlPoint.y);
			if (diffx * diffx + diffy * diffy < 2 * ptSize * ptSize){
				ctx->fillRect(controlPoint, WHITE);
			}
		}

		cPos = {(cInput->x * maxValue / XAxisMaxVal) + center.x - ptSize / 2,
				-(cInput->y * maxValue / YAxisMaxVal) + center.y - ptSize / 2, ptSize, ptSize};
		cRawPos.x = cInput->x;
		cRawPos.y = cInput->y;

		ctx->fillRect(cPos, RED);
	}

	if (cInput->start){ //Calibrar
		if (calibrated){
			ctx->game->initScreen(new MainMenuScreen(ctx));
		}else{
			ctx->game->calibrate(cInput->rawX, cInput->rawY);
			calibrated = true;

			//Cambiar botonsito
			ListItem* newBtn = ListItem::create(ctx, lstr(STR_CALIBRATE_RETURN), 1, items[0]->rect, -1, -1, -1, -1, false);
			delete items[0];
			items[0] = newBtn;
			items[0]->render(Selected);
		}
	}

	delay(25);
}
void CalibrationScreen::onEnd() {}
#endif

//DeleteMaxScoreConfirmScreen
DeleteMaxScoreConfirmScreen::DeleteMaxScoreConfirmScreen(Context* ctx) : ListScreen(ctx) {}
void DeleteMaxScoreConfirmScreen::onInit(){
	ctx->clear(BLACK);
	char* title = lstr(STR_RESET_TITLE);
	char** lines = new char*[6];
	lines[0] = lstr(STR_RESET_L0);
	lines[1] = lstr(STR_RESET_L1);
	lines[2] = lstr(STR_RESET_L2);
	lines[3] = lstr(STR_RESET_L3);
	lines[4] = lstr(STR_RESET_L4);
	lines[5] = lstr(STR_RESET_L5);

	Point titlePt = {5,5};
	ctx->drawText(title, 2, titlePt, RED);
	ctx->drawLines(lines, 6, 1, {5, titlePt.y + 25}, WHITE);

	delete[] title;
	for (int i = 0; i < 6; i++){
		delete lines[i];
	}
	delete[] lines;

	itemCount = 2;
	defaultItem = 1;
	buildItemMatrix();
	items[0] = ListItem::create(ctx, lstr(STR_YES), 1, {0, ctx->height - 16, ctx->width / 2, 16}, 1, 1, 1, 1, false);
	items[1] = ListItem::create(ctx, lstr(STR_NO), 1, {ctx->width / 2, ctx->height - 16, ctx->width / 2, 16}, 0, 0, 0, 0, false);
	this->ListScreen::onInit();

}
void DeleteMaxScoreConfirmScreen::render(){
	this->ListScreen::render();
	if (ctx->game->input->start){
		if (chosenItem == 0){
			ctx->game->resetMaxScore();
			ctx->clear(BLACK);
			char* text = lstr(STR_RESET_DONE);
			Size txtSz = ctx->getTextSize(text, 2);
			Rect tftRect = {0,0, ctx->width, ctx->height};
			ctx->drawText(text, 2, CENTER(tftRect, txtSz), RED);
			delete[] text;
			delay(700);
		}
		ctx->game->initScreen(new MainMenuScreen(ctx));
	}
	delay(50);
}
void DeleteMaxScoreConfirmScreen::onEnd(){}

//GameScreen
GameScreen::GameScreen(Context* ctx) : Screen(ctx) {}
GameScreen::~GameScreen(){
	delete[] itemMatrix;
	delete snakeBlocks;
	if (pauseScreen != null)
		delete pauseScreen;
}

void GameScreen::onInit() {
	//Calcular el rectángulo de juego, dejando 12 píxeles en la parte superior
	//para mostrar la puntuación y logrando que el tamaño de éste sea
	//divisible entre el tamaño del bloque de la serpiente
	gameRect = {0, 12, ctx->width, ctx->height - 12};

	//coord -2 => tener en cuenta los bordes del rectángulo!
	while ((gameRect.h - 2) % blockSize > 0){
		gameRect.y++;
		gameRect.h--;
	}

	while ((gameRect.w - 2) % blockSize > 0){
		gameRect.w--;
	}
	//Posicionar en el medio del eje X
	gameRect.x = (ctx->width - gameRect.w) / 2;


	//Calcular el número de bloques que caben en la pantalla
	horizontalBlockCount = (gameRect.w - 2) / blockSize;
	verticalBlockCount = (gameRect.h - 2) / blockSize;
	totalBlockCount = horizontalBlockCount * verticalBlockCount;

	//Inicializar serpiente
	snakeBlocks = new BufferedQueue<int>(totalBlockCount);
	//Iniciar matriz de objetos de la partida
	itemMatrix = new InGameItemType[totalBlockCount];
	memset(itemMatrix, 0, totalBlockCount);
	//Introducir 3 bloques iniciales
	for (int i = 0; i < 3; i++){
		itemMatrix[i] = SnakeObject;
		snakeBlocks->push(i);
	}
	this->regenCoin();
	this->fullRender();
}

void GameScreen::render() {
	if (pauseScreen != null){ //Juego pausado; delegar en la pantalla de pausa
		pauseScreen->render();
		return;
	}
	Input* input = ctx->game->input;

	if (input->currentDir != None && nextDir != direction_opposite(input->currentDir)
			&& lastDir != direction_opposite(input->currentDir)){
		nextDir = input->currentDir;
	}

	if (input->start){ //Pausar juego
		pauseGame();
		return;
	}

	//countdown:
	//valor		3	2	1	0	-1			-2
	//accion	3!	2!	1!	Go!	Borrar go!	Primer loop del juego
	if (countdown == -2){
		if (millis() - lastMillis > movementDelay){
			lastDir = nextDir;
			lastMillis = millis();
			int headIndex= *snakeBlocks->tail;

			switch(nextDir){
			case Left:
				if (headIndex % horizontalBlockCount == 0)
					headIndex = -1;
				else headIndex--;

				break;
			case Right:
				if (headIndex % horizontalBlockCount == horizontalBlockCount - 1)
					headIndex = -1;
				else headIndex++;
				break;
			case Up:
				if (headIndex / horizontalBlockCount == 0)
					headIndex = -1;
				else headIndex -= horizontalBlockCount;
				break;
			case Down:
				if (headIndex / horizontalBlockCount == verticalBlockCount - 1)
					headIndex = -1;
				else headIndex += horizontalBlockCount;
				break;
			case None:
				break;
			}
			if (headIndex == -1 || itemMatrix[headIndex] == SnakeObject){ //Setear itemAtCell tras estar seguros de que el nuevo objeto está dentro del recinto de juego!
				//Game over
				callGameOver(false);
				return;
			}else{
				snakeBlocks->push(headIndex);
				ctx->fillRect(rectForMatrixIndex(headIndex), GREEN);

				if (itemMatrix[headIndex] == Coin){
					score += 20 * snakeBlocks->size();
					//Resetear la coin
					regenCoin();

					//Disminuir el delay, para aumentar la velocidad del juego
					if ((movementDelay += movementDelta) < minMovementDelay){
						movementDelay = minMovementDelay;
					}
				}else{
					score += snakeBlocks->size();
					int tail = *snakeBlocks->pop();
					ctx->fillRect(rectForMatrixIndex(tail), BLACK);
					itemMatrix[tail] = Empty;
				}
				itemMatrix[headIndex] = SnakeObject;
				renderScoreValue();
				if ((int)snakeBlocks->size() == totalBlockCount){
					//Pantalla sin espacio (El jugador ha ganado (olé sus huevos))
					callGameOver(true);
				}
			}
		}

		if (millis() - coinLastTilt > 100){
			coinLastTilt = millis();
			if ((coinVisible = !coinVisible)){
				ctx->fillRect(coinPos, BLACK);
			}else{
				ctx->fillRect(coinPos, AQUA);
			}
		}
	}else{
		if (millis() - lastMillis > 1000){
			lastMillis = millis();
			ctx->fillRect(countdownRect, BLACK);
			char* title;
			switch(countdown--){
			case 3:
				title = lstr(STR_GAME_3);
				break;
			case 2:
				title = lstr(STR_GAME_2);
				break;
			case 1:
				title = lstr(STR_GAME_1);
				break;
			case 0:
				title = lstr(STR_GAME_GO);
				break;
			default:
				//El siguiente loop iniciará el movimiento de la serpiente,
				//asi que reseteamos esta variable aquí para evitar retrasos.
				lastMillis = 0;

				//Renderizar todo de nuevo, para evitar que la serpiente
				//u otros elementos salgan cortados despues de salir del
				//pausa
				fullRender(false, false, true);
				return;
			}
			Size sz = ctx->getTextSize(title, 3);
			Point pt = CENTER(gameRect, sz);
			countdownRect = {pt.x, pt.y, sz.w, sz.h};
			ctx->drawText(title, 3, pt, WHITE);
			delete[] title;
		}
	}
	delay(50);
}

void GameScreen::fullRender(bool clean, bool drawBorder, bool drawSnake) {
	if (clean)
		ctx->clear(BLACK);
	if (drawBorder)
		ctx->drawRect(gameRect, WHITE);
	if (drawSnake && snakeBlocks->size() > 0){
		for (unsigned int i = 0; i < snakeBlocks->size(); i++){
			ctx->fillRect(rectForMatrixIndex(*snakeBlocks->itemAtHeadOffset(i)), GREEN);
		}
	}

	char* scoreText = lstr(STR_GAME_SCORE);
	scoreRenderRect = {gameRect.x + 1, (gameRect.y - 10) / 2, 0, 0};
	ctx->drawText(scoreText, 1, {scoreRenderRect.x, scoreRenderRect.y}, WHITE);
	scoreRenderRect.x += ctx->getTextSize(scoreText, 1).w;
	delete[] scoreText;
	renderScoreValue();
}

void GameScreen::renderScoreValue(){
	ctx->fillRect(scoreRenderRect, BLACK);
	char* cscore = new char[ctx->getNumberLength(score) + 1]();
	ultoa(score, cscore, 10);

	Size sz = ctx->getTextSize(cscore, 1);
	scoreRenderRect.w = sz.w;
	scoreRenderRect.h = sz.h;

	ctx->drawText(cscore, 1, {scoreRenderRect.x, scoreRenderRect.y}, WHITE);

	delete[] cscore;
}

void GameScreen::regenCoin(){
	int randIndex = random(0, totalBlockCount - snakeBlocks->size());
	int c = 0;

	int cellx = 0;
	int celly = 0;
	int coinIndex = 0;

	/*
	 * Teniendo en cuenta que la moneda no puede aparecer sobre un bloque ocupado
	 * por una parte de la serpiente, en los momentos avanzados de la partida,
	 * cuando la serpiente es muy larga, un algoritmo que genere posiciones aleatorias
	 * puede producir retrasos en el juego debido al elevado número de posibilidades
	 * de que las coordenadas generadas puedan chocar con los bloques de la serpiente.
	 *
	 * Como alternativa se utilizará este método que, pese a que su complejidad es de O(n),
	 * donde n es el número de bloques máximo de la pantalla de juego, consigue buenos
	 * resultados en cuanto a rendimiento (En la Arduino Mega rev3 tarda < 1 ms).
	 */
	for (int i = 0; i < totalBlockCount; i++){
		if (itemMatrix[i] == Empty){
			if ((c++) == randIndex) {
				celly = i / horizontalBlockCount;
				cellx = i % horizontalBlockCount;
				coinIndex = i;
				break;
			}
		}
	}

	itemMatrix[coinIndex] = Coin;

	//2 => 1 + 1 => 1 pixel del borde + 1 pixel de diferencia con respecto a la posición obtenida,
	//para que éste siempre aparezca en un punto centrado respecto a los bloques de la serpiente
	coinPos = {cellx * blockSize + gameRect.x + 2, celly * blockSize + gameRect.y + 2, coinSize, coinSize};
}

void GameScreen::callGameOver(bool win){
	delay(500);
	if (!win){
		Rect* prev = null;
		for (unsigned int i = 0; i < snakeBlocks->size(); i++){
			Rect current = rectForMatrixIndex(*snakeBlocks->itemAtTailOffset(-i));
			if (prev != null){
				ctx->fillRect(*prev, DARK_RED);
			}
			ctx->fillRect(current, RED);
			delay(50);
			if (prev == null){
				prev = (Rect*)malloc(sizeof(Rect));
			}else{
				ctx->fillRect(*prev, BLACK);
			}
			memcpy(prev, &current, sizeof(Rect));
		}
		delay(50);
		ctx->fillRect(*prev, BLACK);
		free(prev);
		delay(500);
	}
	ctx->game->notifyScore(score);
	ctx->game->initScreen(new GameEndScreen(ctx, win, score));
}

void GameScreen::pauseGame(){
	if (pauseScreen != null)
		return;
	pauseScreen = new PauseScreen(this);
	pauseScreen->onInit();
}

void GameScreen::resumeGame(){
	if (pauseScreen == null)
		return;
	pauseScreen->onEnd();

	delete pauseScreen;
	pauseScreen = null;

	countdown = 3;
	fullRender(true, true, true);
}

int GameScreen::matrixIndexAtCoord(int x, int y){
	int ox = (x - gameRect.x + 1) / blockSize;
	int oy = (y - gameRect.y + 1) / blockSize;
	return oy * horizontalBlockCount + ox;
}
Point GameScreen::pointForMatrixIndex(int index){
	return {(index % horizontalBlockCount) * blockSize + gameRect.x + 1,
		(index / horizontalBlockCount) * blockSize + gameRect.y + 1};
}
Rect GameScreen::rectForMatrixIndex(int index){
	return {(index % horizontalBlockCount) * blockSize + gameRect.x + 1,
		(index / horizontalBlockCount) * blockSize + gameRect.y + 1,
		blockSize, blockSize};
}

void GameScreen::onEnd() {}

//PauseScreen
PauseScreen::PauseScreen(GameScreen* game) : ListScreen(game->getContext()) {
	this->game = game;
	itemCount = 2;
}
void PauseScreen::onInit() {
	initScreen(lstr(STR_GAME_PAUSE), lstr(STR_GAME_RESUME), lstr(STR_GAME_EXIT_GAME));
}

void PauseScreen::initScreen(char* title, char* button1, char* button2){
	Rect titleRect = {0,0,ctx->width, (int)(ctx->height * 0.7)};
	ctx->fillRect(titleRect, BLACK);
	ctx->drawText(title, 2, CENTER(titleRect, ctx->getTextSize(title, 2)), ORANGE);
	delete[] title;

	ctx->fillRect({0,titleRect.h, ctx->width, ctx->height - titleRect.h}, BLACK);
	itemCount = 2;
	buildItemMatrix();
	int buttonHeight = (ctx->height - titleRect.h) / itemCount;
	int buttonY = titleRect.h;
	items[0] = ListItem::create(ctx, button1, 1, {0, buttonY, ctx->width, buttonHeight}, 1,1,1,1, false);
	items[1] = ListItem::create(ctx, button2, 1, {0, buttonY + buttonHeight, ctx->width, buttonHeight},0,0,0,0, false);
	this->ListScreen::onInit();
}

void PauseScreen::render() {
	this->ListScreen::render();
	if (ctx->game->input->start){
		delay(100);

		switch(chosenItem){
		case 0:
			if (confirm){
				ctx->game->initScreen(new MainMenuScreen(ctx));
			}else{
				game->resumeGame();
			}
			break;
		case 1:
			deleteItems();

			if (confirm){
				confirm = false;
				defaultItem = 0;
				initScreen(lstr(STR_GAME_PAUSE), lstr(STR_GAME_RESUME), lstr(STR_GAME_EXIT_GAME));
			}else{
				confirm = true;
				defaultItem = 1;
				initScreen(lstr(STR_GAME_EXIT_CONFIRM), lstr(STR_YES), lstr(STR_NO));
			}
			break;
		}
	}
	delay(50);
}

void PauseScreen::onEnd() {}

//GameEndScreen
GameEndScreen::GameEndScreen(Context* ctx, bool win, unsigned long score) : ListScreen(ctx) {
	this->win = win;
	this->score = score;
}
void GameEndScreen::onInit() {
	char* title;
	Color titleColor;

	if (win) {
		title = lstr(STR_GAME_WIN);
		titleColor = GREEN;
	}else{
		title = lstr(STR_GAME_OVER);
		titleColor = RED;
	}

	Rect titleRect = {0,0,ctx->width, (int)(ctx->height * 0.7)};
	Size txtSize = ctx->getTextSize(title, 2);
	Point titleLoc = CENTER(titleRect, txtSize);

	ctx->fillRect(titleRect, BLACK);
	ctx->drawText(title, 2, titleLoc, titleColor);
	delete[] title;

	ctx->fillRect({0,titleRect.h, ctx->width, ctx->height - titleRect.h}, BLACK);

	Rect scoreRect = {0, titleLoc.y + txtSize.h, ctx->width, 0};
	scoreRect.h = titleRect.h - scoreRect.y;

	char* scorePrefix = lstr(STR_GAME_SCORE);
	int prefixLen = strlen(scorePrefix);
	char* scoreText = new char[prefixLen + ctx->getNumberLength(score) + 1]();
	memcpy(scoreText, scorePrefix, prefixLen);
	ultoa(score, scoreText + prefixLen, 10);
	txtSize = ctx->getTextSize(scoreText, 1);
	ctx->drawText(scoreText, 1, CENTER(scoreRect, txtSize), AQUA);
	delete[] scoreText;
	delete[] scorePrefix;

	delay(500);

	itemCount = 2;
	buildItemMatrix();
	int buttonHeight = (ctx->height - titleRect.h) / itemCount;
	int buttonY = titleRect.h;

	items[0] = ListItem::create(ctx, lstr(STR_GAME_RETRY), 1, {0, buttonY, ctx->width, buttonHeight}, 1,1,1,1, false);
	items[1] = ListItem::create(ctx, lstr(STR_GAME_BACK_MENU), 1, {0, buttonY + buttonHeight, ctx->width, buttonHeight},0,0,0,0, false);
	this->ListScreen::onInit();
}

void GameEndScreen::render() {
	this->ListScreen::render();
	if (ctx->game->input->start){
		delay(100);
		switch(chosenItem){
		case 0:
			ctx->game->initScreen(new GameScreen(ctx));
			break;
		case 1:
			ctx->game->initScreen(new MainMenuScreen(ctx));
			break;
		}
	}
	delay(50);
}

void GameEndScreen::onEnd() {}
