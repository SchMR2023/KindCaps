import cv2
import numpy as np
import serial

ser = serial.Serial('/dev/ttyUSB0', 115200)

blur = (3, 3)
thresh = 20
result = ''

colorDict = {
    '0' : np.array([[0, 141, 84], [10, 230, 255]]),     # red
    '1' : np.array([[22, 133, 170], [30, 214, 255]]),   # green
    '2' : np.array([[100, 91, 55], [127, 231, 196]]),   # blue
    '3' : np.array([[92, 112, 103], [109, 174, 181]])   # white
}

def getUART(result):
    return int(ser.readline())

def sendUART(result):
    ser.write(result)

start = int(getUART())

while not start:
    start = getUART()
  
cap = cv2.VideoCapture(0)                               # видео с камеры
                                                            
success, img = cap.read()                               # захват кадра

if not success:                                         # проверка наличия кадра
    result = 'ERROR'
    
else:
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)           # перевод изображения в формат Hue, Saturation, Value (оттенок, насыщенность, яркость)
    for color in colorDict:

        checkingColor = colorDict[color]
        
        h_min = checkingColor[0][0]
        h_max = checkingColor[1][0]
        s_min = checkingColor[0][1]
        s_max = checkingColor[1][1]
        v_min = checkingColor[0][2]
        v_max = checkingColor[1][2]

        lower = np.array([h_min, s_min, v_min])             # нижняя граница для фильтра
        upper = np.array([h_max, s_max, v_max])             # верхняя граница для фильтра

        mask = cv2.inRange(imgHSV, lower, upper)            # создание фильтра
        imgMask = cv2.bitwise_and(img, img, mask=mask)      # наложение фильтра

        gray_image = cv2.cvtColor(imgMask, cv2.COLOR_BGR2GRAY)                                      # ч/б формат
        blurred = cv2.GaussianBlur(gray_image, blur, 0)                                             # размытие 
        _, thresh_img = cv2.threshold(blurred, thresh, 255, cv2.THRESH_BINARY)                      # двоичный ч/б формат
        contours, _ = cv2.findContours(thresh_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)      # поиск контуров 
        #                                   🠕                 🠕                      🠕
        #                              изображение, только внешние контуры, простая аппроксимация(упрощение сложных контуров)

        if cv2.contourArea(contours[0]) > 400:              # проверка размеров найденного контура
            result = color                                  # присваивание результату значения цвета
        else:
            result = 'ERROR'

        

sendUART(result)                                        # вывод результата сканирования