const int motor1_a = 5; //rear motor 1
const int motor1_b = 6; //rear motor 1
const int motor2_a = 7; //rear motor 2
const int motor2_b = 8; //rear motor 2

const int steer_m1 = 3; //steer motor
const int steer_m2 = 4; //steer motor

const int TRIG1 = 9;    //ultrasonic trig
const int ECHO1 = 10;   //ultrasonic echo
const int TRIG2 = 11;   //ultrasonic trig
const int ECHO2 = 12;   //ultrasonic echo
const int TRIG3 = 13;   //ultrasonic trig
const int ECHO3 = 14;   //ultrasonic echo
const int TRIG4 = 15;   //ultrasonic trig
const int ECHO4 = 16;   //ultrasonic echo
const int TRIG5 = 17;   //ultrasonic trig
const int ECHO5 = 18;   //ultrasonic echo
const int TRIG6 = 19;   //ultrasonic trig
const int ECHO6 = 20;   //ultrasonic echo

const int MAX_DISTANCE = 2000;  //for ideal can detect for about 4m
const int ROLL_AHEAD = 500;     //minimum distance for not to colllide
const int PAUSE_TIME = 1000;


const int angle_offset = -8;    //steering offset in deg
const int angle_limit = 55;     //steering angle limit in deg

float cur_steering;         //current steering  needed for gradual change
float cur_speed;            //current speed     needed for overcome friction
float compute_steering;     //steering for compute
float compute_speed;        //speed for compute


const int punch_pwm = 200;    //pwm for overcome friction
const int punch_time = 50;    //time needed to overcome friction
const int stop_time = 300;    //time needed to change between forward n backward


void motor_forward(int IN1, int IN2, int speed)
{
    analogWrite(IN1, speed);
    analogWrite(IN2, LOW);
}

void motor_backward(int IN1, int IN2, int speed)
{
    analogWrite(IN1, LOW);
    analogWrite(IN2, speed);
}

void motor_hold(int IN1, int IN2)
{
    analogWrite(IN1, LOW);
    analogWrite(IN2, LOW);
}

int potentiometer_Read(int pin)
{
    int value;

    value = analogRead(pin) / 4;

    return value;
}

float GetDistance(int trig, int echo)
{
    long distance, duration;

    digitalWrite(trig, LOW);
    digitalWrite(echo, LOW);
    delayMicroseconds(2);

    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);

    if (duration == 0)
        return MAX_DISTANCE;    //no responce
    else
        return distance = ((float)(340 * duration) / 1000) / 2; //speed 340m/s
}

void avoid(){
    first = 0;
    if (first==0)
    {
        while(GetDistance(TRIG2, ECHO2) > 750)
        {
            #turn left
        }
    }
    else{
        while(GetDistance(TRIG6, ECHO6) > 750)
        {
            #turn right
        }
    }
}

void DriveMotor(int toward, int pwm=0)
{
    //toward front:1, backward:-1, hold:0
    //pwm 0~255 if no pwm input-hold

    if(toward == 0)
    {
        motor_hold(motor1_a, motor1_b);
        motor_hold(motoe2_a, motoe2_b);
    }
    else if (toward == 1)
    {
        motor_forward(motor1_a, motor1_b, pwm);
        motor_forward(motor2_a, motor2_b, pwm);
    }
    else
    {
        motor_backward(motor1_a, motor1_b, pwm);
        motor_backward(motor2_a, motor2_b, pwm);
    }
}
/*
void SteerMotor(float steering)
{
    cur_steering = constrain(steering, -1, 1); // constrain -1~ 1 값으로 제한

    float angle = cur_steering * angle_limit;

    //need more code for actual steering
}
*/
float abs_float(float num){
    return num < 0 ? -num : num;
}
const int Potentiometer_Pin = 13;
void SteerMotor(float steering)
{
    // cur_steering = constrain(steering, -1, 1); // constrain -1~ 1 값으로 제한

    float angle = steering * angle_limit;
    float gap_goal = 0.01f;
    float const_num = 100.f;
    while (1)
    {
        float now_angle = potentiometer_Read(Potentiometer_Pin);
        if (abs_float(now_angle - angle) < gap_goal)
            break;
        float delta_angle = angle - now_angle;
        if (delta_angle < 0)
            motor_forward(steer_m1, steer_m2, abs_float(delta_angle) * const_num);
        else
            motor_backward(steer_m1, steer_m2, abs_float(delta_angle) * const_num);
    }
    //need more code for actual steering
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
            DriveMotor(1, pwm)
        }
        else if (speed < 0)
        {
            DriveMotor(-1, pwm)
        }
    }
    cur_speed = speed;
}

void Drive()
{
    if (GetDistance(TRIG1, ECHO1) < 750)
    {
        avoid()
    }
    else if (0)      //straight -- need to fill condition
    {
        compute_steering = 0;
        compute_speed = 0;
    }
    else if (1) //toward left
    {
        compute_steering = -1;
        compute_speed = 0;
    }

    else if (2) //toward right
    {
        compute_steering = 1;
        compute_speed = 0;
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
    pinMode(ECHO2, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO3, INPUT);
    pinMode(TRIG3, OUTPUT);
    pinMode(ECHO4, INPUT);
    pinMode(TRIG4, OUTPUT);
    pinMode(ECHO5, INPUT);
    pinMode(TRIG5, OUTPUT);
    pinMode(ECHO6, INPUT);
    pinMode(TRIG6, OUTPUT);

    DriveSpeed(0);  //start from stop
    SteerMotor(0);  //start steering 0
}

void loop()
{
    compute_speed = cur_speed;
    compute_steering = cur_steering;

    Drive();
    DriveSpeed(compute_speed);
    SteerMotor(compute_steering);
}
