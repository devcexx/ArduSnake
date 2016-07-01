# ArduSnake
ArduSnake es la implementación del clásico juego de la serpiente para Arduino, programado en C++. Antes de cargar este Sketch en una placa, es recomendable leer este documento, pues contiene información acerca de como modificar la ubicación los pines y el hardware que utilizará el circuito.

### Hardware
Para correr este Sketch, es necesario disponer de una placa Arduino que posea, al menos, 2 KB de SRAM y 32 KB de memoria Flash. Todas las placas de Arduino que cumplan este requisito, **salvo la Arduino Esplora (cuyo soporte se añadirá en un futuro)**, deberían ser capaces de ejecutar sin problemas este Sketch. No obstante, solo ha sido probado en una Arduino UNO y una Arduino MEGA.

El programa incluye dos modos de funcionamiento, que puede ser cambiado por el usuario antes de subir el Sketch a la placa: control por joystick o por botones de dirección. Más abajo se mostrará la manera de alternar entre estos dos modos.

El hardware requerido para hacer funcionar el Sketch variará en función del modo preferido. Así, si se desea controlar la placa con un joystick, el hardware requerido será el siguiente:
  - Una pantalla TFT, compatible con la librería TFT estándar de Arduino, a ser posible de 160x128 px de resolución mínima.
  - Un joystick de dos ejes.
  - Un pulsador de membrana.
  - Una resistencia de 10kΩ.

Si por el contrario se prefiere el uso de botones de dirección, éste será el hardware requerido:
  - Una pantalla TFT, compatible con la librería TFT estándar de Arduino, a ser posible de 160x128 px de resolución mínima.
  - Cinco pulsadores de membrana.
  - Cinco resistencias de 10kΩ.

Podrá encontrar ejemplos de montaje del hardware en la sección de descargas de este repositorio: <https://bitbucket.org/devcexx/ardusnake/downloads>\
Estos efectivamente solo son ejemplos de montaje: el usuario puede modificar los pines de los botones y/o joystick a su gusto. No obstante, las conexiones de la pantalla suelen variar solo en función de la placa que estemos usando. Para más información acerca de cómo conectar una pantalla TFT a la placa, léase esta guía: <https://www.arduino.cc/en/Guide/TFTtoBoards>

### Preparación del Software

Éste Sketch viene preparado para que el usuario final pueda realizar cambios en la configuración del joystick, botones etc, a través de cláusulas ```#define``` del preprocesador. Todas estas definiciones se encuentran en la parte superior del archivo ```snake.hpp``` y se pueden dividir en varias secciones:

##### Funciones
Permiten al usuario configurar el funcionamiento de algunos aspectos de la placa. En esta sección aparecen tres cláusulas:
```
#define USE_JOYSTICK
#define BEGIN_SERIAL
#define DEBUG_MEMORY
```
La primera de ellas permite al usuario determinar si utilizará un joystick o botones de dirección para controlar el juego. Si está definida, se utilizará un joystick. Elimínela o coméntela si prefiere utilizar botones de dirección.

La segunda determina si el programa iniciará la comunicación serial tras el inicio del programa. En el archivo ```snake.hpp``` verá que esta línea está, por defecto, comentada. Eso significa que el Sketch no iniciará la comunicación serial. Descoméntela si desea utilizar la comunicación serial de la placa. Si es así, también deberá prestar atención a la línea ```#define SERIAL_BAUD_RATE```, que le permitirá establecer la velocidad, en baudios, del puerto serial.

La última definición, si está descomentada, provocará que la placa envíe la cantidad de memoria RAM libre en ella cada segundo por el puerto serial. De manera predeterminada, esta opción está desactivada.

##### Joystick

Si se ha configurado la placa para utilizar un joystick (es decir, ```#define USE_JOYSTICK``` está definido), debería echarle un vistazo a los parámetros del mismo. Los parámetros disponibles son los siguientes:

```
#define X_AXIS_INPUT A1
#define Y_AXIS_INPUT A0
#define X_AXIS_LEFT 1
#define Y_AXIS_UP 1
#define X_AXIS_CENTER 512
#define Y_AXIS_CENTER 512
#define X_AXIS_MAX_VALUE 512
#define Y_AXIS_MAX_VALUE 512
#define JOY_DETECT_THRESHOLD 300
```

```X_AXIS_INPUT```: La entrada analógica a través de la cual la placa recibirá los valores de movimiento en el eje X del joystick. Cámbiela según a qué pin haya conectado la entrada horizontal del joystick.

```Y_AXIS_INPUT```: La entrada analógica a través de la cual la placa recibirá los valores de movimiento en el eje Y del joystick. Cámbiela según a qué pin haya conectado la entrada vertical del joystick.

```X_AXIS_LEFT```: Indica como varía el valor de la entrada analógica del eje X a medida que el joystick se mueve hacia la izquierda. Si cuando el joystick se mueve hacia la izquierda, el valor de la entrada analógica tiende a cero, establezca el valor de la definición a -1. Si por el contrario los valores tienden a 1023, establézcalo a 1.

```Y_AXIS_UP```: Indica como varía el valor de la entrada analógica del eje Y a medida que el joystick se mueve hacia arriba. Si cuando el joystick se mueve hacia arriba, el valor de la entrada analógica tiende a cero, establezca el valor de la definición a -1. Si por el contrario los valores tienden a 1023, establézcalo a 1.

```X_AXIS_CENTER```: Indica al Sketch cuál es el valor de la entrada analógica del eje X para cuando el joystick está en la posición de reposo. Generalmente esta opción suele quedarse establecida a 512. No obstante, esta opción puede ser ignorada si el usuario ha calibrado el joystick en el menú "Calibrar joystick" y éstos datos aún están presentes en la EEPROM.

