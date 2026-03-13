# 🚀 Arduino Remote Servo Control via RS-485 (15 Meters)

โปรเจกต์ควบคุม Servo SG90 ระยะไกล 15 เมตร โดยใช้เซนเซอร์ Ultrasonic (HC-SR04) และสื่อสารผ่านโมดูล RS-485 (MAX485) เพื่อความเสถียรของสัญญาณในระยะไกล

## 🛠 อุปกรณ์ที่ใช้
1. **Arduino Nano / Uno** (ฝั่งตัวส่ง)
2. **Ultrasonic Sensor HC-SR04**
3. **MAX485 Module** (2 ตัว - ตัวส่งและตัวรับ)
4. **Servo SG90** (ฝั่งตัวรับ)
5. **Adapter 5V 2A** (แนะนำให้แยกแหล่งจ่ายไฟฝั่งปลายทาง)
6. **สาย LAN (CAT5/CAT6)** ยาว 15 เมตร

## 🔌 การต่อสาย (Wiring Diagram)

### จุดที่ 1: ฝั่งต้นทาง (Arduino + Ultrasonic + MAX485)
| อุปกรณ์ | ขาอุปกรณ์ | ต่อเข้ากับ |
|---|---|---|
| **Ultrasonic** | VCC | 5V |
| | GND | GND |
| | Trig | D9 |
| | Echo | D10 |
| **MAX485 (TX)** | VCC | 5V |
| | GND | GND |
| | DE / RE | 5V (บังคับส่งสัญญาณตลอดเวลา) |
| | DI | D11 (Servo Signal) |

### จุดที่ 2: การเชื่อมต่อระหว่างโมดูล (สาย LAN 15 เมตร)
| MAX485 ตัวส่ง (A/B) | สาย LAN | MAX485 ตัวรับ (A/B) |
|---|---|---|
| **ช่องน็อต A** | สีส้ม | **ช่องน็อต A** |
| **ช่องน็อต B** | ขาวส้ม | **ช่องน็อต B** |
| **GND** | สีน้ำตาล | **GND** (เชื่อมกราวด์ระหว่างสองฝั่ง) |

### จุดที่ 3: ฝั่งปลายทาง (MAX485 + Servo)
| อุปกรณ์ | ขาอุปกรณ์ | ต่อเข้ากับ |
|---|---|---|
| **MAX485 (RX)** | VCC | Adapter 5V (+) |
| | GND | Adapter 5V (-) |
| | DE / RE | GND (มัดรวมกันเพื่อรับสัญญาณตลอดเวลา) |
| | RO | สายสัญญาณ Servo (สีส้ม) |
| **Servo SG90** | VCC (แดง) | Adapter 5V (+) |
| | GND (น้ำตาล) | Adapter 5V (-) |

---

## 💻 Code (Fast & Stable Version)

โค้ดนี้ถูกปรับแต่งให้มีความเร็วในการตอบสนองสูง (Turbo Mode) แต่ยังคงความเสถียรเพื่อป้องกันอาการ Servo สั่นหรือค้าง

```cpp
#include <Servo.h>

// กำหนดพิน
const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 11; 

Servo myServo;
int lastState = 0; 
int confirmCount = 0; 

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin);
  myServo.write(0); 
  
  Serial.begin(115200);
  Serial.println("System Ready...");
}

void loop() {
  // สั่งยิง Ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  
  // ตรวจจับระยะทาง (Timeout 15ms ~ 2.5 เมตร)
  long duration = pulseIn(echoPin, HIGH, 15000); 
  
  // เงื่อนไขระยะ 15 cm (Duration ประมาณ 870us)
  if (duration > 0 && duration <= 870) { 
    confirmCount++; 
  } else {
    confirmCount = 0; 
  }

  // ส่วนตัดสินใจการทำงาน
  if (confirmCount >= 3) { // ยืนยัน 3 รอบเพื่อกันสัญญาณกวน
    if (lastState == 0) {
      myServo.write(90);        
      lastState = 1; 
      Serial.println("Action: Open");
      delay(250); // รอให้ Servo หมุนเสร็จ
    }
  } 
  else if (confirmCount == 0) {
    if (lastState == 1) {
      myServo.write(0);         
      lastState = 0; 
      Serial.println("Action: Close");
      delay(250); // รอให้ Servo กลับตำแหน่งเดิม
    }
  }

  delay(10); // พักลูปเล็กน้อยเพื่อลดภาระ CPU
}
