#include <ESP32Servo.h>

#define SERVO_PIN_3 27
#define SERVO_PIN_2 26
#define SERVO_PIN_1 25

Servo tail_servo;
Servo body_servo;
Servo head_servo;

float global_amplitude = 1.0;
float tail_amplitude = global_amplitude * 0.35;
float body_amplitude = global_amplitude * 0.25;
float head_amplitude = global_amplitude * 0.15;

float body_phase_offset = 0; //0.9;
float head_phase_offset = 0; //1.2;

float turn_angle;

float frequency = 0.6; // Hz

bool c_start;
bool stop = true; // counter or time, using it like this because I want the ability to reset it

bool is_burst_coast = false;
bool burst_coast_rising = false;
float burst_time = 0.0;

float phase_accumulator = 0.0;
unsigned long last_micros = 0;

void setup() {
  Serial.begin(9600);

  tail_servo.attach(SERVO_PIN_1);
  body_servo.attach(SERVO_PIN_2);
  head_servo.attach(SERVO_PIN_3);

  tail_servo.write(90);
  body_servo.write(90);
  head_servo.write(90);

  last_micros = micros();

  Serial.println("Started");
}

void loop() {

  unsigned long current_micros = micros();
  float delta_time = (current_micros - last_micros) / 1000000.0;
  last_micros = current_micros;

  if (delta_time > 0.1) delta_time = 0.1;

  if (stop) {
    head_servo.write(90);
    body_servo.write(90);
    tail_servo.write(90);
    phase_accumulator = 0;
    turn_angle = 0;
  }
  else {
    phase_accumulator += 2.0 * PI * frequency * delta_time;
    if (phase_accumulator > 2.0 * PI) {

      phase_accumulator -= 2.0 * PI;
    } 

    if (head_phase_offset < 1.2) {
      head_phase_offset = head_phase_offset + 0.01;
    }
    if (body_phase_offset < 0.9) {
      body_phase_offset = body_phase_offset + 0.01;
    }
  
    float sine_head = head_amplitude * sin(phase_accumulator + head_phase_offset);
    int head_angle = (90 * sine_head);

    float sine_body = body_amplitude * sin(phase_accumulator + body_phase_offset);
    int body_angle = (90 * sine_body);

    float sine_tail = tail_amplitude * sin(phase_accumulator);
    int tail_angle = (90 * sine_tail);
    
    head_servo.write(90 - head_angle); //head
    body_servo.write(90 - body_angle + turn_angle);
    tail_servo.write(90 + tail_angle);

    if (is_burst_coast) {
      burst_time = burst_time + delta_time;

      if (global_amplitude > 0 && burst_coast_rising == false) {
        global_amplitude = global_amplitude - 0.04;
      }
      else {
        global_amplitude = global_amplitude + 0.04;
      }

      if (burst_time > 5) {
        burst_time = 0;
        burst_coast_rising = true;
      }
      
      if (global_amplitude >= 1.0) 
        burst_coast_rising = false;

      tail_amplitude = global_amplitude * 0.35;
      body_amplitude = global_amplitude * 0.25;
      head_amplitude = global_amplitude * 0.1;
    }

  }

  if (Serial.available() > 0) {
    int choice = Serial.read();
    switch (choice) {
      case 119: // w
        stop = false;
        frequency = frequency + 0.1;
        break;
      case 115: // s
        stop = false;
        frequency = frequency - 0.1;
        break;
      case 100: // d
        if (turn_angle > -20) {
          turn_angle = turn_angle - 5;
        }
        break;
      case 97: // a
        if (turn_angle < 20) turn_angle = turn_angle + 5;
        break;
      case 48: // 0
        head_phase_offset = 0;
        body_phase_offset = 0;
        stop = true;
        break;
      case 49: // 1
        is_burst_coast = false;
        tail_amplitude = 0.35;
        body_amplitude = 0.25;
        head_amplitude = 0.1;
        break;
      case 50: // 2
        is_burst_coast = true;
        break;
    }

    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");
    Serial.print("Current character code: ");
    Serial.println(choice);
    Serial.println();
  }
}


