# FL Studio — demostracion musical

## Objetivo

Validar el uso tradicional de MIDI controlando instrumentos virtuales desde los
doce pulsadores y asignando mensajes Control Change a parametros de sintesis.

## Configuracion ensayada

- Entrada MIDI: `MidiPort`.
- B1-B12: notas MIDI para disparar un conjunto de bateria en un MPC virtual.
- B1-B12: ejecucion de sintetizadores virtuales.
- Potenciometros: vinculacion a distintos parametros mediante la funcion
  `Link to controller` de FL Studio.

## Resultado

Los pulsadores dispararon correctamente las muestras de bateria y las notas de
los sintetizadores. Los potenciometros modificaron en tiempo real los
parametros vinculados mediante mensajes Control Change. Se grabo una breve
muestra de audio como comprobacion integral de interpretacion y control
continuo.

El ensayo confirma que Nexo MIDI puede utilizarse como superficie de
interpretacion y control dentro de una estacion de trabajo de audio digital.

## Convencion de octavas

FL Studio denomina `C5` a la nota MIDI 60. Desde la version 1.2.2, la pantalla
del controlador usa la misma convencion: MIDI 0 se muestra como `C0` y MIDI 60
como `C5`. Esta nomenclatura solo afecta al texto de la pantalla; los numeros
MIDI transmitidos no cambian.
