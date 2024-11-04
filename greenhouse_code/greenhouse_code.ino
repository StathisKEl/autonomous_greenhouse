#include <dht.h> // Βιοβλιοθήκη αισθητήρα υγρασίας-θερμοκρασίας
#include <Servo.h> // Βιβλιοθήκη σερβοκινητήρα
#include <LiquidCrystal.h> // Βιβλιοθήκη LCD Οθόνης

#define soilWet 400 // μέγιστη τιμή που θεωρείται το έδαφος υγρό
#define soilDry 600 // ελάχιστη τιμή που θεωρείται το έδαφος ξηρό
#define Tmin 17 // μικρότερη "καλή" θερμοκρασία
#define Tmax 21 // μεγαλύτερη "καλή θερμοκρασία
#define HDmin 55 // μικρότερη "καλή" υγρασία
#define HDmax 70 // μεγαλύτερη "καλή" υγρασία
#define h_water 13 // όριο ύψους νερού
#define light 30 // όριο φωτισμού
#define flamelim 800 // όριο κυνδίνου φλόγας
#define windowcl 0 // κλειστό παράθυρο
#define windowmid 30 // μέτρια ανοιχτό παράθυρο
#define windowopen 90 // ανοιχτό παράθυρο
// ονοματοδώτηση των pins
#define soilWetPin A0
#define DHT11Pin 2
#define flamePin A1
#define p_buzzer 5
#define photoresistor A2
#define BUTTON_PIN 19
#define koumpi1 18
#define koumpi2 17
#define servoPin 7
#define antlia 6
#define antistash 10
#define lampa 9
#define anemisthras 8
#define trigPin 3
#define echoPin 4
dht DHT;
// ορισμός pins της οθόνης
const int rs = 16, en = 15, d4 = 14, d5 = 13, d6 = 12, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Servo servo;
int leitourgia = 0;

void setup()
{
  // ορισμός εισόδων-εξόδων
  servo.attach(servoPin);
  servo.write(windowcl);
  pinMode(antlia, OUTPUT);
  pinMode(p_buzzer, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), allagh, RISING);  
  pinMode(antistash, OUTPUT);
  pinMode(lampa, OUTPUT);
  pinMode(anemisthras, OUTPUT);
  pinMode(koumpi1, INPUT);
  pinMode(koumpi2, INPUT);
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);
  lcd.begin(16, 2);
  digitalWrite(antlia, HIGH);
  digitalWrite(anemisthras, HIGH);
  digitalWrite(lampa, HIGH);
  digitalWrite(antistash, HIGH);
  digitalWrite(p_buzzer, LOW);
  lcd.clear();
  lcd.print("Start");
}

