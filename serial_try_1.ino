


#define  led 12
int data;
void setup() {
  // initialize the serial communication for UART
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
 if(Serial.available() > 0)
  {
    data = Serial.read();
    delay(10);
    Serial.println(data);

    if(data >= 5){
      digitalWrite(led, HIGH);
    }
    else if(data < 4){
      digitalWrite(led, LOW);
    }
    

  }
  

}

/*
 int receiveddata =0;

void setup()

{

  Serial.begin(9600);//serial data rate is set for 9600BPS
  
  pinMode(1,INPUT);//RXD pin is set for INPUT
  pinMode(0,OUTPUT);
  pinMode(7,OUTPUT);//PIN1,PIN7 are set for output

               

}

void loop()
{
  if (Serial.available() > 0) //if data received is available
  {
    receiveddata = Serial.read();//read serial data available
    
    if (receiveddata == 0)//compare the data received
    {
    PORTD^=(1<<7);//id data matches toggle the LED.
    }

  }
Serial.print(receiveddata);
}

 */
