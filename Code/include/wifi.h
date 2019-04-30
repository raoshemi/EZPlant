

typedef enum {
    WIFI_POWER_ON,
    WIFI_READY,
    WIFI_CONNECTED,
    WIFI_GOT_IP,
    WIFI_CIP_SEND,
    WIFI_DISCONNECT
} WIFI_STATUS_T;

typedef enum {
    COMMAND_SENT,
    COMMAND_ERROR,
    COMMAND_OK
} WIFI_COMMAND_STATUS_T;

const String AT_RESET = "AT+RST";
// airkiss
const String AT_CWMODE_DEF_1 = "AT+CWMODE_DEF=1";
const String AT_CWAUTOCONN_1 = "AT+CWAUTOCONN=1";
const String AT_CWSTARTSMART_3 = "AT+CWSTARTSMART=3";
const String AT_CWSTOPSMART_3 = "AT+CWSTOPSMART=3";
// 透传
const String AT_CIPMODE_1 = "AT+CIPMODE=1";
const String AT_CIPMUX_0 = "AT+CIPMUX=0";
const String AT_CIPSTART = "AT+CIPSTART=\"TCP\",\"cloud.ezplant.cn\",13399";
const String AT_CIPSEND = "AT+CIPSEND";

// AT返回
const String RET_OK = "OK\r\n";
const String RET_ERROR = "ERROR\r\n";
const String RET_WIFI_CONNECTED = "WIFI CONNECTED\r\n";
const String RET_WIFI_GOT_IP = "WIFI GOT IP\r\n";
const String RET_WIFI_DISCONNECT = "WIFI DISCONNECT\r\n";
const String RET_READY = "ready\r\n";
const String RET_AIRKISS_START = "smartconfig type:AIRKISS\r\n";
const String RET_AIRKISS_FINISH = "smartconfig connected wifi\r\n";



boolean is_wifi_started = false;
boolean is_wifi_connected = false;
boolean is_command_sent = false;
boolean is_command_successed = false;
boolean is_airkiss_mode = false;

    

