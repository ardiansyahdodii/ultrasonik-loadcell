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

void setup()
{
  // load cell
  LoadCell.begin();
  unsigned long stabilizingtime = 10000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;
  LoadCell.setCalFactor(20.22);
  LoadCell.start(stabilizingtime, _tare);

  // ultrasonik
  // Sensor 1
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);

  // Sensor 2
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  // Sensor 3
  pinMode(trig3, OUTPUT);
  pinMode(echo3, INPUT);

  // Set kecepatan motor (semakin tinggi nilai, semakin lambat kecepatannya)
  myStepper1.setSpeed(100);
  myStepper2.setSpeed(100);
  myStepper3.setSpeed(100);
  myStepper4.setSpeed(100);

  // Set nilai variabel naik
  naik = true;
  naik2 = true;
  mengukur = true;
  langkah = 0;
  langkah2 = 0;
  langkahSamping = 0;
  langkahSamping2 = 0;
  noDetect = 0;
  noDetect2 = 0;

  Serial.begin(9600);
}

void loop()
{
  // Load cell
  if (LoadCell.update())
    newDataReady = true;
  if (newDataReady)
  {
    for (int i = 0; i < 10; i++)
    {
      float value = LoadCell.getData();
      float filtered_value = kalman.update_state(value);
      Serial.print(filtered_value);
      Serial.print(",");
      newDataReady = false;
      berat = filtered_value;
      delay(50);
    }
    if (filtered_value > 1000)
    {

      // Sensor 3
      digitalWrite(trig3, LOW);
      delayMicroseconds(2);
      digitalWrite(trig3, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig3, LOW);

      // Sensor 3
      long duration3, distance3;
      duration3 = pulseIn(echo3, HIGH);
      distance3 = (duration3 * 0.034) / 2;

      if (distance3 < 96 && mengukur == true)
      {
        // waktu mulai
        unsigned long startTime = millis();

        delay(3000);
        Serial.println("mengukur tinggi");
        ketinggian();
        delay(500);
        Serial.println("mengukur lebar");
        motor1();
        delay(500);
        Serial.println("mengukur panjang");
        //motor2();
        mengukur = false;

        // waktu berakhir
        unsigned long endTime = millis();
        unsigned long elapsedTime = endTime - startTime;

        Serial.print("Waktu pengukuran: ");
        Serial.print(elapsedTime / 1000);
        Serial.println(" detik");

        delay(1000);
      }
      else if (distance3 >= 96)
      {
        mengukur = true;
        naik = true;
        naik2 = true;
        langkah = 0;
        langkah2 = 0;
        noDetect = 0;
        noDetect2 = 0;
        delay(1000);
      }
      delay(2000);
    }
  }
}

void ketinggian()
{
  // Sensor 3
  int counterTinggi = 0;

  while (counterTinggi < 10)
  {
    digitalWrite(trig3, LOW);
    delayMicroseconds(2);
    digitalWrite(trig3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig3, LOW);

    // Sensor 3
    long duration3, distance3;
    duration3 = pulseIn(echo3, HIGH);
    distance3 = (duration3 * 0.034) / 2;

    long tinggi = 98 - distance3;

    Serial.print(" , ,");
    Serial.println(tinggi);
    // Serial.print(",");

    counterTinggi += 1;
    delay(500);
  }
}

