# Nexo MIDI

Superficie de control MIDI reconfigurable desarrollada sobre una placa
standalone con microcontrolador ATmega328P. El equipo integra doce pulsadores,
cuatro potenciometros, encoder con pulsador, pantalla TFT, memoria EEPROM y
alimentacion autonoma mediante bateria recargable.

El proyecto fue realizado como trabajo final de **Complementos de Analogica 2**
de la carrera **Ingenieria Electronica** de la **Universidad Nacional de San
Juan (UNSJ)**.

## Autores

- Marcelo Cuello — Registro 23646
- Leonardo Pezzini — Registro 25084

Docente responsable: Ernesto Accolti.

## Funciones principales

- Doce pulsadores configurables para mensajes `Note On` y `Note Off`.
- Cuatro potenciometros configurables para mensajes `Control Change`.
- Ocho posiciones de preset, con una sola configuracion creada inicialmente.
- Creacion, duplicacion, restablecimiento y borrado de presets.
- Edicion de notas, controladores y canales MIDI desde el propio dispositivo.
- Persistencia de configuraciones en EEPROM.
- Salida MIDI DIN a 31250 bit/s.
- Interfaz grafica sobre TFT ILI9163C de 128 x 128 pixeles.
- Alimentacion autonoma y medicion del nivel de bateria.

## Estado

La version de firmware documentada es **v1.2.1**. Fue validada sobre el equipo
real con un adaptador MIDI-USB inalambrico y una aplicacion musical movil. Se
comprobaron los doce pares Note On/Off, los cuatro Control Change, la edicion y
persistencia de presets, el encoder, la TFT y el indicador de bateria.

Las demostraciones de integracion con QLC+, OBS Studio y FL Studio se encuentran
en desarrollo y se incorporaran progresivamente en `docs/software/`.

## Estructura

```text
firmware/
  nexo_midi_controller/   Firmware principal v1.2.1
diagnostics/              Pruebas aisladas de hardware y MIDI
docs/
  hardware/               Arquitectura, pinout y alimentacion
  software/               Guias de integracion con aplicaciones
  report/                 Material base para el informe academico
```

## Hardware principal

- ATmega328P en placa standalone compatible con Arduino Uno.
- Multiplexor analogico CD74HC4067.
- 12 pulsadores y 4 potenciometros.
- Encoder incremental con pulsador.
- TFT SPI basada en ILI9163C.
- Salida MIDI DIN de 5 pines.
- Celda Li-ion/LiPo de 3,7 V.
- Cargador TP4056 con proteccion.
- Elevador MT3608 ajustado a 5,00 V.
- Divisor resistivo 10 kohm / 10 kohm para medir la bateria mediante A1.

## Bibliotecas

- `SPI` y `EEPROM`, incluidas con el nucleo Arduino AVR.
- `Adafruit GFX Library`.
- `TFT_ILI9163C`.
- `MIDI Library` de Forty Seven Effects.

## Carga del firmware

La placa final no incorpora conversor USB. Para programarla se extrae el
ATmega328P, se coloca temporalmente en un Arduino Uno y se carga:

```text
firmware/nexo_midi_controller/nexo_midi_controller.ino
```

Luego el microcontrolador se reinstala en la placa standalone respetando su
orientacion.

## Documentacion

- [Arquitectura del sistema](docs/hardware/arquitectura.md)
- [Mapa de pines y multiplexor](docs/hardware/pinout.md)
- [Sistema de alimentacion](docs/hardware/alimentacion.md)
- [Ensayos y validacion](docs/testing.md)
- [Demostraciones de software](docs/software/README.md)
- [Estructura propuesta del informe](docs/report/esquema-informe.md)
- [Historia, evolucion y alcance de MIDI](docs/report/historia-midi.md)
