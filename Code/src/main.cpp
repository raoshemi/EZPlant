#include <Arduino.h>
#define PT_USE_TIMER
#define PT_USE_SEM
#include <pt-sem.h>
#include <pt-timer.h>
#include <pt.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <avr/pgmspace.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <wifi.h>
#include <byte_fifo.h>


// Big lookup Table (approx 750 entries), subtract 238 from ADC reading to start at 0*C. Entries in 10ths of degree i.e. 242 = 24.2*C Covers 0*C to 150*C For 10k resistor/10k thermistor voltage divider w/ therm on the + side.
const int temps[] PROGMEM = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 143, 144, 145, 146, 147, 148, 149, 150, 151, 151, 152, 153, 154, 155, 156, 157, 158, 159, 159, 160, 161, 162, 163, 164, 165, 166, 167, 167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 181, 182, 182, 183, 184, 185, 186, 187, 188, 189, 190, 190, 191, 192, 193, 194, 195, 196, 197, 197, 198, 199, 200, 201, 202, 203, 204, 205, 205, 206, 207, 208, 209, 210, 211, 212, 212, 213, 214, 215, 216, 217, 218, 219, 220, 220, 221, 222, 223, 224, 225, 226, 227, 228, 228, 229, 230, 231, 232, 233, 234, 235, 235, 236, 237, 238, 239, 240, 241, 242, 243, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 255, 256, 257, 258, 259, 260, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 422, 423, 424, 425, 426, 427, 428, 429, 430, 432, 433, 434, 435, 436, 437, 438, 439, 441, 442, 443, 444, 445, 446, 448, 449, 450, 451, 452, 453, 455, 456, 457, 458, 459, 460, 462, 463, 464, 465, 466, 468, 469, 470, 471, 472, 474, 475, 476, 477, 479, 480, 481, 482, 484, 485, 486, 487, 489, 490, 491, 492, 494, 495, 496, 498, 499, 500, 501, 503, 504, 505, 507, 508, 509, 511, 512, 513, 515, 516, 517, 519, 520, 521, 523, 524, 525, 527, 528, 530, 531, 532, 534, 535, 537, 538, 539, 541, 542, 544, 545, 547, 548, 550, 551, 552, 554, 555, 557, 558, 560, 561, 563, 564, 566, 567, 569, 570, 572, 574, 575, 577, 578, 580, 581, 583, 585, 586, 588, 589, 591, 593, 594, 596, 598, 599, 601, 603, 604, 606, 608, 609, 611, 613, 614, 616, 618, 620, 621, 623, 625, 627, 628, 630, 632, 634, 636, 638, 639, 641, 643, 645, 647, 649, 651, 653, 654, 656, 658, 660, 662, 664, 666, 668, 670, 672, 674, 676, 678, 680, 683, 685, 687, 689, 691, 693, 695, 697, 700, 702, 704, 706, 708, 711, 713, 715, 718, 720, 722, 725, 727, 729, 732, 734, 737, 739, 741, 744, 746, 749, 752, 754, 757, 759, 762, 764, 767, 770, 773, 775, 778, 781, 784, 786, 789, 792, 795, 798, 801, 804, 807, 810, 813, 816, 819, 822, 825, 829, 832, 835, 838, 842, 845, 848, 852, 855, 859, 862, 866, 869, 873, 877, 881, 884, 888, 892, 896, 900, 904, 908, 912, 916, 920, 925, 929, 933, 938, 942, 947, 952, 956, 961, 966, 971, 976, 981, 986, 991, 997, 1002, 1007, 1013, 1019, 1024, 1030, 1036, 1042, 1049, 1055, 1061, 1068, 1075, 1082, 1088, 1096, 1103, 1110, 1118, 1126, 1134, 1142, 1150, 1159, 1168, 1177, 1186, 1196, 1206, 1216, 1226, 1237, 1248, 1260, 1272, 1284, 1297, 1310, 1324, 1338, 1353, 1369, 1385, 1402, 1420, 1439, 1459, 1480, 1502};
// output
#define EC_POWER1_PIN PB9
#define EC_POWER2_PIN PB8
#define CLK_PIN PB6
#define SDA_PIN PB7
#define TIMER_PLUG_PIN PA2
#define NUTRITION_SUPPLY_PIN PA3
#define DRAIN_PUMP_PIN PA9
#define WATER_SUPPLY_PIN PA8
// input
#define WATER_FULL_PIN PB3
#define WATER_EMPTY_PIN PB4
#define WATER_OVERFLOW_PIN PB5
#define DHT_PIN PC14
#define THERM_PIN PB1 // 10ktherm & 10k resistor as divider.
#define EC_PIN1 PA0
#define EC_PIN2 PA1
#define WIFI_RESET PA10


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

