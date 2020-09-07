//
//  crew.hpp
//  aflac2019
//
//  Created by Wataru Taniguchi on 2019/04/28.
//  Copyright © 2019 Ahiruchan Koubou. All rights reserved.
//

#ifndef crew_hpp
#define crew_hpp

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/**
 * 左コース/右コース向けの設定を定義します
 * デフォルトは左コース(ラインの右エッジをトレース)です
 */
#if defined(MAKE_RIGHT)
    static const int _LEFT = 0;
    #define _EDGE -1
#else
    static const int _LEFT = 1;
    #define _EDGE 1
#endif

//#include <cinttypes>
#include <cmath>
#include "TouchSensor.h"
#include "SonarSensor.h"
#include "ColorSensor.h"
#include "GyroSensor.h"
#include "Motor.h"
#include "Steering.h"
#include "Clock.h"
using namespace ev3api;
#include "utility.hpp"

// global variables
extern rgb_raw_t g_rgb;
extern hsv_raw_t g_hsv;
extern int16_t g_grayScale, g_grayScaleBlueless;
extern int16_t g_angle, g_anglerVelocity;

/* 下記のマクロは個体/環境に合わせて変更する必要があります */
#define GYRO_OFFSET           0  /* ジャイロセンサオフセット値(角速度0[deg/sec]時) */
#define LIGHT_WHITE          60  /* 白色の光センサ値 */
#define LIGHT_BLACK           3  /* 黒色の光センサ値 */
#define GS_LOST              90  // threshold to determine "line lost"
#define OLT_SKIP_PERIOD    1000 * 1000 // period to skip outlier test in miliseconds
#define OLT_INIT_PERIOD    3000 * 1000 // period before starting outlier test in miliseconds
#define FINAL_APPROACH_LEN  100  // final approch length in milimater
#define ANG_V_TILT           50  // threshold to determine "tilt"
#define SONAR_ALERT_DISTANCE 10  /* 超音波センサによる障害物検知距離[cm] */
#define TAIL_ANGLE_STAND_UP  85  /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE      3  /* バランス走行時の角度[度] */
#define P_GAIN             2.5F  /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60  /* 完全停止用モータ制御PWM絶対最大値 */

#define TIRE_DIAMETER    100.0F  // diameter of tire in milimater
#define WHEEL_TREAD      150.0F  // distance between the right and left wheels
//#define P_CONST           0.38D  // PID constants determined by Ultimate Gain method
//#define I_CONST           0.06D
//#define D_CONST          0.027D
#define P_CONST           0.46D  // PID constants determined by Ultimate Gain method
#define I_CONST     0.00000013D
#define D_CONST          0.075D
#define SPEED_NORM           45
#define GS_TARGET            45

//#define DEVICE_NAME     "ET0"  /* Bluetooth名 hrp2/target/ev3.h BLUETOOTH_LOCAL_NAMEで設定 */
//#define PASS_KEY        "1234" /* パスキー    hrp2/target/ev3.h BLUETOOTH_PIN_CODEで設定 */
#define CMD_START_R     'R' // R-mode start command
#define CMD_START_r     'r' // R-mode start command
#define CMD_START_L     'L' // L-mode start command
#define CMD_START_l     'l' // L-mode start command
#define CMD_STOP_S      'S'
#define CMD_STOP_s      's'

// machine state
#define ST_takingOff    0
#define ST_tracing_L    1
#define ST_crimbing     2
#define ST_tracing_R    3
#define ST_dancing      4
#define ST_stopping_L   5
#define ST_stopping_R   6
#define ST_landing      7

#define ST_NAME_LEN     20  // maximum number of characters for a machine state name
const char stateName[][ST_NAME_LEN] = {
    "ST_takingOff",
    "ST_tracing_L",
    "ST_crimbing",
    "ST_tracing_R",
    "ST_dancing",
    "ST_stopping_L",
    "ST_stopping_R",
    "ST_landing"
};

// event
#define EVT_cmdStart_L      0
#define EVT_cmdStart_R      1
#define EVT_touch_On        2
#define EVT_touch_Off       3
#define EVT_sonar_On        4
#define EVT_sonar_Off       5
#define EVT_backButton_On   6
#define EVT_backButton_Off  7
#define EVT_bk2bl           8
#define EVT_bl2bk           9
#define EVT_cmdDance        10
#define EVT_cmdCrimb        11
#define EVT_cmdStop         12
#define EVT_line_lost       13
#define EVT_line_found      14
#define EVT_dist_reached    15
#define EVT_tilt            16

