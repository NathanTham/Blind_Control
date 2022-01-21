int enable2 = 9;
int in3 = 10;
int in4 = 11;
void setup()
{
pinMode(enable2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
}
void loop()
{
analogWrite(enable2, 200); // Any value between 0 and 255
digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
delay(10000);
analogWrite(enable2, 200); // Any value between 0 and 255
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
delay(10000);
}
