#include <HX711_ADC.h>

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

kalmanfilter kalman;
unsigned long t = 0;
float last_value = 0;
float berat;

// pins:
const int HX711_dout = 4; // mcu > HX711 dout pin
const int HX711_sck = 5;  // mcu > HX711 sck pin

// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

#define trig 2
#define echo 3

bool mengukur;
bool newDataReady;
float filtered_value;

void setup()
{
  // load cell
  LoadCell.begin();
  unsigned long stabilizingtime = 10000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;
  LoadCell.setCalFactor(20.22);
  LoadCell.start(stabilizingtime, _tare);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);

  mengukur = true;
}

void loop()
{
  // Load cell
  //  if (LoadCell.update())
  //    newDataReady = true;
  //  if (newDataReady)
  //  {
  //    for (int i = 0; i < 10; i++)
  //    {
  //      float value = LoadCell.getData();
  //      filtered_value = kalman.update_state(value);
  //      Serial.print(filtered_value);
  //      Serial.print(",");
  //      newDataReady = false;
  //      berat = filtered_value;
  //      delay(50);
  //    }
  //    if (filtered_value > 1000)
  //    {
  //      weight();
  //      tinggi();
  //    }
  //  }
  weight();
  tinggi();
}

void tinggi()
{
  int counter = 0;

  while (counter < 5)
  {
    Serial.println("U");

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long duration, distance;
    duration = pulseIn(echo, HIGH);
    distance = (duration * 0.0343) / 2;
    //  Serial.print("jarak : ");
    Serial.println(distance);
    //  Serial.println(" CM");
    counter += 1;
    delay(500);
  }
  Serial.println("false");
}

void weight()
{
  // Load cell
  int counter = 0;

  Serial.println("L");
  if (LoadCell.update())    newDataReady = true;

  if (newDataReady)
  {
    if (LoadCell.getData() > 250)
    {
      for (int i = 0; i < 10; i++)
      {
        float value = LoadCell.getData();
        filtered_value = kalman.update_state(value);
        newDataReady = false;
        berat = filtered_value;
        delay(50);
      }
    }
  }
  Serial.println(filtered_value);
}