// 符号点阵库
uint8_t icon_temp[8]  = {0x4, 0xa, 0xa, 0xa, 0xe, 0x1f, 0x1f, 0xe};
uint8_t icon_humi[8]  = {0x4, 0x4, 0xa, 0x1b, 0x11, 0x11, 0xe, 0x0};
//uint8_t icon_sun[8]   = {0x1f,0x1c,0x1c,0x1a,0x9,0x8,0x0,0x0};
uint8_t icon_sun[8]   = {0x9, 0x1a, 0x1c, 0x1f, 0x1c, 0x1a, 0x9, 0x0};
uint8_t icon_ec[8]    = {0x1c, 0x10, 0x1c, 0x13, 0x1c, 0x4, 0x4, 0x3};
uint8_t icon_wifi[8]    = {0x0, 0x1f, 0x0, 0xe, 0x0, 0x4, 0x0, 0x0};
uint8_t icon_nowifi[8]    = {0x1, 0x1d, 0x1, 0xc, 0x1, 0x4, 0x0, 0x0};
uint8_t icon_lx[8]    = {0x10, 0x10, 0x10, 0x10, 0x15, 0x12, 0x15, 0x0};
uint8_t icon_c[8]    = {0x10, 0x6, 0x9, 0x8, 0x8, 0x9, 0x6, 0x0};

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
int BH1750address = 0x23;//照度BH1750 I2C地址

//******************************全局常量****************************
#define   EC_CHECK_INTERVAL           5000        //EC值检测时间间隔
#define   EC_METER_LATENCY            500        // EC读数延迟
#define   EC_SUPLY_LATENCY            3000        // 浓缩液补充时间
#define   EC_SUPLY_INTERVAL           300000       // 浓缩液补充时间间隔
#define   CONNECTION_CHECK_INTERVAL   60000       //网络连接检测时间间隔
#define   WATER_LEVEL_CHECK_INTERVAL  1000        //水位检测时间间隔
#define   REPORT_INTERVAL             10000       // 上报数据间隔

typedef enum
{
    AIR_LINK = 0,
    DISCONNECT,
    RUNNING,
    EC_TOO_HIGH,
    EC_TOO_LOW,
    WATER_EMPTY,
    WATER_FULL
} STATUS_T;

STATUS_T status;

static struct pt 
    th_wifi_read,
    th_read_ec,
    th_read_solution_temp,
    th_check_ec,
    th_ec_supply,
    th_read_dht,
    th_read_illumination,
    th_timer_plug,
    th_report,
    th_lcd_display,
    th_check_water,
    th_water_supply,
    th_timer_power,
    th_led;


WIFI_STATUS_T wifi_status;
WIFI_COMMAND_STATUS_T wifi_command_status;
byte illuminationBuff[2];
uint32_t illumination;
int humidity, temperature;
DHT dht(PC14, DHT11);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];
int R         = 500;     // EC接入电阻
int Ra        = 25;       // 供电口内阻
float PPMconversion = 0.7;
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring

float solution_temperature = 10;
float solution_temperature_sum = 0;
int solution_temperature_count = 0;
uint32_t ec_standard = 800;
float EC = 0;
float EC1 = 0;
float EC2 = 0;
uint32_t EC25 = 0;
uint32_t ppm = 0;

float raw = 0;
float Vin = 3.3;
float Vdrop = 0;
float Rc = 0;
float K = 1.028;
int zheng = 1;

String returnStr = "";
// 定时插座设置
struct TimerPlug {
    bool enabled = 0;
    uint32_t on_min = 0;
    uint32_t off_min = 0;
};
TimerPlug myTimerPlug;
uint32_t power_on_sec;
uint32_t power_off_sec;
uint32_t power_on_sum = 0;
uint32_t power_off_sum = 0;
boolean is_power_on = false;

uint8_t buf[512];
STU_BYTE_QUEUE data_queue;


boolean is_ec_too_high = false;
boolean is_water_full = false;
boolean is_water_empty = false;
boolean is_water_overflow = false;

