# Historia, evolucion y alcance de MIDI

## El problema previo a la estandarizacion

A comienzos de la decada de 1980 los sintetizadores, secuenciadores y cajas de
ritmos utilizaban interfaces propietarias. Distintos fabricantes empleaban
tensiones, conectores y protocolos incompatibles. La integracion de un sistema
con equipos de marcas diferentes era costosa, limitada y, en algunos casos,
imposible sin interfaces especiales.

La necesidad no era transmitir audio digital, sino comunicar acciones
musicales: que nota se ejecutaba, con que intensidad, cuando debia comenzar una
secuencia o que parametro se modificaba. Esto permitia controlar varios
generadores de sonido sin duplicar teclados ni convertir la señal de audio.

## Nacimiento de MIDI

Entre 1981 y 1983, representantes de fabricantes estadounidenses y japoneses
trabajaron sobre una interfaz comun. Dave Smith, de Sequential Circuits, e
Ikutaro Kakehashi, de Roland, tuvieron un papel central, junto con ingenieros y
representantes de otras empresas. La demostracion publica de interoperabilidad
entre instrumentos de distintos fabricantes en 1983 mostro que el acuerdo era
tecnicamente viable.

El resultado fue MIDI 1.0: una interfaz serie asincronica de 31250 bit/s,
organizada en mensajes compactos y hasta dieciseis canales logicos por enlace.
Su publicacion como estandar abierto para los fabricantes fue decisiva para su
adopcion.

## MIDI no es audio

MIDI representa eventos. Un mensaje puede indicar `Note On`, `Note Off`, un
cambio continuo, la seleccion de un programa o informacion de sincronizacion.
El receptor decide como interpretar el mensaje. La nota 60 puede ejecutar un
piano virtual, disparar una muestra de percusion o activar una escena de luces.

Esta separacion entre gesto, mensaje y resultado explica la longevidad del
protocolo y constituye la idea principal de Nexo MIDI.

## Expansión dentro de la musica

La aplicacion inicial entre sintetizadores se amplio rapidamente a:

- secuenciadores por hardware y software;
- cajas de ritmos y samplers;
- superficies de mezcla;
- instrumentos virtuales y estaciones de audio digital;
- procesadores de efectos y pedaleras;
- sistemas de conmutacion de audio;
- partituras, automatizacion y control de espectaculos.

Los mensajes de reloj permiten que varios dispositivos compartan tempo. Un
secuenciador puede mantener sincronizadas una caja de ritmos, un arpegiador,
efectos dependientes del tempo y otros instrumentos. Por su parte, mensajes
programados en una pista pueden cambiar presets de sintetizadores, pedaleras o
sistemas de switcheo en momentos determinados de una cancion. En una puesta
automatizada, el interprete puede concentrarse en tocar mientras los cambios de
sonido se ejecutan de forma reproducible.

Conviene distinguir dos mecanismos:

- **MIDI Clock:** transmite tempo y pulsos relativos para sincronizacion musical.
- **MIDI Time Code:** representa una referencia temporal absoluta derivada de
  SMPTE, util para coordinar audio, video y produccion escenica.

## Aplicaciones fuera de la interpretacion musical

Al ser un sistema de eventos discretos y controles continuos, MIDI tambien se
utiliza como interfaz humana para:

- iluminacion y escenas DMX;
- realizacion audiovisual y streaming;
- edicion de imagen y video;
- visuales en tiempo real;
- automatizacion de presentaciones;
- instalaciones interactivas y arte digital.

En estos casos MIDI funciona como una capa de control. No reemplaza a DMX, OBS
ni al motor grafico: proporciona botones, faders, valores y sincronizacion que
el programa receptor mapea a sus propias funciones.

## Evolucion de los medios fisicos

El conector DIN de cinco pines se convirtio en el simbolo de MIDI, pero el
protocolo se adapto posteriormente a computadoras y dispositivos portatiles.
Hoy puede transportarse mediante USB, conectores TRS, Bluetooth, Ethernet y
redes inalambricas, manteniendo los conceptos musicales originales.

## MIDI 2.0

MIDI 2.0 extiende MIDI 1.0 sin invalidarlo. Incorpora mayor resolucion,
comunicacion bidireccional, negociacion de capacidades, perfiles y mecanismos
de intercambio de propiedades. La compatibilidad y la traduccion con MIDI 1.0
continuan siendo objetivos fundamentales.

La vigencia de MIDI 1.0 durante mas de cuatro decadas demuestra el valor de una
interfaz simple, interoperable y suficientemente abstracta para sobrevivir a
cambios profundos en instrumentos, computadoras y medios de transporte.

## Relacion con Nexo MIDI

Nexo MIDI materializa esos principios en un dispositivo autonomo:

- los pulsadores generan eventos discretos;
- los potenciometros generan valores continuos;
- los presets separan la disposicion fisica de la funcion asignada;
- la salida DIN conserva compatibilidad con instrumentos tradicionales;
- un adaptador MIDI-USB o inalambrico permite controlar software actual;
- QLC+, OBS Studio y FL Studio demuestran aplicaciones diferentes sin cambiar
  el hardware.

## Fuentes principales

- The MIDI Association, [MIDI History Chapter 6: MIDI Begins 1981-1983](https://midi.org/midi-history-chapter-6-midi-begins-1981-1983).
- The MIDI Association, [MIDI History](https://midi.org/midi-history).
- The MIDI Association, [MIDI 2.0](https://midi.org/midi-2-0).
- The MIDI Association, [Specifications](https://midi.org/specs).
- The MIDI Association, [Network MIDI 2.0 Overview](https://midi.org/network-midi-2-0-udp-overview).