void loop()
{
  if(leitourgia == 1)
  { // χειροκίνητη λειτουργία
    lcd.clear();
    lcd.print("User Control");
    delay(1000);
    lcd.clear();
    lcd.print("Open lamp?"); // ορισμός λειτουργίας λάμπας
    delay(1000);
    while(leitourgia == 1)
    {
      if(digitalRead(koumpi1) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(lampa, HIGH);
        lcd.print("Closed");
        break;
      }
      else if(digitalRead(koumpi2) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(lampa, LOW);
        lcd.print("Open");
        break;
      }
    }
    delay(1000);
    lcd.clear();
    lcd.print("Open fan?"); // ορισμός λειτουργίας ανεμιστήρα
    delay(1000);
    while(leitourgia == 1)
    {
      if(digitalRead(koumpi1) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(anemisthras, HIGH);  
        lcd.print("Closed");
        break;
      }
      else if(digitalRead(koumpi2) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(anemisthras, LOW);
        lcd.print("Open");
        break;
      }
    }
    delay(1000);
    lcd.clear();
    lcd.print("Open window?"); // ορισμός λειτουργίας παραθύρου
    delay(1000);
    while(leitourgia == 1)
    {
      if(digitalRead(koumpi1) == 1)
      {
        lcd.setCursor(0, 1);
        servo.write(windowcl);
        lcd.print("Closed");
        break;
      }
      else if(digitalRead(koumpi2) == 1)
      {
        lcd.setCursor(0, 1);
        servo.write(windowopen);
        lcd.print("Open");
        break;
      }
    }
    delay(1000);
    lcd.clear();
    lcd.print("Open resistor?"); // ορισμός λειτουργίας αντίστασης
    delay(1000);
    while(leitourgia == 1)
    {
      if(digitalRead(koumpi1) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(antistash, HIGH);
        lcd.print("Closed");
        break;
      }
      else if(digitalRead(koumpi2) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(antistash, LOW);
        lcd.print("Open");
        break;
      }
    }
    delay(1000);
    lcd.clear();
    lcd.print("Open watering?"); // ορισμός λειτουργίας αντλίας
    delay(1000);
    while(leitourgia == 1)
    {
      if(digitalRead(koumpi1) == 1)
      {
        lcd.setCursor(0, 1);
        digitalWrite(antlia, HIGH);
        lcd.print("Closed");
        break;
      }
      else if(digitalRead(koumpi2) == 1)
      {
        lcd.setCursor(0, 1);
        lcd.print("Open");
        potisma();
        break;
      }
    }
    delay(1000);
    while(leitourgia == 1)
    {
      LCDFunction(); // εμφάνιση τιμών στην οθόνη
    }

  }
  else if(leitourgia == 0)
  { // αυτόματη λειτουργία
    lcd.clear();
    lcd.print("Automatic");
    lcd.setCursor(0, 1);
    lcd.print("Control");
   
    while(leitourgia == 0)
    {
      // ανάγνωση αισθητήρων και αναγνώριση καταστάσεων
      int chk = DHT.read11(DHT11Pin);
      float thermokrasia = DHT.temperature;
      float ygrasia = DHT.humidity;
      float ygrasia_edafous = analogRead(soilWetPin);
      int aktinovolia = analogRead(photoresistor);
      int timh_thermokrasias;
      int timh_ygrasias;
      int timh_ygrasias_edafous;
      if(thermokrasia >= Tmax)
      {
        timh_thermokrasias = 1;
      }
      else if(thermokrasia < Tmax && thermokrasia >= Tmin)
      {
        timh_thermokrasias = 2;
      }
      else
      {
        timh_thermokrasias = 3;
      }
      if(ygrasia >= HDmax)
      {
        timh_ygrasias = 1;
      }
      else if(ygrasia < HDmax && ygrasia >= HDmin)
      {
        timh_ygrasias = 2;
      }
      else
      {
        timh_ygrasias = 3;
      }
      if(ygrasia_edafous >= soilDry)
      {
        timh_ygrasias_edafous = 1;
      }
      else if(ygrasia_edafous < soilDry && ygrasia_edafous >= soilWet)
      {
        timh_ygrasias_edafous = 2;
      }
      else
      {
        timh_ygrasias_edafous = 3;
      }
      if(aktinovolia > light)
      {
        digitalWrite(lampa, HIGH);
      }
      else
      {
        digitalWrite(lampa, LOW);
      }
      // έλεγχος βάση συνθηκών
      if((timh_thermokrasias == 1) && (timh_ygrasias == 1) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 1) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 1) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 1) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 1) && (timh_ygrasias_edafous = 1))
      {
      digitalWrite(anemisthras, HIGH);
      potisma();
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 1) && (timh_ygrasias_edafous = 2))
      {
      digitalWrite(anemisthras, HIGH);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 1) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowopen); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, HIGH);
      potisma();
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, HIGH);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, HIGH);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, HIGH);
      potisma();
      servo.write(windowcl); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, HIGH);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 2) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, HIGH);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 1) && (timh_ygrasias_edafous = 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowmid); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 1) && (timh_ygrasias_edafous = 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 1) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowmid); 
      digitalWrite(antistash, HIGH);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 2) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 1))
      {
      digitalWrite(anemisthras, LOW);
      potisma();
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 2))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      else if((timh_thermokrasias == 3) && (timh_ygrasias == 3) && (timh_ygrasias_edafous == 3))
      {
      digitalWrite(anemisthras, LOW);
      digitalWrite(antlia, HIGH);
      servo.write(windowcl); 
      digitalWrite(antistash, LOW);
      }
      delay(1000);
      LCDFunction();
    }
  }
}

void allagh() 
{ // αλλαγή κατάστασης λειτουργίας
  if (leitourgia == 0)
  {
    leitourgia = 1 ; 
  }
  else 
  {
    leitourgia = 0 ;
  }
  delay(500);
}

void LCDFunction()
{ // εμφάνιση κλιματικών συνθηκών στην LCD
      lcd.clear();
      int chk = DHT.read11(DHT11Pin);
      lcd.print("Temperature");
      lcd.setCursor(0, 1);
      lcd.print((float)DHT.temperature);
      lcd.print(" C");
      ErrorFunction();
      delay(2000);
      lcd.clear();
      lcd.print("Humidity");
      lcd.setCursor(0, 1);
      lcd.print((float)DHT.humidity);
      lcd.print(" %");
      ErrorFunction();
      delay(2000);
      lcd.clear();
      int SOILM = analogRead(soilWetPin);
      lcd.print("Soil Moisture");
      lcd.setCursor(0, 1);
      lcd.print(SOILM);
      ErrorFunction();
      delay(2000);
}

void ErrorFunction()
{ // συνάρτηση ανίχνευσης σφαλμάτων (ανίχνευση φλόγας ή μικρής στάθμης)
  int flameSensorReading = analogRead(flamePin);  
  digitalWrite(trigPin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trigPin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trigPin, LOW);  
  long duration = pulseIn(echoPin, HIGH);
  int distance = (duration*0.0343)/2; 
  if((flameSensorReading < flamelim) || (distance > h_water))
  {
    digitalWrite(p_buzzer, HIGH);
  }
  else
  {
    digitalWrite(p_buzzer, LOW);
  }
  delay(500);  
}

void potisma()
{ // πότισμα 3 δευτερολέπτων
  digitalWrite(antlia, LOW);
  delay(3000);
  digitalWrite(antlia, HIGH);
}
