# ESP32-S3-DevKitC-1-N8R8_OV7670
ESP32-S3-DevKitC-1-N8R8_OV7670 web server de imagenes bmp
Las conecciones a la camara son las siguientes:
#define PWDN_GPIO_NUM    -1 // GND
#define RESET_GPIO_NUM   -1 // 3.3v
#define XCLK_GPIO_NUM    10
#define SIOD_GPIO_NUM    8
#define SIOC_GPIO_NUM    9

#define Y9_GPIO_NUM      12
#define Y8_GPIO_NUM      11
#define Y7_GPIO_NUM      14
#define Y6_GPIO_NUM      13
#define Y5_GPIO_NUM      4
#define Y4_GPIO_NUM      3
#define Y3_GPIO_NUM      2
#define Y2_GPIO_NUM      1
#define VSYNC_GPIO_NUM   7
#define HREF_GPIO_NUM    6
#define PCLK_GPIO_NUM    5
Usamos la PSRAM para lograr la maxima resolucion de esta camara que es 640 x 480.
viene codigo platformio para esp32-s3 y camara ov7670
viene codigo python cliente que muestra imagenes de la camara.