void motor1()
{
  counter = true;

  while (counter)
  {

    button1.loop();
    button2.loop();

    // Sensor 1
    digitalWrite(trig1, LOW);
    delayMicroseconds(2);
    digitalWrite(trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1, LOW);

    // Sensor 1
    long duration1, distance1;
    duration1 = pulseIn(echo1, HIGH);
    distance1 = (duration1 * 0.034) / 2;

    long jarak1 = 50 - distance1;

    //    Serial.print(jarak1);
    //    Serial.print(",");

    if (button1.getState() == 1 && button2.getState() == 1 && noDetect < 3)
    {
      // Bergerak Naik (ada objek)
      if (jarak1 > 0 && naik)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper1.step(-stepPerRevolition);
        langkah += 1;
        noDetect = 0;
        counter = true;
      }
      // Ketika Bergerak Naik => gerak samping dan turun
      else if (jarak1 <= 0 && naik)
      {
<<<<<<< HEAD
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper2.step(stepPerRevolition1);
        langkahSamping += 1;
        noDetect += 1;
        counter = true;
        naik = false;
=======

>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9
      }
      // Bergerak Turun
      else if (jarak1 > 0 && !naik && langkah > 0)
      {
<<<<<<< HEAD
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper1.step(stepPerRevolition);
        langkah -= 1;
        noDetect = 0;
        counter = true;
        naik = false;
=======
>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9

      }
      // Mentok Bawah ada objek (Tidak mentok atas dan samping)
      else if (jarak1 > 0 && !naik && langkah == 0)
      {
<<<<<<< HEAD
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper2.step(stepPerRevolition1);
        langkahSamping += 1;
        noDetect += 1;
        naik = true;
=======

>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9
      }
      // Mentok Bawah tidak ada objek (Tidak mentok atas dan samping)
      else if (jarak1 <= 0 && !naik && langkah == 0)
      {
<<<<<<< HEAD
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper2.step(stepPerRevolition1);
        langkahSamping += 1;
        noDetect += 1;
        naik = true;
      }
      else if (jarak1 <= 0 && naik && langkah == 0)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak1);
        myStepper2.step(stepPerRevolition1);
        langkahSamping += 1;
        noDetect += 1;
        naik = false;
=======

>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9
      }
    }
    else if (button1.getState() == 0 && button2.getState() == 1) {
      // geser samping
      myStepper2.step(stepPerRevolition1);
      // variabel noDetect bertambah
      noDetect += 1;
    }
    else if (button1.getState() == 1 && button2.getState() == 0) {
      // geser samping stop
      // bisa gerak naik dan turun aja
      // variabel no detect bertambah
      noDetect += 1;
    }
    else if (button1.getState() == 0 && button2.getState() == 0)
    {
      noDetect = 3;
    }
    // stack
<<<<<<< HEAD
    else if (noDetetct >= 3) {
      counter = false;
      while (langkah > 0)
      {
        myStepper1.step(stepsPerRevolution);
        langkah -= 1;
      }
      while (langkahSamping > 0)
      {
        myStepper2.step(-stepsPerRevolution1);
        langkahSamping -= 1;
      }
=======
  }
}

