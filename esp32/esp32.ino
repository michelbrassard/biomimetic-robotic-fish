#include <ESP32Servo.h>

#define SERVO_PIN_3 27
#define SERVO_PIN_2 26
#define SERVO_PIN_1 25

Servo servo1;
Servo servo2;
Servo servo3;

// Parameters
float amplitude1;
float amplitude2;
float amplitude3;

float phase_offset1;
float phase_offset2;
float phase_offset3;

float head_turn_angle;
float tail_turn_angle;

float frequency = 5.0;

bool ondulatory_swimming;
bool burst_coast;
bool turning;
bool c_start;

bool zero = true;

void setup() {
  Serial.begin(9600);

  servo1.attach(SERVO_PIN_1);
  servo2.attach(SERVO_PIN_2);
  servo3.attach(SERVO_PIN_3);
  // servo4.attach(SERVO_PIN_4);
  // servo5.attach(SERVO_PIN_5);

  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  // servo4.write(90);
  // servo5.write(90);

  delay(2000);

  Serial.println("Started");
}

void loop() {

  if (zero) {
    servo3.write(90);
    servo2.write(90);
    servo1.write(90);
  }
  else {
    float t = millis() / 1000.0;
  
    float sine3 = 0.1 * sin(t * frequency + 1.2);
    int angle3 = (90 * sine3);

    float sine2 = 0.25 * sin(t * frequency + 0.9);
    int angle2 = (90 * sine2);

    float sine1 = 0.35 * sin(t * frequency);
    int angle1 = (90 * sine1);
    
    servo3.write(90 + angle3); //head
    servo2.write(90 - angle2);
    servo1.write(90 + angle1);
  }

  if (Serial.available() > 0) {

    // Multiple characters can be added, writing "wwwww" will give +5, repeating the code for each character
    // Negativan f obrne smjer kretanja vala
    int choice = Serial.read();
    switch (choice) {
      // Character "w"
      case 119:
        zero = false;
        frequency = frequency + 0.50;
        break;

      // Character "s"
      case 115:
        zero = false;
        frequency = frequency - 0.50;
        break;
      
      case 100:
        
        break;
      
      case 97:
        
        break;
      
      // zero
      case 48:
        zero = true;
        break;
    }

    Serial.print("Frequency: ");
    Serial.println(frequency);
    Serial.println(choice);
  }

}