#define EVT_NAME_LEN        20  // maximum number of characters for an event name
const char eventName[][EVT_NAME_LEN] = {
    "EVT_cmdStart_L",
    "EVT_cmdStart_R",
    "EVT_touch_On",
    "EVT_touch_Off",
    "EVT_sonar_On",
    "EVT_sonar_Off",
    "EVT_backButton_On",
    "EVT_backButton_Off",
    "EVT_bk2bl",
    "EVT_bl2bk",
    "EVT_cmdDance",
    "EVT_cmdCrimb",
    "EVT_cmdStop",
    "EVT_line_lost",
    "EVT_line_found",
    "EVT_dist_reached",
    "EVT_tilt"
};

// FIR filter parameters
const int FIR_ORDER = 10;
//const double hn[FIR_ORDER+1] = { 2.993565708123639e-03, 9.143668394023662e-03, -3.564197579813870e-02, -3.996625085414179e-02, 2.852028479250662e-01, 5.600000000000001e-01, 2.852028479250662e-01, -3.996625085414179e-02, -3.564197579813870e-02, 9.143668394023662e-03, 2.993565708123639e-03 };
const double hn[FIR_ORDER+1] = { -1.247414986406201e-18, -1.270350182429102e-02, -2.481243022283666e-02, 6.381419731491805e-02, 2.761351394755998e-01, 4.000000000000000e-01, 2.761351394755998e-01, 6.381419731491805e-02, -2.481243022283666e-02, -1.270350182429102e-02, -1.247414986406201e-18 };

// moving average parameter
const int MA_CAP = 10;

/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

#define M_2PI    (2.0 * M_PI)

class Radioman {
private:
    FILE*           bt;      /* Bluetoothファイルハンドル */
public:
    Radioman();
    void operate(); // method to invoke from the task handler
    ~Radioman();
};

class Observer {
private:
    Motor*          leftMotor;
    Motor*          rightMotor;
    TouchSensor*    touchSensor;
    SonarSensor*    sonarSensor;
    GyroSensor*     gyroSensor;
    ColorSensor*    colorSensor;
    double distance, azimuth, locX, locY;
    int16_t traceCnt, prevGS;
    int32_t prevAngL, prevAngR, notifyDistance, gsDiff, timeDiff;
    uint64_t curTime, prevTime;
    bool touch_flag, sonar_flag, backButton_flag, lost_flag, frozen, blue_flag;
    rgb_raw_t cur_rgb;
    hsv_raw_t cur_hsv;
    FIR_Transposed<FIR_ORDER> *fir_r, *fir_g, *fir_b;
    MovingAverage<int32_t, MA_CAP> *ma;
    //OutlierTester*  ot_r;
    //OutlierTester*  ot_g;
    //OutlierTester*  ot_b;

    bool check_touch(void);
    bool check_sonar(void);
    bool check_backButton(void);
    bool check_lost(void);
    bool check_tilt(void);
protected:
public:
    Observer();
    Observer(Motor* lm, Motor* rm, TouchSensor* ts, SonarSensor* ss, GyroSensor* gs, ColorSensor* cs);
    void goOnDuty();
    void reset();
    void notifyOfDistance(int32_t delta);
    int32_t getDistance();
    int16_t getAzimuth();
    int32_t getLocX();
    int32_t getLocY();
    void operate(); // method to invoke from the cyclic handler
    void goOffDuty();
    void freeze();
    void unfreeze();
    ~Observer();
};

class Navigator {
private:
protected:
    int8_t forward;      /* 前後進命令 */
    int8_t turn;         /* 旋回命令 */
    int8_t pwm_L, pwm_R; /* 左右モータPWM出力 */
    int8_t speed;
    int16_t         trace_pwmLR;
    Motor*          leftMotor;
    Motor*          rightMotor;
    PIDcalculator*  ltPid;
public:
    Navigator();
    void goOnDuty();
    virtual void haveControl() = 0;
    virtual void operate() = 0;
    void goOffDuty();
    virtual ~Navigator();
};

class LineTracer : public Navigator {
private:
    int32_t motor_ang_l, motor_ang_r;
    bool    frozen;
protected:
public:
    LineTracer();
    LineTracer(Motor* lm, Motor* rm, Motor* tm);
    void haveControl();
    void operate(); // method to invoke from the cyclic handler
    void setSpeed(int8_t s);
    void freeze();
    void unfreeze();
    ~LineTracer();
};

#include "BlindRunner.hpp"

class Captain {
private:
    TouchSensor*    touchSensor;
    SonarSensor*    sonarSensor;
    ColorSensor*    colorSensor;
    GyroSensor*     gyroSensor;
    Motor*          leftMotor;
    Motor*          rightMotor;
    Motor*          tailMotor;
    Steering*       steering;
    LineTracer*     lineTracer;
    BlindRunner*    blindRunner;
protected:
public:
    Captain();
    void takeoff();
    void decide(uint8_t event);
    void triggerLanding();
    void land();
    ~Captain();
};

extern Captain*     captain;
extern Observer*    observer;
extern Navigator*   activeNavigator;
extern Clock*       clock;
extern uint8_t      state;

#endif /* crew_hpp */
