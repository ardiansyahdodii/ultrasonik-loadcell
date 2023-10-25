// HERE LIES ALL THE DREAMS I WANTED 'makan'

// mind the gap

// Library yang diperlukan
#include <Stepper.h>
#include <HX711_ADC.h> // install dari olkal
#include <ezButton.h>

// deklarasi kelas kalman filter
class kalmanfilter
{
  float initial_state = 0;
  float process_variance = 1;
  float measurement_variance = 0.1;

public:
  float state, kalman_gain;

  kalmanfilter()
  {
    state = initial_state;
    process_variance = process_variance;
    measurement_variance = measurement_variance;
  }

  float update_state(float measurement)
  {
    kalman_gain = process_variance / (process_variance + measurement_variance);
    state = state + kalman_gain * (measurement - state);
    return state;
  }
};

// inisialisasi variabel
kalmanfilter kalman;
unsigned long t = 0;
float last_value = 0;
float berat;

// uno lalal
// pins:
const int HX711_dout = 4; // mcu > HX711 dout pin
const int HX711_sck = 5;  // mcu > HX711 sck pin

// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// Definisikan jumlah langkah per putaran pada motor stepper yang Anda gunakan
const int stepsPerRevolution = 200;
const int stepsPerRevolution1 = 200;

// Inisialisasi objek Stepper dengan jumlah langkah per putaran dan pin kontrol
Stepper myStepper1(stepsPerRevolution, 2, 3, 4, 5);      // naik dan turun
Stepper myStepper2(stepsPerRevolution1, 6, 7, 8, 9);     // kanan dan kiri
Stepper myStepper3(stepsPerRevolution, 10, 11, 12, 13);  // naik dan turun
Stepper myStepper4(stepsPerRevolution1, 22, 23, 24, 25); // kanan dan kiri

ezButton button1(40);
ezButton button2(41);
ezButton button3(42);
ezButton button4(43);

// Sensor 1
#define trig1 30
#define echo1 31

// Sensor 2
#define trig2 32
#define echo2 33

// Sensor 3
#define trig3 34
#define echo3 35

// pengondisian untuk logika
bool naik, naik2, counter, counter2;
bool mengukur;
bool newDataReady;
int langkah, langkah2, langkahSamping, langkahSamping2, noDetect, noDetect2;
