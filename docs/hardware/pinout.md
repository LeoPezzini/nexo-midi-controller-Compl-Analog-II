# Mapa de pines

Mapa reconstruido desde el firmware historico y confirmado mediante pruebas
sobre el equipo real.

| Elemento | Senal | ATmega328P / Arduino Uno |
|---|---|---:|
| CD74HC4067 | S3 | 2 |
| CD74HC4067 | S2 | 3 |
| CD74HC4067 | S1 | 4 |
| CD74HC4067 | S0 | 5 |
| CD74HC4067 | SIG | A0 |
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
| B1 | 0 | B7 | 14 |
| B2 | 12 | B8 | 5 |
| B3 | 6 | B9 | 4 |
| B4 | 9 | B10 | 10 |
| B5 | 8 | B11 | 1 |
| B6 | 2 | B12 | 13 |
| P1 | 3 | P3 | 7 |
| P2 | 11 | P4 | 15 |

La tabla explicita permite conservar el cableado existente aunque su orden no
sea secuencial.

