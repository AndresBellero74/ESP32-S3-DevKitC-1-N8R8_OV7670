#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>  // ya lo tenés
WebServer server(80);   // ← esta línea es la que falta


/*const int SIOD = 8; //SDA
const int SIOC = 9; //SCL

const int VSYNC = 7;
const int HREF = 6;

const int XCLK = 10;
const int PCLK = 5;

const int D0 = 1;
const int D1 = 2;
const int D2 = 3;
const int D3 = 4;

const int D4 = 13;
const int D5 = 14;
const int D6 = 11;
const int D7 = 12;*/
// Pines para ESP32-S3 (ajustá según tu wiring)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
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

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "Telecentro-2be3";
const char* password = "GU3QFXA6FH47";
// IP fija
IPAddress local_IP(192, 168, 0, 250);      // IP deseada
IPAddress gateway(192, 168, 0, 1);         // IP del router
IPAddress subnet(255, 255, 255, 0);        // Máscara de subred
IPAddress dns(8, 8, 8, 8);                 // DNS opcional

void startCameraServer() {
  server.on("/photo.bmp", HTTP_GET, handlePhoto);
  server.begin();
}

/*void handlePhoto() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "❌ Error al capturar imagen");
    return;
  }
  Serial.printf("Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);
  server.setContentLength(54 + fb->width * fb->height * 3); // BMP header + pixel data
  server.send(200, "image/bmp", ""); // Cambia MIME type

  WiFiClient client = server.client();

// 🧠 Encabezado BMP (simplificado para 24 bits sin compresión)
  uint8_t bmp_header[54] = {
    0x42, 0x4D,                         // BM
    0, 0, 0, 0,                         // Tamaño total (lo completamos abajo)
    0, 0, 0, 0,                         // Reservado
    54, 0, 0, 0,                        // Offset a datos
    40, 0, 0, 0,                        // Tamaño del header DIB
    fb->width & 0xFF, (fb->width >> 8) & 0xFF, 0, 0,
    fb->height & 0xFF, (fb->height >> 8) & 0xFF, 0, 0,
    1, 0,                               // Planos
    24, 0,                              // Bits por pixel
    0, 0, 0, 0,                         // Compresión (0 = sin compresión)
    0, 0, 0, 0,                         // Tamaño de imagen (puede ser 0)
    0, 0, 0, 0,                         // Resolución X
    0, 0, 0, 0,                         // Resolución Y
    0, 0, 0, 0,                         // Colores usados
    0, 0, 0, 0                          // Colores importantes
  };

  // 🧠 Completar tamaño total
  uint32_t file_size = 54 + fb->width * fb->height * 3;
  bmp_header[2] = file_size & 0xFF;
  bmp_header[3] = (file_size >> 8) & 0xFF;
  bmp_header[4] = (file_size >> 16) & 0xFF;
  bmp_header[5] = (file_size >> 24) & 0xFF;

  // 🧠 Enviar encabezado
  client.write(bmp_header, 54);

  // 🧠 Convertir RGB565 a BGR y enviar
  for (int y = fb->height - 1; y >= 0; y--) {
    for (int x = 0; x < fb->width; x++) {
      int i = (y * fb->width + x) * 2;
 //     uint16_t pixel = fb->buf[i] | (fb->buf[i + 1] << 8);
      uint16_t pixel = (fb->buf[i] << 8) | fb->buf[i + 1];

      uint8_t r = ((pixel >> 11) & 0x1F) << 3;
      uint8_t g = ((pixel >> 5) & 0x3F) << 2;
      uint8_t b = (pixel & 0x1F) << 3;

      client.write(b);
      client.write(g);
      client.write(r);
    }
  }


  esp_camera_fb_return(fb);
} */
void handlePhoto() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "❌ Error al capturar imagen");
    return;
  }

  Serial.printf("Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);

  const int width = fb->width;
  const int height = fb->height;
  const int rowSize = width * 3;
  const int padding = (4 - (rowSize % 4)) % 4;
  const uint32_t file_size = 54 + (rowSize + padding) * height;

  server.setContentLength(file_size);
  server.send(200, "image/bmp", "");

  WiFiClient client = server.client();

  // 🧠 Encabezado BMP (24 bits)
  uint8_t bmp_header[54] = {
    0x42, 0x4D,
    (uint8_t)(file_size), (uint8_t)(file_size >> 8), (uint8_t)(file_size >> 16), (uint8_t)(file_size >> 24),
    0, 0, 0, 0,
    54, 0, 0, 0,
    40, 0, 0, 0,
    (uint8_t)(width), (uint8_t)(width >> 8), 0, 0,
    (uint8_t)(height), (uint8_t)(height >> 8), 0, 0,
    1, 0,
    24, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

  client.write(bmp_header, 54);

  // 🧠 Buffer temporal para una fila
  uint8_t rowBuffer[rowSize + padding];

  for (int y = height - 1; y >= 0; y--) {
    int rowIndex = 0;

    for (int x = 0; x < width; x++) {
      int i = (y * width + x) * 2;
      uint16_t pixel = (fb->buf[i] << 8) | fb->buf[i + 1];

      uint8_t r = ((pixel >> 11) & 0x1F) << 3;
      uint8_t g = ((pixel >> 5) & 0x3F) << 2;
      uint8_t b = (pixel & 0x1F) << 3;

      rowBuffer[rowIndex++] = b;
      rowBuffer[rowIndex++] = g;
      rowBuffer[rowIndex++] = r;
    }

    // 🧩 Padding por fila
    for (int p = 0; p < padding; p++) {
      rowBuffer[rowIndex++] = 0x00;
    }

    // 🧠 Enviar fila completa
    client.write(rowBuffer, rowIndex);
  }

  esp_camera_fb_return(fb);
}
/*void handlePhoto() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "❌ Error al capturar imagen");
    return;
  }

  Serial.printf("Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);

  const int srcWidth = fb->width;     // 320
  const int srcHeight = fb->height;   // 240
  const int dstWidth = srcWidth / 2;  // 160
  const int dstHeight = srcHeight / 2;// 120
  const int rowSize = dstWidth * 3;
  const int padding = (4 - (rowSize % 4)) % 4;
  const uint32_t file_size = 54 + (rowSize + padding) * dstHeight;

  server.setContentLength(file_size);
  server.send(200, "image/bmp", "");

  WiFiClient client = server.client();

  // 🧠 Encabezado BMP para 24 bits
  uint8_t bmp_header[54] = {
    0x42, 0x4D,
    (uint8_t)(file_size), (uint8_t)(file_size >> 8), (uint8_t)(file_size >> 16), (uint8_t)(file_size >> 24),
    0, 0, 0, 0,
    54, 0, 0, 0,
    40, 0, 0, 0,
    (uint8_t)(dstWidth), (uint8_t)(dstWidth >> 8), 0, 0,
    (uint8_t)(dstHeight), (uint8_t)(dstHeight >> 8), 0, 0,
    1, 0,
    24, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

  client.write(bmp_header, 54);

  // 🧠 Reducción 2×2 y conversión RGB565 → BGR
  for (int y = dstHeight - 1; y >= 0; y--) {
    for (int x = 0; x < dstWidth; x++) {
      int srcX = x * 2;
      int srcY = y * 2;
      int i = (srcY * srcWidth + srcX) * 2;
      uint16_t pixel = (fb->buf[i] << 8) | fb->buf[i + 1];

      uint8_t r = ((pixel >> 11) & 0x1F) << 3;
      uint8_t g = ((pixel >> 5) & 0x3F) << 2;
      uint8_t b = (pixel & 0x1F) << 3;

      client.write(b);
      client.write(g);
      client.write(r);
    }

    // 🧩 Padding por fila
    for (int p = 0; p < padding; p++) {
      client.write((uint8_t)0x00);
    }
  }

  esp_camera_fb_return(fb);
}*/

/*void handlePhoto() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "❌ Error al capturar imagen");
    return;
  }

  Serial.printf("Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);

  int rowSize = fb->width;
  int padding = (4 - (rowSize % 4)) % 4;
  uint32_t file_size = 54 + 1024 + (rowSize + padding) * fb->height;

  server.setContentLength(file_size);
  server.send(200, "image/bmp", "");

  WiFiClient client = server.client();

  // 🧠 Encabezado BMP para 8 bits + paleta
  uint8_t bmp_header[54] = {
    0x42, 0x4D,
    (uint8_t)(file_size), (uint8_t)(file_size >> 8), (uint8_t)(file_size >> 16), (uint8_t)(file_size >> 24),
    0, 0, 0, 0,
    54 + 1024, 0, 0, 0,
    40, 0, 0, 0,
    (uint8_t)(fb->width), (uint8_t)(fb->width >> 8), 0, 0,
    (uint8_t)(fb->height), (uint8_t)(fb->height >> 8), 0, 0,
    1, 0,
    8, 0, // 8 bits por píxel
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

  client.write(bmp_header, 54);

  // 🧠 Paleta de grises (256 colores)
  for (int i = 0; i < 256; i++) {
    client.write(i); // B
    client.write(i); // G
    client.write(i); // R
    client.write((uint8_t)0); // Reserved
  }

  // 🧠 Convertir RGB565 a escala de grises y enviar
  for (int y = fb->height - 1; y >= 0; y--) {
    for (int x = 0; x < fb->width; x++) {
      int i = (y * fb->width + x) * 2;
      //uint16_t pixel = (fb->buf[i] << 8) | fb->buf[i + 1];
      uint16_t pixel = fb->buf[i] | (fb->buf[i + 1] << 8);

      uint8_t r = ((pixel >> 11) & 0x1F) << 3;
      uint8_t g = ((pixel >> 5) & 0x3F) << 2;
      uint8_t b = (pixel & 0x1F) << 3;

      //uint8_t gray = (r * 30 + g * 59 + b * 11) / 100;
      uint8_t gray = (r + g + b) / 3;
      client.write(gray);
    }

    // 🧩 Padding por fila
    for (int p = 0; p < padding; p++) {
      client.write((uint8_t)0x00);
    }
  }

  esp_camera_fb_return(fb);
}*/


void setup() {
  Serial.begin(115200);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_RGB565;

  config.frame_size = FRAMESIZE_VGA;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  //config.fb_location = CAMERA_FB_IN_DRAM;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Error al iniciar la cámara");
    return;
  }


  Serial.println("Cámara inicializada correctamente");
  
  // Configurar IP fija
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("Error al configurar IP fija");
  }
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  startCameraServer();
}

void loop() {
   // Do nothing. Everything is done in another task by the web server
  server.handleClient();
/*  delay(2000);
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar imagen");
    return;
  }
  
  server.setContentLength(fb->len);              // ✅ define tamaño exacto
  server.send(200, "image/jpeg", "");            // ✅ envía solo encabezados
  WiFiClient client = server.client();           // ✅ acceso directo al socket
  client.write(fb->buf, fb->len);                // ✅ envío binario directo
  

  Serial.printf("Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);
  esp_camera_fb_return(fb); */
}
