import serial

port = 'COM'
baudrate = 115200


pyserial = serial.Serial(port, baudrate)

def judge(cam, us, lid):
    mid_p = camera_line(cam)
    ob_lid = lidar(lid)
    us_collid = []

    # 장애물이 차선보다 먼저
    
    #차선 체킹
    commnd = camera_line(cam)
    
    # 종합해서 상황판단

    # 상황판단 완료 후 각 상황에 대한 시그널 지정해서 보내기, 길면 안됨 시리얼 통신이라 지연존재
    pyserial.write(commend.encode())

# def US():
#     # 초음파 센서 값 읽어오기 -일단은 매번 읽게 해둠. 근데 상황따라 수정 필
#     if pyserial.readable():
#         line = pyserial.readline()
#         #  초음파 센서 중 충돌거리 이하 감지한 센서 넘버링
#         for i in list(line.decode()):
#             if i == 1:
#                 us_collid.append(i)


def camera_line(cam):
    # From camera, get the V
    # N, find middle of the line
    middle_point = 0
    command = None
    if cam < middle_point :
        command = 'right'
    elif cam > middle_point :
        command = 'left'
    else:
        command = 'straight'
    return command

# def lidar(lid):
#     # From LIDAR, get data
#     # N, find the obstacle
#     # None, front, left, right, back
#     where = None
#     return where