void clearStatus() {
    lcd.setCursor(0, 3);
    lcd.print("                    ");
}

void lcdPrintStatus(String str) {
    clearStatus();
    lcd.setCursor(0, 3);
    lcd.print(str);
}
static int readSolutionTemperature(struct pt *pt)
{
    int therm;
    float RAW;
    PT_BEGIN(pt);
    while(1) {
        RAW = analogRead(THERM_PIN);
        RAW = (RAW / 4095) * 1023;
        therm = RAW - 273;
        therm = pgm_read_word(&temps[therm]);

        float TEMPC;
        TEMPC = therm;
        TEMPC = TEMPC / 10;
        if(solution_temperature_count < 20) {
            solution_temperature_sum += TEMPC;
            solution_temperature_count ++;
        } else {
            solution_temperature_sum = (solution_temperature_sum / 20) * 19 + TEMPC;
            solution_temperature_count = 20;
        }
        solution_temperature = solution_temperature_sum / solution_temperature_count;
        PT_TIMER_DELAY(pt, 2000);
    }
    PT_END(pt);
}
static int readDHT(struct pt *pt) {
    PT_BEGIN(pt);
    while (1) {
        temperature = dht.readTemperature();
        if(temperature > 50) {
            temperature = 50;
        }
        if(temperature < -50) {
            temperature = -50;
        }
        humidity = dht.readHumidity();
        if(humidity > 99) {
            humidity = 99;
        }
        if(humidity < 0) {
            humidity = 0; 
        }
        PT_TIMER_DELAY(pt, 1000);
    }
    PT_END(pt);
}

static int readEC(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        if (zheng == 1) {
            pinMode(EC_PIN1, INPUT);
            pinMode(EC_PIN2, OUTPUT);
            digitalWrite(EC_PIN2, LOW);
            digitalWrite(EC_POWER1_PIN, HIGH);
            raw = analogRead(EC_PIN1);
            raw = analogRead(EC_PIN1); // This is not a mistake, First reading will be low beause if charged a capacitor
            digitalWrite(EC_POWER1_PIN, LOW);
            Vdrop = (Vin * raw) / 4096.0;
            Rc = (Vdrop * R) / (Vin - Vdrop);
            Rc = Rc - Ra; //acounting for Digital Pin Resitance
            EC1 = 1000 / (Rc * K);
            // if(EC1 < 0) EC1 = 0;
            zheng = 0;
        }
        else
        {
            pinMode(EC_PIN2, INPUT);
            pinMode(EC_PIN1, OUTPUT);
            digitalWrite(EC_PIN1, LOW);
            digitalWrite(EC_POWER2_PIN, HIGH);
            raw = analogRead(EC_PIN2);
            raw = analogRead(EC_PIN2); // This is not a mistake, First reading will be low beause if charged a capacitor
            digitalWrite(EC_POWER2_PIN, LOW);
            Vdrop = (Vin * raw) / 4096.0;
            Rc = (Vdrop * R) / (Vin - Vdrop);
            Rc = Rc - Ra; //acounting for Digital Pin Resitance
            EC2 = 1000 / (Rc * K);
            // if(EC2 < 0) EC2 = 0;
            zheng = 1;
        }
        EC = (EC1 + EC2) / 2;
        if(EC == EC1 / 2 || EC == EC2 / 2) {
            EC = EC * 2;
        }
        if(EC < 0) EC = 0;
        EC25 = 1000 * EC / (1 + TemperatureCoef * (solution_temperature - 25.0));
        ppm = (EC25) * (PPMconversion);
        PT_TIMER_DELAY(pt, EC_CHECK_INTERVAL);
    }
    PT_END(pt);
}

