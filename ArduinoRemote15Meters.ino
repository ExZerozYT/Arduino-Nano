#include <Servo.h>

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
  
  // ปิด Serial ไปก่อนเพื่อดูว่าหายค้างไหม (หรือใช้ Baud rate สูงสุด)
  Serial.begin(115200); 
}

void loop() {
  // --- อ่านค่า Sensor แบบเน้นชัวร์ ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // ปรับกลับเป็น 10 เพื่อให้คลื่นเสถียรขึ้น
  digitalWrite(trigPin, LOW);
  
  // ใช้ Timeout 15000 (ประมาณ 2.5 เมตร) เพื่อไม่ให้บอร์ดรีบตัดจบจนค้าง
  long duration = pulseIn(echoPin, HIGH, 15000); 
  
  // ระยะ 15cm (duration 870)
  if (duration > 0 && duration <= 870) { 
    confirmCount++; 
  } else {
    confirmCount = 0; 
  }

  // --- ส่วนตัดสินใจ (เพิ่มจังหวะพักเล็กน้อยป้องกัน CPU ค้าง) ---
  if (confirmCount >= 3) { // กลับมาใช้ 3 เพื่อความชัวร์ไม่สั่น
    if (lastState == 0) {
      myServo.write(90);        
      lastState = 1; 
      delay(250); // ให้เวลาหมุน
    }
  } 
  else if (confirmCount == 0) {
    if (lastState == 1) {
      myServo.write(0);         
      lastState = 0; 
      delay(250); // ให้เวลาหมุนกลับ
    }
  }

  delay(10); // *** สำคัญมาก: ใส่ 10ms เพื่อให้บอร์ดได้เคลียร์ค่าต่างๆ ในระบบ ***
}
