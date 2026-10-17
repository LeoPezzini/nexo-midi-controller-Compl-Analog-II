# QLC+ - control de iluminacion mediante MIDI

## Objetivo

Demostrar el uso del controlador fuera del ambito musical mediante una
luminaria RGB virtual, escenas de color, apagado y una secuencia temporizada.

## Version y arquitectura

- QLC+ 5.2.2.
- Universo 1: fixture `Generic RGB`, direccion inicial 1.
- Universo 2: entrada `MidiPort` con perfil `Generic MIDI`.
- Consola virtual: cuatro sliders y botones asociados a funciones.

Separar la entrada MIDI del universo de iluminacion evita que los canales
internos del perfil MIDI aparezcan como canales DMX del fixture.

## Mapeo validado

| Control | Mensaje | Funcion QLC+ |
|---|---|---|
| P1 | CC20 | Rojo |
| P2 | CC21 | Verde |
| P3 | CC22 | Azul |
| P4 | CC23 | Intensidad general |
| B1 | Nota 60 | Escena roja |
| B2 | Nota 61 | Escena verde |
| B3 | Nota 62 | Escena azul |
| B4 | Nota 63 | Escena blanca |
| B5 | Nota 64 | Stop All Functions con fade out de 2 s |
| B6 | Nota 65 | Secuencia cromatica temporizada |

## Procedimiento resumido

1. Agregar una luminaria `Generic RGB` al Universo 1.
2. Vincular `MidiPort` como entrada del Universo 2.
3. Crear sliders de nivel para rojo, verde, azul e intensidad.
4. Usar External Controls para aprender CC20 a CC23.
5. Crear escenas roja, verde, azul y blanca.
6. Asociar los botones MIDI a las escenas, al apagado y a la secuencia.
7. Salir del modo de edicion de la consola virtual para ejecutar las funciones.

## Resultado

Los cuatro potenciometros modificaron en tiempo real los componentes RGB y la
intensidad general. Los pulsadores activaron y desactivaron escenas, ejecutaron
un apagado con desvanecimiento y reprodujeron la secuencia configurada. La
luminaria virtual respondio correctamente sin cambios en el firmware.
