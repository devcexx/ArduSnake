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
 * Este archivo contiene tipos comunes que pueden son utilizados para
 * diferentes aplicaciones dentro del Sketch.
 */

#ifndef types_hpp
#define types_hpp

//A mi el null me gusta con minúsculas :P
#define null 0

#include "Arduino.h"

/*
 * Define los posibles estados de un elemento seleccionable de
 * una pantalla.
 */
enum ListItemState : byte {
	Unselected = 0,
			Selected = 1,
			Pressed = 2
};

enum InGameItemType : byte {
	Empty = 0,
			SnakeObject = 1,
			Coin = 2
};

/*
 * Define las posibles direcciones que puede poseer el joystick
 */
enum Direction : byte {
	None = 0,
			Left = 1,
			Right = 2,
			Up = 3,
			Down = 4
};

/*
 * Obtiene la dirección opuesta a la dirección especificada.
 */
extern inline Direction direction_opposite(Direction d){
	switch (d){
	case Left:
		return Right;
	case Right:
		return Left;
	case Up:
		return Down;
	case Down:
		return Up;
	default:
		return None;
	}
}


/*
 * Representa el tamaño de un objeto en un espacio bidimensional
 */
typedef struct {
	int w;
	int h;
} Size;

/*
 * Representa un punto en un espacio bidimensional
 */
typedef struct {
	int x;
	int y;
} Point;

/*
 * Representa un rectángulo ubicado en un espacio bidimensional y definido
 * por su posición y su tamaño.
 */
typedef struct {
	int x;
	int y;
	int w;
	int h;
} Rect;

/*
 * Representa un color en formato RGB y utilizando 8 bits por canal
 */
typedef struct {
	byte r;
	byte g;
	byte b;

	long toRGB(){
		long l = r;
		l <<= 8;
		l |= g;
		l <<= 8;
		l |= b;
		return l;
	}
} Color;

/*
 * Representa el estado del hardware de entrada conectado al Arduino
 * en un instante determinado.
 */
typedef struct {
	//El estado previo del botón start
	bool prevstart = false;
	//El estado del botón start (Se setea a false en cada loop)
	bool start = false;
	//El estado físico del boton de start
	bool currentStart = false;
	//La coordenada X hacia la que se está moviendo el joystick
	int x = 0;
	//La coordenada Y hacia la que se está moviendo el joystick
	int y = 0;

	//El valor de la entrada analógica de la coordenada X del joystick
	int rawX = 0;
	//El valor de la entrada analógica de la coordenada Y del joystick
	int rawY = 0;
	//La última dirección a la que apuntaba el joystick
	Direction lastDir = None;
	//La dirección actual del joystick (se setea a None en cada loop)
	Direction dir = None;
	//La dirección física del joystick
	Direction currentDir = None;
} Input;

/*
 * Representa una cola FIFO (First In First Out) de valores con una capacidad
 * máxima definida. Esta cola organizará sus entradas en base a una matriz
 * ubicada en el heap y con un tamaño de n*s, donde n es la capacidad de la cola,
 * definida por el usuario en el constructor, y s, el tamaño del tipo de dato
 * que contendrá esta cola.
 *
 * La ventaja principal de este tipo de colas aparece cuando el tamaño máximo de la
 * cola es conocido. Esta implementación se diferencia de, por ejemplo, una cola
 * basada en nodos, en que no requiere de nodos, por lo que el tamaño de la cola
 * en memoria es aproximadamente lo que ocuparía una matriz con su misma capacidad,
 * que contuviese elementos del mismo tipo. Además, también tiene la ventaja de que
 * solo se solicita la creación de nuevos objetos en el heap una vez en el constructor,
 * por lo que se evita así la fragmentación de la memoria.
 *
 * Esta es una implementación parcial, por lo que solo posee las funciones básicas. No
 * obstante, se pueden realizar tareas más complejas fuera de la clase utilizando los
 * datos almacenados en la matriz subyacente a la clase.
 *
 * Todo el código de la clase está escrito en el archivo de header para que sea
 * más sencilla utilizar (al ser ésta clase una template, puede dar más por saco si
 * los métodos están escritos en un archivo aparte).
 */
template <class T>
class BufferedQueue {
public:
    T* buffer;
    T* head = null;
    T* tail = null;

    inline ~BufferedQueue(){
    	delete buffer;
    }
    inline BufferedQueue(size_t capacity){
        this->_capacity = capacity;
        this->_size = 0;
        this->buffer = (T*)malloc(sizeof(T) * capacity);
    }
    inline void push(T value){
        if (head == null){
            head = (T*)bufferStartAddr();
            *head = value;
        }else{
            if (tail == null){
                tail = itemAtHeadOffset(1);
            }else{
                tail = itemAtTailOffset(1);
            }
            *tail = value;
        }
        _size++;
    }
    inline T* peek(){
        if (_size == 0) return null;
        return head;
    }
    inline T* pop(){
        if (_size == 0) return null;
        T* n = head;
        head = itemAtHeadOffset(1);
        if (head == tail){
            tail = null;
        }
        if (--_size == 0){
            head = null;
        }
        return n;
    }

    T* itemAtHeadOffset(size_t offset){
    	return (T*)itemAddr(((unsigned int)head) + sizeof(T) * offset);
    }
    T* itemAtTailOffset(size_t offset){
    	if (tail == null){
    		return itemAtHeadOffset(offset);
    	}
        return (T*)itemAddr(((unsigned int)tail) + sizeof(T) * offset);
    }

    inline size_t size(){
        return _size;
    }
    inline size_t capacity(){
        return _capacity;
    }

private:
    size_t _capacity;
    size_t _size;

    inline unsigned int itemAddr(unsigned int i){
        if (i < bufferStartAddr())
            return (i + _capacity * sizeof(T));
        else if (i > bufferEndAddr())
            return (i - _capacity * sizeof(T));
        else return i;
    }
    inline unsigned int bufferStartAddr(){
         return (unsigned int)buffer;
    }
    inline unsigned int bufferEndAddr(){
        return bufferStartAddr() + (_capacity - 1) * sizeof(T);
    }
};


#endif
