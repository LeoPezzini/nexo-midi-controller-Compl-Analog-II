# OBS Studio - control audiovisual mediante MIDI

## Objetivo

Demostrar el uso del controlador en produccion audiovisual mediante cambios de
escena, control continuo de audio, mute y grabacion.

## Software utilizado

- OBS Studio 32.2.2.
- Complemento obs-midi-mg 3.1.5.
- Entrada MIDI `MidiPort`.

El complemento realiza la capa de asignacion en la computadora. No requiere
modificaciones en el firmware del controlador.

## Escenas de prueba

- `Principal`.
- `Escritorio`, con captura de pantalla o de una ventana.
- `Camara`, con la camara integrada de la notebook.

## Mapeo validado

| Control | Mensaje | Funcion en OBS |
|---|---|---|
| P1 | CC20 | Volumen de Mic/Aux |
| P2 | CC21 | Volumen de la captura de salida de audio |
| B1 | Nota 60 | Seleccionar escena Principal |
| B2 | Nota 61 | Seleccionar escena Escritorio |
| B3 | Nota 62 | Seleccionar escena Camara |
| B4 | Nota 63 | Alternar mute de Mic/Aux |
| B5 | Nota 64 | Iniciar o detener grabacion |

Para los potenciometros, el campo `Value` del mensaje se configuro con rango
0-127 y la accion `Sources > Volume` en formato porcentaje. Para los botones se
utilizo `Note On` con velocidad 1-127. Las acciones de mute y grabacion se
configuraron en modo alternado.

## Resultado

Se realizo una grabacion de prueba alternando las tres escenas. Los dos
potenciometros modificaron sus respectivos faders, B4 alterno el silenciamiento
y B5 inicio y detuvo la grabacion. Las asignaciones funcionaron tambien con OBS
minimizado, por lo que el controlador puede operar sin mantener la ventana del
programa en primer plano.
