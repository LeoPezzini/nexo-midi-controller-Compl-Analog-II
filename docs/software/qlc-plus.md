# QLC+ — control de iluminacion mediante MIDI

## Objetivo

Controlar una luminaria RGB virtual con los cuatro potenciometros y utilizar los
pulsadores para recuperar escenas.

## Mapeo propuesto

| Control | Funcion QLC+ |
|---|---|
| P1 / CC20 | Rojo |
| P2 / CC21 | Verde |
| P3 / CC22 | Azul |
| P4 / CC23 | Intensidad general |
| B1-B4 | Escenas de color |
| B5 | Blanco |
| B6 | Blackout |

## Arquitectura recomendada

- Universo 1: fixture `Generic RGB`, direccion inicial 1.
- Universo 2: entrada `MidiPort` con perfil MIDI del controlador.
- Consola virtual: cuatro sliders y botones asociados a escenas.

Separar la entrada MIDI del universo de iluminacion evita que los canales
internos del perfil MIDI aparezcan como si fueran canales DMX del fixture.

## Estado

Documento en desarrollo. La configuracion se repetira desde un workspace limpio
y se agregaran capturas una vez validada.

