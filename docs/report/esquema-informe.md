# Estructura propuesta del informe

## Titulo

**Diseno e implementacion de una superficie de control MIDI reconfigurable para
aplicaciones musicales, audiovisuales y de iluminacion**

## Datos academicos

- Universidad Nacional de San Juan (UNSJ).
- Carrera: Ingenieria Electronica.
- Asignatura: Complementos de Analogica 2.
- Alumnos: Marcelo Cuello, registro 23646; Leonardo Pezzini, registro 25084.
- Docente responsable: Ernesto Accolti.

## Capitulos

1. Resumen.
2. Introduccion y motivacion.
3. Historia y evolucion del protocolo MIDI.
4. Fundamentos: mensajes, canales, CC, clock y transporte fisico.
5. Objetivos y requisitos del proyecto.
6. Hardware construido.
7. Arquitectura del firmware.
8. Interfaz, edicion y gestion de presets.
9. Sistema de alimentacion autonoma.
10. Metodologia de ensayos y problemas encontrados.
11. Aplicacion musical con FL Studio.
12. Control de iluminacion con QLC+.
13. Control audiovisual con OBS Studio.
14. Posibilidades de sincronizacion y automatizacion escenica.
15. Resultados, limitaciones y mejoras futuras.
16. Conclusiones.
17. Referencias y anexos.

## Idea central

MIDI no transporta audio: transporta instrucciones y valores. Por esa razon, un
mismo gesto fisico puede disparar una nota, cambiar el color de una luminaria o
seleccionar una escena audiovisual. La funcion final depende del mapeo realizado
en el receptor.

