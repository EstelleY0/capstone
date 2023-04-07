# 이전에 환결설정 영상에서 했던 방법으로 matplotlib 라이브러리를 추가해주시면 됩니다.
# 설치가 안되면 pip version upgrade 하고 다시 시도해주세요.

import Function_Library as fl

EPOCH = 500000

if __name__ == "__main__":
    env = fl.libCAMERA()

    """ Exercise 1: RGB Color Value Extracting """
    ######################################################
    # example = env.file_read("./bird.jpg")
    # R, G, B = env.extract_rgb(example, print_enable=True)
    # print("RED SAMPLE: ", R[0])
    # quit()
    ######################################################

    # Camera Initial Setting
    ch0, ch1 = env.initial_setting(capnum=2)

    # Camera Reading
    for i in range(EPOCH):
        _, frame0, _, frame1 = env.camera_read(ch0, ch1)

        """ Exercise 2: Webcam Real-time Reading """
        ################################
        env.image_show(frame0, frame1)
        ################################

        """ Exercise 3: Object Detection (Traffic Light Circle) """
        ####################################################################
        # color = env.object_detection(frame1, sample=16, print_enable=True)
        ####################################################################

        """ Exercise 4: Specific Edge Detection (Traffic Line) """
        ###########################################################################
        direction = env.edge_detection(frame1, width=640, height=480, gap=40, threshold=150, print_enable=True)
        ###########################################################################



        # Process Termination (If you input the 'q', camera scanning is ended.)
        if env.loop_break():
            break



