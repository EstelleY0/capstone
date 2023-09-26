import cv2
import numpy as np
import matplotlib.pyplot as plt
import warnings
import serial
import time
from multiprocessing import Process, Queue
warnings.filterwarnings('ignore')

# 640 x 480 이미지
image_width = 640
image_height = 480
w = 640
h = 480


def color_filter(image):
    HSV_frame = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    H,S,V = cv2.split(HSV_frame)

    H_green_lower = 35
    # green
    H_green_condition = (H_green_lower<H) & (H<90)
    S_green_condition = (S>30)
    V_green_condition = V>100
    green_condition = H_green_condition & S_green_condition & V_green_condition
    H[green_condition] = 50
    S[green_condition] = 100
    V[green_condition] = 100

    V_white_condition = V>200

    white_condition = V_white_condition
    H[V_white_condition] = 0
    S[V_white_condition] = 0
    V[V_white_condition] = 255
    # gray
    V_gray_condition = (115<V) & (V<=200)

    # S_gray_condition = S<25
    gray_condition = V_gray_condition #& S_gray_condition
    H[gray_condition] = 120
    S[gray_condition] = 150
    V[gray_condition] = 150
    # black -> blue (road)
    road_condition = True^ ( (green_condition | V_white_condition) | V_gray_condition)
    H[road_condition] = 120
    S[road_condition] = 150
    V[road_condition] = 150

    HSV_frame[:,:,0] = H
    HSV_frame[:,:,1] = S
    HSV_frame[:,:,2] = V
    frame_filtered = cv2.cvtColor(HSV_frame, cv2.COLOR_HSV2BGR)

    return frame_filtered

def remove_black(image):
    x = np.linspace(0,639,640)
    y = np.linspace(0,479,480)
    X,Y = np.meshgrid(x,y)

    left_bottom = -123*X + 69*Y - 69*354
    left_bottom = left_bottom>0
    right_bottom = 173*(X-639) + 93*Y - 93*304
    right_bottom = right_bottom>0

    B, G, R = cv2.split(image)
    left_color = image[479, 70]
    right_color = image[479, 545]

    B[left_bottom] = left_color[0]
    G[left_bottom] = left_color[1]
    R[left_bottom] = left_color[2]

    B[right_bottom] = right_color[0]
    G[right_bottom] = right_color[1]
    R[right_bottom] = right_color[2]

    image[:,:,0] = B
    image[:,:,1] = G
    image[:,:,2] = R

    return image



# BGR로 특정 색을 추출하는 함수 (흰색 라인 검출 용, 실내 : 검정)
def bgrExtraction(image):
    bgrLower = np.array([155, 175, 185])    # 추출할 색의 하한
    bgrUpper = np.array([255, 255, 255])    # 추출할 색의 상한
    img_mask = cv2.inRange(image, bgrLower, bgrUpper)
    result = cv2.bitwise_and(image, image, mask=img_mask)
    ###cv2.imshow('bgr', result)
    return result


def reg_of_interest(image) :    # Region of Interest(관심영역)
    image_height = image.shape[0]
    image_width = image.shape[1]

    polygons = np.array(    ### hyper parameter ###
        [[ (round(image_width * 0.00), image_height), \
            (round(image_width * 0.99), image_height), \
            (round(image_width * 0.99), round(image_height * 0.00)),\
            (round(image_width * 0.00), round(image_height * 0.00)) ]] )
    image_mask = np.zeros_like(image)
    cv2.fillPoly(image_mask, polygons, 255)
    masking_image = cv2.bitwise_and(image, image_mask)
    # cv2.imshow('roi', masking_image)
    return masking_image


