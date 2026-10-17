# Demostraciones de software

El objetivo de estas pruebas es demostrar que MIDI describe eventos y valores,
pero no impone una aplicacion concreta. El mismo hardware y el mismo firmware
pueden controlar musica, iluminacion o produccion audiovisual mediante
diferentes mapeos en el receptor.

| Aplicacion | Uso validado | Estado |
|---|---|---|
| QLC+ 5.2.2 | RGB, intensidad, escenas, apagado y secuencia | Validado |
| OBS Studio 32.2.2 | Escenas, grabacion, mute y niveles de audio | Validado |
| FL Studio | Notas, instrumentos virtuales y parametros CC | Pendiente |

Guias:

- [QLC+](qlc-plus.md)
- [OBS Studio](obs-studio.md)
- [FL Studio](fl-studio.md)

QLC+ y OBS Studio confirmaron que los mensajes de notas y Control Change pueden
adquirir funciones completamente diferentes sin modificar el dispositivo. La
prueba con FL Studio completara la demostracion musical en una estacion de
trabajo de escritorio.
