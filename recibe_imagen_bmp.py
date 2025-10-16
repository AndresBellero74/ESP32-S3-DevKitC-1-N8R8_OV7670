import requests
import time
import numpy as np
import cv2

URL = "http://192.168.0.250/photo.bmp"

cv2.namedWindow("ESP32-CAM", cv2.WINDOW_AUTOSIZE)

while True:
    try:
        response = requests.get(URL, timeout=5)
        if response.status_code == 200:
            # Convertir BMP a array de imagen
            img_array = np.frombuffer(response.content, dtype=np.uint8)
            img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

            if img is not None:
                cv2.imshow("ESP32-CAM", img)
            else:
                print("⚠️ No se pudo decodificar la imagen")
        else:
            print(f"⚠️ Error HTTP {response.status_code}")
    except Exception as e:
        print(f"❌ Error de conexión: {e}")

    # Espera 1 segundo o hasta que se presione 'q'
    if cv2.waitKey(1000) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
