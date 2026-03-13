1. Organización del Proyecto
Dividimos el proyecto en dos partes para mantener el orden:
La Lógica (log_processor.c y .h): Es el motor universal. Contiene las matemáticas y estructuras para procesar texto rápidamente.
Los Ejecutores (main_linux.c y main_windows.c): Son los encargados de hablar con el sistema operativo para crear threads y abrir los archivos.

2. ¿Qué debíamos realizar?
El objetivo era construir un programa que leyera un archivo gigante de registros web de la forma más rápida posible usando múltiples hilos en paralelo. Al final, el programa debía encontrar tres cosas:
- Cuántas solicitudes hizo cada IP.
- Cuál fue la URL más visitada.
- Cuántos errores HTTP ocurrieron.

3. Funciones Clave del Analizador (log_processor)
Para lograr que fuera ultra rápido, usamos tablas hash. Las funciones principales hacen esto:
- parse_log_line: Toma una línea de texto y "recorta" solo lo que nos importa: la IP, la URL y el código de error.
- hash_function e insert_or_update: Trabajan juntas. Toman el texto como la IP, lo convierten en un número, y usan ese número para guardar y contar el dato casi instantáneamente.
- merge_stats: Como cada hilo cuenta por su lado para no estorbarse, esta función junta las tablas de todos los hilos en un solo resultado final.

4. ¿Qué hacen los main?
A grandes rasgos, los programas principales leen el tamaño del archivo y lo parten en partes iguales. Le entregan un pedazo a cada hilo para que trabajen al mismo tiempo.
Al terminar, usan un candado de seguridad (llamado Mutex) para que los hilos entreguen sus resultados ordenadamente sin borrar los datos del otro.
