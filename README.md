### Facultad de Ciencias Exactas, Físicas y Naturales - Universidad Nacional de Córdoba

# Sistemas de Computación: Modulos de Kernel

- `lsmod`: loadable kernel modules, muestra que modulos están disponibles para ser cargados.
- `insmod`: es un programa trivial que inserta un módulo en el kernel. 

## Modulos vs Programas

### Cómo empiezan y terminan los módulos
Un programa generalmente inicia con una función `main()`, ejecuta multiples instrucciones y finaliza una vez que las completa. Los modulos de kernel funcionan de una forma distinta. Un modulo de kernel siempre inicia ya sea con `init_module` o la funcion especificada en la llamada a `module_init`.Esta es la funcion de entrada para los modulos: le dice al kernel qué funcionalidad provee dicho modulo y prepara el kernel para ejecutar las funciones del módulo cuando éste las necesite. Una vez que lo hace, la funcion de entrada retorna y el modulo no hace nada hasta que el kernel necesita hacer algo con el código que ese modulo provee.

Todos los modulos finalizan ya sea llamando a `cleanup_module` o la funcion que especifican en la llamada a  `module_exit`. Esta es la función `exit()` de los modulos, deshace cualquier cosa que la funcion de entrada haya hecho y elimina las funcionalidades que la misma registró. 

### Funciones disponibles para los módulos
Cuando programamos usamos constantemente funciones que no definimos, como por ejemplo `printf()`, prevista por la librería estándar de C, libc. Las definiciones de dichas funciones no entran a nuestro programa hasta la etapa de linkeo, donde se asegura que el código (de la función `printf()`) está disponible, y arregla la llamada a la instrucción para apuntar a ese código.

Los módulos de kernel son distintos en esta etapa también. EN algunos ejemplos se utiliza la funcion `printk()`, pero no incluimos ninguna librería de entrada/salida. Esto es porque los modulos son archivos objeto cuyos símbolos se resuelven al hacer `insmod`. La definición de los símbolos proviene del kernel mismo; las únicas funciones externas que podemos utilizar son aquellas provistas por el kernel. 

Para conocer qué símbolos se exportaron a nuestro kernel podemos leer el archivo `/proc/kallsyms`.

## User Space vs Kernel Space
La CPU puede correr en distintos modos. Cada modo nos brinda diferentes niveles de libertad para hacer lo que queramos con el sistema. Unix utiliza 2 anillos para manejar estos "modos". El anillo mas alto (anillo 0, conocido como "supervisor mode" donde podemos hacer cualquier cosa) y el anillo mas bajo, denominado "usermode".
Cuando usamos una función como `printf()`, típicamente la usamos en modo usuario. La función realiza una o más llamadas a sistema (system calls) y cada una de estas llamadas a sistema se ejecutan en nombre de la función de la librería, pero para hacerlo deben estar en modo supervisor ya que son parte del kernel mismo. Una vez que la llamada a sistema completa su tarea, retorna y la ejecución se transfiere a user mode nuevamente.