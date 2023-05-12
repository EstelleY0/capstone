# LiDAR Lib
import Lib_LiDAR as LiDAR
import threading

# 전역변수에 장애물이 전방에 있는지 없는지에 대한 정보가 저장됨
g_stop_flag = False

def init_lidar(port_name):
    env = LiDAR.libLidar(port_name)
    env.init()
    return env

def stop_function(scan):
    global g_stop_flag
    scan = env.getAngleDistanceRange(scan, 160, 220, 100, 600)
    if len(scan) > 0:
        g_stop_flag = True
        # return True
    else:
        g_stop_flag =  False
        # return False

def scan_lidar(env):
    for scan in env.scanning():
        stop_function(scan)


# USB 포트 번호에 따라 포트 번호 변경 필요 
PORT_NAME = 'COM8'

if (__name__ == "__main__"):
    env = init_lidar(PORT_NAME)
    t1 = threading.Thread(target=scan_lidar, args=(env,))
    t1.start()
    # 장애물 전방 유무를 출력하는 테스트 코드
    while True:
        print(g_stop_flag)
