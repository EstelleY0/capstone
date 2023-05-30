const int motor1_a = 5; //rear motor 1
const int motor1_b = 6; //rear motor 1
const int motor2_a = 7; //rear motor 2
const int motor2_b = 8; //rear motor 2

const int steer_m1 = 3; //steer motor
const int steer_m2 = 4; //steer motor

const int TRIG1 = 9;    //ultrasonic trig
const int ECHO1 = 10;   //ultrasonic echo

const int MAX_DISTANCE = 2000;  //for ideal can detect for about 4m
const int ROLL_AHEAD = 500;     //minimum distance for not to colllide
const int PAUSE_TIME = 1000;


const int angle_offset = -8;    //steering offset in deg
const int angle_limit = 20;     //steering angle limit in de
const int angle_mid = 300;

float cur_steering = 1;         //current steering  needed for gradual change
float cur_speed = 100;            //current speed     needed for overcome friction
float compute_steering;     //steering for compute
float compute_speed;        //speed for compute


const int punch_pwm = 200;    //pwm for overcome friction
const int punch_time = 50;    //time needed to overcome friction
const int stop_time = 300;    //time needed to change between forward n backward

bool start = false;
char direct = 's';

void forward(int speed = 100)
{
    //forward, right
    analogWrite(motor1_a, speed);
    analogWrite(motor1_b, LOW);
    analogWrite(motor2_a, speed);
    analogWrite(motor2_b, LOW);
}

void backward(int speed = 100)
{
    //backward, left
    analogWrite(motor1_b, speed);
    analogWrite(motor1_a, LOW);
    analogWrite(motor2_b, speed);
    analogWrite(motor2_a, LOW);
}

void hold()
{
    analogWrite(motor1_a, LOW);
    analogWrite(motor1_b, LOW);
    analogWrite(motor2_a, LOW);
    analogWrite(motor2_b, LOW);
}

void left(int steer = 100)
{
    analogWrite(steer_m1, steer);
    analogWrite(steer_m2, LOW);
}

void right(int steer = 100)
{
    analogWrite(steer_m2, steer);
    analogWrite(steer_m1, LOW);
}

void straight()
{
    analogWrite(steer_m1, LOW);
    analogWrite(steer_m2, LOW);
}

int potentiometer_Read(int pin)
{
    int value;

    value = analogRead(pin) / 4;

    return value;
}

float GetDistance()
{
    long distance, duration;

    digitalWrite(TRIG1, LOW);
    digitalWrite(ECHO1, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG1, LOW);
    duration = pulseIn(ECHO1, HIGH);

    if (duration == 0)
        return MAX_DISTANCE;    //no responce
    else
        return distance = ((float)(340 * duration) / 1000) / 2; //speed 340m/s
}

void DriveMotor(int toward, int pwm=100)
{
    //toward front:1, backward:-1, hold:0
    //pwm 0~255 if no pwm input-hold

    if(toward == 0)
    {
        hold();
    }
    else if (toward == 1)
    {
        forward(pwm);
    }
    else
    {
        backward(pwm);
    }
}

void DriveSpeed(float speed)
{
    speed = constrain(speed, -1, 1);

    if ((cur_speed * speed < 0)            //when moving
        || (cur_speed != 0 && speed == 0)) //need to move opposite direction or stop
    {
        cur_speed = 0;
        DriveMotor(0);

        if (stop_time > 0)
            delay(stop_time);
    }

    if (cur_speed == 0 && speed != 0)       //from stop
    {
        if (punch_time > 0)
        {
            if (speed > 0)
            {
                DriveMotor(1, punch_pwm);
            }
            else if (speed < 0)
            {
                DriveMotor(-1, punch_pwm);
            }
            delay(punch_time);
        }
    }

    if (speed != 0) //need to move
    {
        int pwm = abs(speed) * (225) + 0; //speed to pwm, 225:max-min 0:min -- need modification

        if (speed > 0)
        {
            DriveMotor(1, pwm);
        }
        else if (speed < 0)
        {
            DriveMotor(-1, pwm);
        }
    }
    cur_speed = speed;
}

void Drive(char direction)
{
    if (GetDistance() < 750)
    {
        avoid();
    }
    else {
        // Execute the corresponding action
        if (direction == 'l') {
            compute_steering = -1;
            compute_speed = 1;
        } else if (direction == 'r') {
            compute_steering = 1;
            compute_speed = 1;
        } else if (direction == 's') {
            compute_steering = 0;
            compute_speed = 1;
        } else {
            compute_steering = 0;
            compute_speed = 0;
        }
    }
}

int phase = 0; //오른쪽으ㄹ 피하는거 먼저면 0 아니면 1로 수정

void avoid()
{
    if (phase == 0){
        //오른쪽 회피
        phase = 1;
        right(100);
        DriveMotor(1, 100);  //pwm = 100 이거 수정 해야함
        delay(1000);  //이거 그냥 1초 우회전 해놨는데이거도 수정 필요함
        left(100);
        DriveMotor(1, 100); //이거도 속도 수정
        delay(1000);  //이거도 시간 
    }
    else{
        //왼쪽회피
        phase = 0;
        left(100);
        DriveMotor(1, 100);  //pwm = 100 이거 수정 해야함
        delay(1000);  //이거 그냥 1초 우회전 해놨는데이거도 수정 필요함
        right(100);
        DriveMotor(1, 100); //이거도 속도 수정
        delay(1000);  //이거도 시간 
    }
}

void SteerMotor(int steer)
{
    if (steer>0)
    {
        //right
        right(steer*225);
    }
    else if (steer<0)
    {
        //left
        left(steer*225);
    }
    else
    {
        straight();
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(motor1_a, OUTPUT);
    pinMode(motor1_b, OUTPUT);
    pinMode(motor2_a, OUTPUT);
    pinMode(motor2_b, OUTPUT);
    
    pinMode(steer_m1, OUTPUT);
    pinMode(steer_m2, OUTPUT);
    
    pinMode(ECHO1, INPUT);
    pinMode(TRIG1, OUTPUT);

    DriveSpeed(0);  //start from stop
}

void loop()
{
    if (start)
    {
        compute_speed = cur_speed;
        compute_steering = cur_steering;

        if (Serial.available() > 0)
        {
            // Read the command from Python
            direct = Serial.read();
        }
        Drive(direct);
        DriveSpeed(compute_speed);
        SteerMotor(compute_steering);
    }
    else
    {
        if (Serial.available() > 0) { // Check if data is available to read
            char key = Serial.read();
            if (key == 'a'){
              start = true;
            }
        }
    }
}