```X_AXIS_MAX_VALUE```: El valor máximo que puede el joystick obtener en el eje X.

```Y_AXIS_MAX_VALUE```: El valor máximo que puede el joystick obtener en el eje Y.

```JOY_DETECT_THRESHOLD```: El valor de un eje a partir del cual el programa comprenderá que el joystick se está moviendo en una de las cuatro direcciones (izquierda, derecha, arriba o abajo).

##### Botones de dirección

Si se ha configurado el programa para utilizar botones de dirección, se deberán repasar los siguientes parámetros:
```
#define BUTTON_LEFT_PIN 3
#define BUTTON_RIGHT_PIN 4
#define BUTTON_UP_PIN 5
#define BUTTON_DOWN_PIN 6
```

Cada uno de ellos determina en qué pin se encuentra cada uno de los botones de dirección. Deben modificarse en función de en qué pines el usuario conecte cada uno de los botones.

##### Botón "Start"

Es el botón central del programa, y el pin al que debe estar conectado viene definido por la línea ```#define BUTTON_START_PIN 2```, la cual por su puesto puede ser modificada para variar el pin al que éste botón debe estar conectado.

##### Pantalla TFT

Los parámetros que se deben atender para configurar la pantalla dentro del programa son los siguientes:
```
#define TFT_WIDTH 160
#define TFT_HEIGHT 128
#define TFT_LCD 10
#define TFT_DC 9
#define TFT_RST 8  
```

```TFT_WIDTH```: El ancho, en píxeles de la pantalla.\
```TFT_HEIGHT```: El alto, en píxeles de la pantalla.\
```TFT_LCD```: El pin de la placa en donde se encuentra conectado el pin "LCD CS" de la pantalla.\
```TFT_DC```: El pin de la placa en donde se encuentra conectado el pin "D/C" de la pantalla.\
```TFT_RST```: El pin de la placa en donde se encuentra conectado el pin "RESET" de la pantalla.\

##### Otros
Algunos otros parámetros que pueden ser de interés y que pueden ser modificados son:
```
#define EEPROM_SAVE_OFFSET 0
#define RANDOM_ANALOG_PIN A2
```

```EEPROM_SAVE_OFFSET```: Desplazamiento en bytes desde el inicio de la EEPROM donde se comenzarán a guardar los datos del juego (calibración y puntuación máxima).

```RANDOM_ANALOG_PIN```: Un pin analógico desconectado de la placa, cuyo ruido se utilizará para inicializar el generador de números aleatorios.

### Compilar el Sketch

Una vez el usuario ha llevado a cabo las modificaciones necesarias en los parámetros del programa para adaptarlo a sus necesidades y a su hardware, es hora de cargar el Sketch en la placa. Para ello simplemente se deberá abrir el archivo .ino en el Arduino IDE (que deberá estar en la misma carpeta que el resto de archivos de código), elegir la placa de destino y el puerto donde está ubicado y cargarlo en ella.

Se ha podido observar que la carga de este Sketch en las placas puede resultar un poco lenta, sobre todo en la Arduino UNO. En ésta, la carga del Sketch en la placa puede demorarse hasta 30 segundos.

### El juego

Una vez que el Sketch está cargado en la Arduino y ésta se reinicia, lo primero que se puede observar es una rápida pantalla de inicio seguida después por un menú principal. En él podemos encontrar tres opciones, o cuatro si está habilitado el uso del joystick. Podrá desplazarse a través de ellas utilizando el joystick o los botones de dirección, y seleccionarlas con el botón "Start".

La primera opción presente en el menú será la de "Jugar". Tras seleccionarla, el menú se borrará, dejando paso a la pantalla del juego. Tras ello, comenzará una cuenta atrás que nos indicará del inicio del mismo. Para poder controlar el juego, utilice el joystick o los botones de dirección para mover a la serpiente hacia arriba, abajo, izquierda o derecha, evitando colisionar con los bordes y con si misma. Además también en la pantalla se podrá observar en todo momento una moneda que parpadea, que producirá que, cada vez que la serpiente la toque, ésta aumente su tamaño un bloque. El objetivo del juego es lograr la máxima longitud posible de la serpiente y, como último reto, lograr que ésta ocupe todo el recinto de juego.

La segunda opción será la de "Calibrar joystick". Si el programa ha sido configurado para utilizar botones de dirección en vez de un joystick, ésta opción no se mostrará. Tras seleccionarla, se abrirá una pantalla en la que se podrá observar el movimiento del joystick y calibrarlo si se desea. En esta pantalla, deje el joystick en reposo y pulse el botón "Start" para calibrarlo. Una vez hecho esto mueva el joystick y verifique que el punto rojo se mueve a la par que éste. Una vez hecho esto, pulse de nuevo el botón "Start" para salir al menú principal. Si tras calibrarlo el punto rojo no se mueve igual que el joystick, éste no llega a los extremos, etc, verifique los parámetros del joystick que se exponen en la parte superior de este documento.

La siguiente opción será la de "Resetear puntuación". Con esto se borrará la máxima puntuación almacenada en la EEPROM, reseteándola a cero. Tras seleccionar esta opción, el programa pedirá confirmación antes de borrarla.

La última opción es la de "Reiniciar". Esta simplemente reiniciará el Sketch, al igual que lo haría presionar el botón "reset" adherido a la placa.

### Licencia

Este programa y todos los archivos de código que lo conforman están bajo la protección de la GNU General Public License. Para más información, véase el archivo ```LICENSE``` de este repositorio.

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)