void motor2()
{

  counter2 = true;

  while (counter2)
  {
    // Sensor 2
    digitalWrite(trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig2, LOW);

    // Sensor 2
    long duration2, distance2;
    duration2 = pulseIn(echo2, HIGH);
    distance2 = (duration2 * 0.034) / 2;

    long jarak2 = 98 - distance2;

    // membuat pengodisian berhenti saat sensor naik
    if (jarak2 > 0 && noDetect2 < 3)
    {
      if (naik2 == true && langkah2 < 21)
      {
        Serial.println(jarak2);
        myStepper3.step(-stepsPerRevolution);
        langkah2 += 1;
        noDetect2 = 0;
        counter2 = true;
      }
      if (naik2 == true && langkah2 >= 21)
      {
        Serial.println(jarak2);
        myStepper3.step(stepsPerRevolution);
        langkah2 -= 1;
        naik2 = false;
        counter2 = true;

        if (langkahSamping2 <= 5)
        {
          myStepper4.step(-stepsPerRevolution1);
          langkahSamping2 += 1;
          noDetect2 = 0;
        }
        else
        {
          noDetect2 += 1;
        }
      }
      else if (naik2 == false)
      {
        Serial.println(jarak2);
        myStepper3.step(stepsPerRevolution);
        langkah2 -= 1;
        // noDetect2 = 0;
        counter2 = true;

        if (langkah2 == 0)
        {
          if (langkahSamping2 < 5)
          {
            Serial.println(jarak2);
            myStepper4.step(-stepsPerRevolution1);
            langkah2 = 0;
            naik2 = true;
            noDetect2 = 0;
            counter2 = true;
            langkahSamping2 += 1;
          }
          else
          {
            langkah2 = 0;
            noDetect2 += 1;
          }
        }
      }
    }
    // ketika sensor sudah tidak mendeteksi objek terluar(berhenti di pojok objk)
    else if (jarak2 <= 0 && noDetect2 < 3)
    {
      Serial.println(jarak2);
      counter2 = true;
      noDetect2 += 1;

      if (langkahSamping2 < 5)
      {
        myStepper4.step(-stepsPerRevolution1);
        langkahSamping2 += 1;

        if (langkah2 > 0)
        {
          myStepper3.step(stepsPerRevolution);
          langkah2 -= 1;
          naik2 = false;
        }
        if (langkah2 == 0)
        {
          myStepper3.step(-stepsPerRevolution);
          langkah2 += 1;
          naik2 = true;
        }
      }
>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9
    }
    else
    {
      // Reset atau posisi sensor berhenti
      counter2 = false;
      while (langkah2 > 0)
      {
        myStepper3.step(stepsPerRevolution);
        langkah2 -= 1;
      }
      while (langkahSamping2 > 0)
      {
        myStepper4.step(stepsPerRevolution1);
        langkahSamping2 -= 1;
      }
    }
    // delay(200);
  }
}
<<<<<<< HEAD
void motor2()
{

  counter2 = true;

  while (counter2)
  {
    // Sensor 2
    digitalWrite(trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig2, LOW);

    // Sensor 2
    long duration2, distance2;
    duration2 = pulseIn(echo2, HIGH);
    distance2 = (duration2 * 0.034) / 2;

    long jarak2 = 98 - distance2;

    // membuat pengodisian berhenti saat sensor naik
    if (button3.getState() == 1 && button4.getState() == 1 && noDetect < 3)
    {
      // Bergerak Naik (ada objek)
      if (jarak2 > 0 && naik2)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper3.step(-stepPerRevolition);
        langkah2 += 1;
        noDetect2 = 0;
        counter2 = true;
      }
      // Ketika Bergerak Naik => gerak samping dan turun
      else if (jarak2 <= 0 && naik2)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper4.step(-stepPerRevolition);
        langkahSamping2 += 1;
        noDetect2 += 1;
        counter2 = true;
        naik2 = false;
      }
      // Bergerak Turun
      else if (jarak2 > 0 && !naik2 && langkah2 > 0)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper3.step(stepPerRevolition);
        langkah2 -= 1;
        noDetect2 = 0;
        counter2 = true;
        naik2 = false;

      }
      // Mentok Bawah ada objek (Tidak mentok atas dan samping)
      else if (jarak2 > 0 && !naik2 && langkah2 == 0)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper4.step(-stepPerRevolition);
        langkahSamping2 += 1;
        noDetect2 += 1;
        naik2 = true;
      }
      // Mentok Bawah tidak ada objek (Tidak mentok atas dan samping)
      else if (jarak2 <= 0 && !naik2 && langkah2 == 0)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper4.step(-stepPerRevolition);
        langkahSamping2 += 1;
        noDetect2 += 1;
        naik2 = true;
      }
      else if (jarak2 <= 0 && naik2 && langkah2 == 0)
      {
        Serial.print("Lebar : ");
        Serial.println(jarak2);
        myStepper4.step(-stepPerRevolition);
        langkahSamping2 += 1;
        noDetect2 += 1;
        naik2 = false;
      }
    }
    else if (button3.getState() == 0 && button4.getState() == 1) {
      // geser samping
      myStepper4.step(stepPerRevolition);
      // variabel noDetect bertambah
      noDetect2 += 1;
    }
    else if (button3.getState() == 1 && button4.getState() == 0) {
      // geser samping stop
      // bisa gerak naik dan turun aja
      // variabel no detect bertambah
      noDetect2 += 1;
    }
    else if (button3.getState() == 0 && button4.getState() == 0)
    {
      noDetect2 = 3;
    }
    // stack
    else if (noDetetct2 >= 3) {
      counter2 = false;
      while (langkah2 > 0)
      {
        myStepper3.step(stepsPerRevolution);
        langkah2 -= 1;
      }
      while (langkahSamping2 > 0)
      {
        myStepper4.step(stepsPerRevolution1);
        langkahSamping2 -= 1;
      }
    }
  }
}
=======
>>>>>>> b4ae243c04d6a9c0afc30a3cc0e830c70e9d6be9