def show_lines(image, lines) :
    image = image
    left_line = []
    right_line = []
    if lines is not None :
        for line in lines:
            for rho, theta in line:
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))
                slope = (y2 - y1) / (x2 - x1 + 1e-6)
                print(slope)
                if((slope < -0.3 and x1<image_width*0.6) or (slope >0.4 and x2>image_width*0.4)):
                    cv2.line(image, (x1, y1), (x2, y2), (0, 0, 255), 3)
                # if(x0 < 320):
                #     left_line.append(x0)
                # else:
                #     right_line.append(x0)
                if slope < -0.4 and x1<image_width*0.6:
                    left_line.append(x0)
                elif slope >0.4 and x2>image_width*0.4:
                    right_line.append(x0)
    print(left_line)
    print(right_line)

    if(len(left_line)==0 and len(right_line)!=0):
        center =0
        return image, center
    elif(len(left_line)!=0 and len(right_line)==0):
        center = 640
        return image, center
    elif(len(left_line)==0 and len(right_line)==0):
        center = 320
        return image, center
    else:
        ave_left = sum(left_line)/len(left_line)
        ave_right = sum(right_line)/len(right_line)
        center = round((ave_left + ave_right) / 2)
        return image, center


def lane_detect(lanelines_image):
    #bird-eye conversion
    src = np.float32([[0, 400], [440, 400], [0, 200], [640, 200]])
    dst = np.float32([[0, 480], [640, 480], [0, 0], [640, 0]])
    matrix = cv2.getPerspectiveTransform(src, dst)
    bird_conversion = cv2.warpPerspective(lanelines_image, matrix, (640,480))

    #색변환
    filtered = color_filter(bird_conversion)
    removed_black = remove_black(filtered)

    bgr_extraction_image = bgrExtraction(filtered)
    #흰 검으로 변환해서 라인 검출함.
    gray_conversion = cv2.cvtColor(bgr_extraction_image, cv2.COLOR_BGR2GRAY)
    ###cv2.imshow('gray', gray_conversion)
    blur_conversion = cv2.GaussianBlur(gray_conversion, (5, 5), 0)      ### hyper parameter ###
    ###cv2.imshow('blur', blur_conversion)
    canny_conversion = cv2.Canny(gray_conversion, 100, 100)              ### hyper parameter (hsv : 200, 200) ###
    ###cv2.imshow('canny', canny_conversion)
    roi_conversion = reg_of_interest(canny_conversion)

    img_height = roi_conversion.shape[0]
    img_width = roi_conversion.shape[1]

    # cv2.imshow('removed_black', removed_black)

    #라인 이어주기
    lines = cv2.HoughLines(roi_conversion, 1, np.pi/180., 100)
    #print(averaged_lines)
    center = show_lines(removed_black, lines)
    #원본 이미지에 라인 그리기
    #combine_image = cv2.addWeighted(lanelines_image, 0.8, lines_image, 1, 1)

    return center


ser = serial.Serial('/dev/cu.usbserial-1440', 9600)
def sendserial(center):
    port = "/dev/cu.usbserial-1440"
    baud_rate = 9600
    global ser
    s = 's'
    r = 'r'
    l = 'l'
    s = s.encode('utf-8')
    r = r.encode('utf-8')
    l = l.encode('utf-8')

    time.sleep(0.2)
    # Send the encoded character over the serial port
    if (center > 300) and (center < 210):
        ser.write(s)
    elif (center <= 210):
        ser.write(l)
    else:
        ser.write(r)
    # print(ser)
    ser.flush()
    # ser.close()



# cap = cv2.VideoCapture(0)
# cap.set(cv2.CAP_PROP_FPS, 5)
# ser = serial.Serial(port, baud_rate)

while True:
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FPS, 5)
    ret, frame = cap.read()
    frame = cv2.resize(frame, (640, 480))
    # cv2.imshow('frame', frame)
    image, center= lane_detect(frame)
    cv2.imshow('image', image)
    print(center)

    t = Process(target=sendserial, args=(center,))
    t.start()
    t.join()
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
ser.close()
cap.release()
cv2.destroyAllWindows()
