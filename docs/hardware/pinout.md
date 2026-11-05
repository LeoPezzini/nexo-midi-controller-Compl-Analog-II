# Mapa de pines

Mapa definitivo reconstruido a partir del cableado fisico, verificado sobre el
prototipo y documentado en el esquematico de KiCad.

| Elemento | Senal | ATmega328P / Arduino Uno |
|---|---|---:|
| CD74HC4067 | S3 | 2 |
| CD74HC4067 | S2 | 3 |
| CD74HC4067 | S1 | 4 |
| CD74HC4067 | S0 | 5 |
| CD74HC4067 | COM | A0 |
| Divisor de bateria | Punto medio 10k/10k | A1 |
| Encoder | SW | 6 |
| Encoder | DT | 7 |
| Encoder | CLK | 8 |
| TFT | CS | 9 |
| TFT | DC/A0 | 10 |
| TFT | MOSI/SDA | 11 |
| TFT | RESET | 12 |
| TFT | SCK | 13 |
| MIDI OUT | TX | 1 |

## Canales fisicos del multiplexor

| Control | Canal | Control | Canal |
|---|---:|---|---:|
| B1 | 0 | B7 | 7 |
| B2 | 3 | B8 | 10 |
| B3 | 6 | B9 | 2 |
| B4 | 9 | B10 | 5 |
| B5 | 1 | B11 | 8 |
| B6 | 4 | B12 | 11 |
| P1 | 12 | P3 | 14 |
| P2 | 13 | P4 | 15 |

La distribucion de los pulsadores sigue el barrido fisico por columnas:
C0-C3-C6-C9 en la primera fila, C1-C4-C7-C10 en la segunda y
C2-C5-C8-C11 en la tercera. Los potenciometros ocupan C12-C15 de izquierda a
derecha.

El terminal COM se conecta a A0, S0-S3 se conectan a D5-D2 y la habilitacion
del CD74HC4067 permanece a nivel bajo. Cada pulsador posee una resistencia de
10 kohm hacia +5 V y cierra la entrada a masa al ser presionado.

## Salida MIDI DIN

- D1/TX -> 220 ohm -> pin 5.
- +5 V -> 220 ohm -> pin 4.
- GND -> pin 2.
- Pines 1 y 3 sin conexion.
