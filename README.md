# TPI - Informática 02 - UTN.FRBA
## Integrantes:
* Juanes  Francisco  (221.033-2)
* Román   Mateo      (220.926-3)
* Yopolo  Iván       (233.133-0)   

## Idea fuerza:   
Desarrollar un rover terrestre inspirado en los robots de exploración espacial, capaz de desplazarse a control remoto, detectar obstáculos y transmitir información del entorno hacia una computadora en tiempo real. Además, el rover debe captar imagen en vivo (cámara), más un mapeo del entorno por curvas de nivel con colores en una interfaz gráfica. Nuestra decisión fue por el deseo de realizar un *Mars Rover de la NASA*.   

### Desafíos:
* Control de potencia (PWM).
* Conexión por WiFi.
* Mapeo topográfico del terreno.
* Interfaz gráfica.
* Procesamiento de imágenes en tiempo real.

### Hardware adicional:
* Doble Puente H.
* Acelerómetro.
* Sensor emisor/receptor Ultrasónico.
* Módulo WiFi.

---

## Repositorio:
Nos dividimos el trabajo en varias partes:

### LPC845
* Protocolo de comunación (WiFi).
* Manejo de motores (Puente H).

### ESP32
* Procesamiento de imagen (cámara).
* Protocolo de comunación (WiFi).

### PC (Qt)
* Protocolo de comunación (WiFi).
* Mapeo topográfico del terreno según input del ultrasónico del LPC.
