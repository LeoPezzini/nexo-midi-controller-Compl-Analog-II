# Sistema de alimentacion autonoma

## Cadena de potencia

```text
Bateria 3,7 V -> TP4056 -> interruptor -> MT3608 a 5,00 V -> VCC del sistema
```

- La bateria se conecta a `B+` y `B-` del TP4056.
- La carga se toma desde `OUT+` y `OUT-`.
- El interruptor se ubica entre el TP4056 y la entrada del MT3608.
- La salida del MT3608 se ajusta a 5,00 V antes de conectarla al equipo.
- Los 5 V regulados ingresan por `VCC`, evitando el regulador lineal 7805.

## Medicion de bateria

La tension se mide antes del MT3608, ya que la salida del elevador permanece
cercana a 5 V durante gran parte de la descarga y no representa el estado real
de la celda.

Dos resistencias de 10 kohm forman un divisor 1:2. El punto medio llega a A1, de
modo que una bateria a 4,20 V produce aproximadamente 2,10 V en la entrada ADC.

El firmware promedia ocho muestras, actualiza una vez por segundo y exige tres
lecturas consecutivas antes de cambiar el icono. Los umbrales implementados son:

| Tension estimada | Indicador |
|---:|---:|
| >= 4,05 V | 4 barras |
| >= 3,85 V | 3 barras |
| >= 3,65 V | 2 barras |
| >= 3,45 V | 1 barra |
| < 3,45 V | Aviso `BAT BAJA` |

El indicador es orientativo: la curva de descarga de una celda de litio no es
lineal y la tension varia con la carga instantanea.
