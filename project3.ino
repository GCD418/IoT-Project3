//Wifi
#include "WifiManager.h"
//AWS
#include "AwsIotManager.h"
#include "JsonManager.h"
//Servo
#include "ServoMotor.h"
//NFC
#include <Wire.h>
#include "NFCReader.h"

const char* WIFI_SSID = "RedG";
const char* WIFI_PASS = "pequebambino";

const char* MQTT_BROKER = "a13ek52zllxy0k-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32";
const char* UPDATE_TOPIC = "$aws/things/iot_thing/shadow/update";
const char* UPDATE_DELTA_TOPIC = "$aws/things/iot_thing/shadow/update/delta";

#define SDA_PIN 21
#define SCL_PIN 22
String UIDS[] = {"9C3D4464", "43768013", "058A0D5C064200"};
String VALUES[] = {"Gabriel", "Ostin", "Chris"};

String getName(String UIDTT){
  for(int i = 0; i < 3; i ++)
  {
    if(UIDTT == UIDS[i])
      return VALUES[i];
  }
  return "Usuario no registrado";
}
String lastUser = "";


const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
  -----BEGIN CERTIFICATE-----
  MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
  ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
  b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
  MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
  b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
  ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
  9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
  IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
  VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
  93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
  jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
  AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
  A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
  U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
  N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
  o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
  5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
  rqXRfboQnoZsG4q5WTP468SQvvG5
  -----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
  -----BEGIN CERTIFICATE-----
  MIIDWTCCAkGgAwIBAgIUR/9Xp6usZ8ONYoOu+1R2yQd27u0wDQYJKoZIhvcNAQEL
  BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
  SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDUxMjIwNDMy
  M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
  ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALyK1znD0Tco1g8f/f+L
  ILOmY2P6evL7kRf7kFbHrOsNQSI2oTUlRJ/BBjFXO8u9LvVlKcht0iMjfX/kB5/o
  edtEaekOwp4+WoTREmd2YYmeJOoy/1rzTpuZoao6doGEUhOFyC3NGMicxUNlyTSn
  Zslypvdj8lAiIeE0VhW/ibEdvIN8lFEbXm/MXytZzr5RrtZuAaHyWsHiBISDGOS7
  LHljF6BPtoByJvew8puAGeAG/VxlbHtvvqhxVn+i7piVokXAcYy7AnKuBARrJtlL
  SsshHCVXSPArMux4qfJp9kbGHhG2ZpGo9b46mcaG09KDl/ebG1qZ3Z3fOlJGPbiE
  eRMCAwEAAaNgMF4wHwYDVR0jBBgwFoAU8iTY2Ni4zS69E2m1kzAZZSoEI5EwHQYD
  VR0OBBYEFJaCA5cPQ1GUr4xpq1RG9NgugxDHMAwGA1UdEwEB/wQCMAAwDgYDVR0P
  AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQA0O8pBRsS4Dhl+4Lenz8drbSiS
  9R8FBNkb5qHiNeY1iDwQr2MZKqeyJNCOzJ+kwx7Wjpde77azOV9oI51XtNCSCUL7
  hJeseXMWkHc6gACZsMzdKTp2o4kiygQ+TpnLJqIkmSlVZokpto3r9bWexWE873l0
  HFVPC/6me6IRdvRxOUeVWzUbJ2kJY6Ds6iNwGNG05LFoj+DQzoxkYIcDM8DVtVKU
  OXWQ/Aw3aZLn5vwyiiVf1VZg4sM6DYC9qY4D6fFXOjmkz3Zs6T0uc/6mPD13XggH
  wZvv6FYAgqVlvxwLjssw4XAICF1bIxgVlpEwbUm23BIGBZAA0FXBrRByO/tw
  -----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
  -----BEGIN RSA PRIVATE KEY-----
  MIIEpQIBAAKCAQEAvIrXOcPRNyjWDx/9/4sgs6ZjY/p68vuRF/uQVses6w1BIjah
  NSVEn8EGMVc7y70u9WUpyG3SIyN9f+QHn+h520Rp6Q7Cnj5ahNESZ3ZhiZ4k6jL/
  WvNOm5mhqjp2gYRSE4XILc0YyJzFQ2XJNKdmyXKm92PyUCIh4TRWFb+JsR28g3yU
  URteb8xfK1nOvlGu1m4BofJaweIEhIMY5LsseWMXoE+2gHIm97Dym4AZ4Ab9XGVs
  e2++qHFWf6LumJWiRcBxjLsCcq4EBGsm2UtKyyEcJVdI8Csy7Hip8mn2RsYeEbZm
  kaj1vjqZxobT0oOX95sbWpndnd86UkY9uIR5EwIDAQABAoIBAQCYkSEoNTSPFhRh
  mNnId6S2uuWieYYUKU13wrFCB8H9UnRqYxqADvBA9ck1FAnnC+d726i772Ij3Qwo
  +TTSSSSSMgIh/TevJXXKFAR4hGrZGYd1nF7WEhyB/Qe84CIVHCXudktOBLERZQqb
  RzpRyUR7rrM9bTsfbP9NZTRj1OJu1uS3g85sS7ZN7mIcTUTWagQjBXSQUKPn2T3e
  8tIgrVer10ROsxo0Z/iwELS5oCLALsp3zOTHgrlfodvFwb+TXy8EcuTENWs9bsY4
  /WKjvZeP2XmJaAtaCTgbhKXPmTgGMzoDg4yasMddS4RUmqLc8RSR6PmT5eUs7tHQ
  xWu24f0BAoGBAOx1A+7w7+Tz150N/XX5pWB2p/wdxmi4WQkfydpQaGCv8wk5A7+8
  y9BTkyqJHSIqDCOAZTrp3+SXlFEGV7HpmGKWrzjMObE7MyhpU3H1MYsCIPAvZXtD
  UOJZn94rqZU14iBvAEzBfmaEaxBMn/xJeL4v00yeB4Q4iiRi/08uBEFjAoGBAMwg
  Cj8dO/Oz7gnatW70uDoTQ2DSChh1wczlb0kREA/nVVTuHZ4jS+ieH7B3AIweeSiI
  DEJOwT1/EGff8/kJ00Jenck6GnDokr6uk87WRi7JRBgsnso6Z6+DTKTn7d+LGfpb
  75pN63KKs6951FitNWaRYRreP9EjHxp+XHWHidCRAoGBAI8mmGcDvnghl5shaR0P
  MiugKjrb285yLJT1hcKBg29llgEzXy2pIsrQjfvmXpjcU9T6Wk68YCuICOa0rEqt
  XO/ywcnk+dhuOp5whXAsPP4nH8HJL+rb6g/eqoZacKRAptFz4f6V9ewipp6LhR+7
  aznYwldUJMDLafZ1aJOLDFB5AoGAW51NvAH89GYfQiD9KW2AeeBHb18V1OCVOMcI
  6XxU/y5OqC2u8VjaKu89PdBLWJgcvMpC5ke+h0FONbDYKT/1p2Yb30LKtz5LDnYA
  NOcK3NjMTSVJp24LN0Tw01qc8qXV5P36TgEG2YPTHcDKQ3qJ6xz4vjxd3BmtThDE
  oqNQgWECgYEAnjyTWFaZMlLfa/0XrLxRhvB1aaFSCmWpBCPp0Sljdsjcznt8zH7Q
  VjAm6opRq0fC6PBq5Z+pdy76x8FSj+JaiqNaCfjc4mpS8aMRUhLjw5twOMjOOrU0
  glnP98AIgBipJ8Bzl+3Pm/xt9huDAPjjW+JFloFPcawixGRntG1+5C8=
  -----END RSA PRIVATE KEY-----
)KEY";

WifiManager wifiManager(WIFI_SSID, WIFI_PASS);
AwsIotManager awsIotManager(MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC, UPDATE_DELTA_TOPIC);

NFCReader nfcReader(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  delay(2000);

  wifiManager.connect();

  awsIotManager.setup(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
  delay(1000);
  nfcReader.begin();
}

void loop() {
  wifiManager.keepConnection();

  awsIotManager.loop();

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  if(!nfcReader.isConnected())
  {
    nfcReader.reconnect();
  }
  if (nfcReader.scanForCard(uid, &uidLength))
  {
    nfcReader.printCardDetails(uid, uidLength);

    String uidStr = nfcReader.getUidAsString(uid, uidLength);
    Serial.print("UID as string: ");
    Serial.println(uidStr);
    if(lastUser != getName(uidStr)){
      lastUser = getName(uidStr);
      Serial.print("Last user: ");
      Serial.println(lastUser);
      awsIotManager.reportUser(lastUser);
    }
    delay(700);

  }
  delay(10);
}