static int readIllumination(struct pt *pt) {
    int i;
    PT_BEGIN(pt);
    while(1) {
        Wire.beginTransmission(BH1750address);
        Wire.write(0x10);
        Wire.endTransmission();
        PT_TIMER_DELAY(pt, 200);
        Wire.beginTransmission(BH1750address);
        Wire.requestFrom(BH1750address, 2);
        i = 0;
        while (Wire.available()) {
            illuminationBuff[i] = Wire.read();
            i++;
        }
        if (2 == i) {
            illumination = ((illuminationBuff[0] << 8) | illuminationBuff[1]) / 1.2;
            if(illumination > 65535) {
                illumination = 65535;
            }
        }
        Wire.endTransmission();
        PT_TIMER_DELAY(pt, 1000);
    }
    PT_END(pt);
}
static int timerPlug(struct pt * pt) {
    PT_BEGIN(pt);
    while (1) {
        PT_WAIT_UNTIL(pt, myTimerPlug.enabled == 1);
        if(is_power_on) {
            power_on_sum ++;
        }
        else {
            power_off_sum ++;
        }
        if(power_on_sum > power_on_sec) {
            is_power_on = false;
            digitalWrite(TIMER_PLUG_PIN, LOW);
            power_on_sum = 0;
            lcdPrintStatus("Plug off!");
            PT_TIMER_DELAY(pt, 1000);
            lcdPrintStatus("Working...");
        }
        if(power_off_sum > power_off_sec) {
            is_power_on = true;
            digitalWrite(TIMER_PLUG_PIN, HIGH);
            power_off_sum = 0;
            lcdPrintStatus("Plug on!");
            PT_TIMER_DELAY(pt, 1000);
            lcdPrintStatus("Working...");
        }
        PT_TIMER_DELAY(pt, 1000);
    }
    PT_END(pt);
}
static int lcdDisplay(struct pt *pt) {
    String str_h, str_temp, str_l, str_ec;
    PT_BEGIN(pt);
    while(1) {
        str_h = String("") + humidity;
        str_temp = String("") + temperature;
        str_l = String("") + illumination;
        str_ec = String("") + EC25;
        lcd.setCursor(4, 1);
        for (int i = str_temp.length(); i < 3; i ++) {
            lcd.print(" ");
        }
        lcd.print(str_temp);
        lcd.setCursor(16, 1);
        lcd.print(str_h);
        lcd.setCursor(2, 2);
        for (int i = str_ec.length(); i < 5.; i ++) {
            lcd.print(" ");
        }
        lcd.print(str_ec);
        lcd.setCursor(13, 2);
        for (int i = str_l.length(); i < 5; i ++) {
            lcd.print(" ");
        }
        lcd.print(str_l);
        if(wifi_status == WIFI_CIP_SEND) {
            lcd.setCursor(19, 0);
            lcd.printByte(4);
        } else {
            lcd.setCursor(19, 0);
            lcd.printByte(5);
        }
        PT_TIMER_DELAY(pt, 200);
    }
    PT_END(pt);
}

