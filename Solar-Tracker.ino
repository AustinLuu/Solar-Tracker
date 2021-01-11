#include <PID_v1.h>
#include <Servo.h>
Servo Vservo;
int vertical = 120;
double Kp=2, Ki=0.2, Kd=0.2;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);
#include <Stepper.h>
int stepsPerRevolution = 32;
Stepper Hstepper(stepsPerRevolution, 8, 10, 9, 11);

int deadband = 20;
int right = A1;
int left = A0;
int r = 0;
int l = 0;
int maxintensity = 0;
int intensity = 0;
int rotation = 0;
int deadzone = 0;
boolean search = false;

void setup()
{
  Serial.begin(9600);
  Vservo.attach(6);
  Vservo.write(vertical);
  myPID.SetMode(AUTOMATIC); 
  Setpoint = (analogRead(left)+analogRead(right))/2;
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetOutputLimits(30,180);
}

void loop()
{
  delay(50);
  r = analogRead(right);
  l = analogRead(left);
  Input = (r+l)/2;
  Serial.println(r);
  Serial.println(l);
  myPID.Compute();
  Vservo.write(round(180.0*Output/9.0));
  if (r > l + deadband)
  {
    Hstepper.setSpeed(200);
    Hstepper.step(10);
    Serial.print("Searching: ");
    deadzone = 0;
    search = true;
  }
  if (l > r + deadband)
  {
    Hstepper.setSpeed(200);
    Hstepper.step(-10);
    Serial.print("Searching: ");
    deadzone = 0;
    search = true;
  }

  if (search == true && r < l + deadband && l < r + deadband)
  {
    deadzone++;
    Serial.print("Deadzone: ");
    Serial.println(deadzone);
    if (deadzone > 20)
    {
      Serial.println("Found");
      vsweep();
      search = false;
      deadzone = 0;
    }
  }
}

int vsweep()
{
  maxintensity = 0;
  for (vertical = 160; vertical >= 90; vertical = vertical - 1)
  {
    Vservo.write(vertical);
    intensity = (analogRead(right) + analogRead(left)) / 2;
    if (maxintensity < intensity)
    {
      maxintensity = intensity;
      rotation = vertical;
    }
    delay(50);
  }
  Vservo.write(rotation);
  delay(200);
}
