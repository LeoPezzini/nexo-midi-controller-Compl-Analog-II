# Ensayos y validacion

## Diagnostico de entradas

El sketch `diagnostics/hardware_test/hardware_test.ino` permitio observar los
dieciseis canales, identificar el orden real del multiplexor y ajustar el
encoder. Se confirmo que:

- Los doce pulsadores conmutan entre valores cercanos a 1023 y 0.
- Los cuatro potenciometros recorren el rango ADC.
- El encoder detecta ambos sentidos y un evento por pulsacion.
- El encoder entrega dos transiciones validas por paso mecanico.
- La actualizacion parcial de la TFT elimina el parpadeo visible.

## Validacion MIDI

El ensayo principal asigno B1-B12 a las notas 60-71 y P1-P4 a CC 20-23, todos
sobre el canal MIDI 1.

El primer adaptador MIDI-USB introdujo mensajes corruptos. Para separar una
falla de firmware de una falla del enlace se utilizo
`diagnostics/midi_link_test/midi_link_test.ino`, que transmite tramas constantes
sin leer entradas. Un segundo adaptador MIDI-USB inalambrico recibio la secuencia
sin errores, permitiendo localizar la falla en el primer adaptador.

Resultados finales:

- Doce mensajes Note On correctos.
- Doce mensajes Note Off correctos.
- Cuatro mensajes Control Change con recorrido 0-127.
- Canal MIDI configurable y persistente.
- Cambios de nota y CC conservados despues de retirar la alimentacion.
- Creacion, duplicacion, restablecimiento y borrado de presets verificados.
- Funcionamiento musical validado mediante telefono Android, OTG y aplicacion
  compatible con MIDI.

## Alimentacion

Se verifico el funcionamiento autonomo con bateria, TP4056 y MT3608 ajustado a
5,00 V. El divisor de A1 mostro aproximadamente la mitad de la tension medida
directamente sobre la celda, como corresponde a la relacion 10k/10k.