static int checkEC(struct pt *pt) {  //循环
    PT_BEGIN(pt);
    while (1) {
        PT_WAIT_UNTIL(pt, EC25 > 0);
        if (ec_standard - EC25 > 100) {
            digitalWrite(NUTRITION_SUPPLY_PIN, HIGH);
            PT_TIMER_DELAY(pt, EC_SUPLY_LATENCY);
            digitalWrite(NUTRITION_SUPPLY_PIN, LOW);
        }
        if (EC25 > ec_standard + 200)  {
            if (!is_ec_too_high) {
                is_ec_too_high = true;
                if (status == RUNNING) {
                    status = EC_TOO_HIGH;
                }
                // 发送到云端

            }
        }
        else {
            if (is_ec_too_high) {
                is_ec_too_high = false;
                status = RUNNING;
                // 发送到云端
              
            }
        }
        PT_TIMER_DELAY(pt, EC_SUPLY_INTERVAL);
    }
    PT_END(pt);
}
static int checkWater(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        if(digitalRead(WATER_OVERFLOW_PIN)) {
            if(!is_water_overflow) {
                is_water_overflow = true;
                digitalWrite(DRAIN_PUMP_PIN, HIGH);
            }
        }
        if(digitalRead(WATER_FULL_PIN)) {
            if(is_water_empty) {
                is_water_empty = false;
                digitalWrite(WATER_SUPPLY_PIN, LOW);
            }
        } else {
            if(is_water_overflow) {
                is_water_overflow = false;
                digitalWrite(DRAIN_PUMP_PIN, LOW);
            }
        }
        if(!digitalRead(WATER_EMPTY_PIN)) {
            if(!is_water_empty) {
                is_water_empty = true;
                digitalWrite(WATER_SUPPLY_PIN, HIGH);
            }
        }
        PT_TIMER_DELAY(pt, WATER_LEVEL_CHECK_INTERVAL);
    }
    PT_END(pt);
}
void wifiAirkiss() {
    is_airkiss_mode = true;
    Serial3.println(AT_RESET);
}
PT_THREAD(wifiRead(struct pt *pt)) {
    PT_BEGIN(pt);
    while(1) {
        if(is_airkiss_mode) {
            lcdPrintStatus("delay 2s");
            PT_TIMER_DELAY(pt, 2000);
            Serial3.println(AT_CWMODE_DEF_1);
            lcdPrintStatus("delay 2.5s");
            PT_TIMER_DELAY(pt, 2500);
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_OK) != -1) {
                    lcdPrintStatus("AT_CWMODE_DEF_1 = ok");
                    Serial3.println(AT_CWAUTOCONN_1);
                    returnStr = "";
                    break;
                }
            }
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_OK) != -1) {
                    lcdPrintStatus("AT_CWAUTOCONN_1 = ok");
                    Serial3.println(AT_CWSTARTSMART_3);
                    returnStr = "";
                    break;
                }
            }
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_OK) != -1) {
                    lcdPrintStatus("Begin Airkiss");
                    returnStr = "";
                    break;
                }
            }
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            lcdPrintStatus("received");
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_WIFI_GOT_IP) != -1) {
                    wifi_status = WIFI_GOT_IP;
                    returnStr = "";
                    lcdPrintStatus("wifi got ip");
                    break;
                }
            }
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_AIRKISS_FINISH) != -1) {
                    Serial3.println(AT_CWSTOPSMART_3);
                    returnStr = "";
                    lcdPrintStatus("Airkiss finished");
                    break;
                }
            }
            PT_WAIT_UNTIL(pt, Serial3.available() > 0);
            while(Serial3.available()) {
                returnStr += char(Serial3.read());
                PT_TIMER_DELAY(pt, 1);
                if(returnStr.indexOf(RET_OK) != -1) {
                    is_airkiss_mode = false;
                    returnStr = "";
                    lcdPrintStatus("airkiss finish");
                    break;
                }
            }
        } else {
            if(wifi_status == WIFI_POWER_ON) {
                lcdPrintStatus("power on");
                PT_WAIT_UNTIL(pt, Serial3.available() > 0);
                PT_TIMER_DELAY(pt, 2000);
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 2);
                    if(returnStr.indexOf(RET_READY) != -1) {
                        lcdPrintStatus("wifi ready");
                        wifi_status = WIFI_READY;
                        returnStr = "";
                        break;
                    }
                }
            }
            if(wifi_status == WIFI_READY) {
                lcdPrintStatus("ready");
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_WIFI_CONNECTED) != -1) {
                        lcdPrintStatus("wifi connected");
                        wifi_status = WIFI_CONNECTED;
                        returnStr = "";
                        break;
                    }
                }
            }
            if(wifi_status == WIFI_READY) {
                lcdPrintStatus("ready");
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_WIFI_DISCONNECT) != -1) {
                        wifi_status = WIFI_DISCONNECT;
                        returnStr = "";
                        lcdPrintStatus("wifi disconnect");
                        break;
                    }
                }
            }
            if(wifi_status == WIFI_CONNECTED) {
                lcdPrintStatus("connected");
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_WIFI_GOT_IP) != -1) {
                        wifi_status = WIFI_GOT_IP;
                        returnStr = "";
                        lcdPrintStatus("wifi Got ip");
                        break;
                    }
                }
            }
            if(wifi_status == WIFI_GOT_IP) {
                lcdPrintStatus("got ip");
                Serial3.println(AT_CIPMODE_1);
                PT_TIMER_DELAY(pt, 2000);
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_OK) != -1) {
                        Serial3.println(AT_CIPMUX_0);
                        returnStr = "";
                        lcdPrintStatus("AT_CIPMODE_1 = ok");
                        PT_TIMER_DELAY(pt, 1000);
                        break;
                    }
                }
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_OK) != -1) {
                        lcdPrintStatus("AT_CIPMUX_0 = ok");
                        returnStr = "";
                        Serial3.println(AT_CIPSTART);
                        PT_TIMER_DELAY(pt, 4000);
                        break;
                    }
                }
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_OK) != -1) {
                        lcdPrintStatus("AT_CIPSTART = ok");
                        Serial3.println(AT_CIPSEND);
                        returnStr = "";
                        PT_TIMER_DELAY(pt, 2000);
                        break;
                    }
                }
                PT_WAIT_UNTIL(pt, Serial3.available());
                while(Serial3.available() > 0) {
                    returnStr += char(Serial3.read());
                    PT_TIMER_DELAY(pt, 1);
                    if(returnStr.indexOf(RET_OK) != -1) {
                        lcdPrintStatus("AT_CIPSEND = ok");
                        wifi_status = WIFI_CIP_SEND;
                        returnStr = "";
                        break;
                    }
                }
                
            }
            if(wifi_status == WIFI_CIP_SEND) {
                returnStr = "";
                PT_WAIT_UNTIL(pt, Serial3.available());
                while (Serial3.available() > 0)
                {
                    
                    // readPacket();
                }
            }
        }
    }
    PT_END(pt);
}
// 各模块初始化
void ECInit()
{
    pinMode(THERM_PIN, INPUT_ANALOG);
    // pinMode(EC_PIN1, INPUT);
    // pinMode(EC_PIN2, INPUT);
    pinMode(EC_POWER1_PIN, OUTPUT);
    pinMode(EC_POWER2_PIN, OUTPUT);
}
void LCDInit() {
    lcd.init(SDA_PIN, CLK_PIN);
    lcd.backlight();
    lcd.createChar(0, icon_temp);
    lcd.createChar(1, icon_humi);
    lcd.createChar(2, icon_sun);
    lcd.createChar(3, icon_ec);
    lcd.createChar(4, icon_wifi);
    lcd.createChar(5, icon_nowifi);
    lcd.createChar(6, icon_lx);
    lcd.createChar(7, icon_c);
    lcd.home();

    delay(100);
    lcd.setCursor(3, 0);
    lcd.print("Hello EZPlant!");
    lcd.setCursor(19, 0);
    lcd.printByte(4);
    lcd.setCursor(0, 1);
    lcd.printByte(0);
    lcd.setCursor(8, 1);
    lcd.printByte(7);
    lcd.setCursor(11, 1);
    lcd.printByte(1);
    lcd.setCursor(19, 1);
    lcd.print("%");
    lcd.setCursor(0, 2);
    lcd.printByte(3);
    lcd.setCursor(11, 2);
    lcd.printByte(2);
    lcd.setCursor(19, 2);
    lcd.printByte(6);

    lcd.setCursor(5, 1);
    lcd.print("--");
    lcd.setCursor(16, 1);
    lcd.print("--");
    lcd.setCursor(3, 2);
    lcd.print("----");
    lcdPrintStatus("Starting...");
}
void WifiInit() {
    Serial3.begin(115200);
    while(!Serial3) {
        Serial3.print(".");
    }
    wifi_status = WIFI_POWER_ON;
}
// 主程序
void setup()
{

    // put your setup code here, to run once:
    ECInit();
    dht.begin();
    Wire.begin();
    delay(1000);

    queue_byte_init(&data_queue, buf, 512);

    pinMode(WATER_EMPTY_PIN, INPUT);
    pinMode(WATER_FULL_PIN, INPUT);
    pinMode(WATER_OVERFLOW_PIN, INPUT);
    pinMode(NUTRITION_SUPPLY_PIN, OUTPUT);
    pinMode(DRAIN_PUMP_PIN, OUTPUT);
    pinMode(TIMER_PLUG_PIN, OUTPUT);
    pinMode(WATER_SUPPLY_PIN, OUTPUT);

    LCDInit();
    PT_INIT(&th_wifi_read);
    // PT_INIT(&th_led);
    PT_INIT(&th_read_ec);
    PT_INIT(&th_read_solution_temp);
    PT_INIT(&th_check_ec);
    PT_INIT(&th_read_dht);
    PT_INIT(&th_read_illumination);
    PT_INIT(&th_timer_plug);
    // PT_INIT(&th_report);
    PT_INIT(&th_lcd_display);
    PT_INIT(&th_check_water);
    WifiInit();
}

void loop()
{
    wifiRead(&th_wifi_read);
    readDHT(&th_read_dht);
    readIllumination(&th_read_illumination);
    readSolutionTemperature(&th_read_solution_temp);
    readEC(&th_read_ec);
    checkEC(&th_check_ec);
    lcdDisplay(&th_lcd_display);
    checkWater(&th_check_water);
    timerPlug(&th_timer_plug);

    long now = millis();
    // lcdPrintStatus((String)now);
  }
