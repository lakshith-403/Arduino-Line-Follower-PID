#include "Arduino.h"
#include <Servo.h>

 int pivot = 400;
 int jPivot = 150;
int pivots[8];
int yCount=0;


const int trig = 11;
const int echo = 10;

const int setPoint = 450;

const double Kp =1.1;
const double Kd =0.8;
const double Ki =0.001;

// const double Kp =1.1;
// const double Kd =0.8;
// const double Ki =0.001;
int totalErr = 0;
int prevError = 0;

#define S0 37
#define S1 35
#define S2 43
#define S3 39
#define sensorOut 41
int r,b,g;
int frequency = 0;
int dd = 300;

const int leftIR = 1;
const int rightIR = 0;

const int maxSpeed = 250;

const int btn = 5;

bool loopFlag = true;

int left = 0;
int right = 0;
int c = 0;

String path = String("");
int memCount = -1;

Servo xServo;
Servo yServo;

int col = 0;
bool box = false;

void setup(){
  Serial.begin(9600);
 
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(22,OUTPUT);
  pinMode(28,OUTPUT);

  pinMode(btn,INPUT_PULLUP);

  pinMode(23,INPUT);
  pinMode(25,INPUT);

  pinMode(leftIR,INPUT);
  pinMode(rightIR,INPUT);

  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  pinMode(45,OUTPUT);
  pinMode(47,OUTPUT);
  pinMode(49,OUTPUT);
  pinMode(51,OUTPUT);

  xServo.attach(9);
  yServo.attach(8);
  calibrate();
  xServo.write(5);
  yServo.write(100);
  stop(1);
}

void loop(){
  lineFollow();
  // int dis = getDistance();
  // while(dis>5){
  //   dis = getDistance();
  //   Serial.println(dis);
  // }
  
  
  // loose();
  
  // delay(1000);
  // armDown();
 
  // delay(1000);
  // grab();
  
  // delay(1000);
  // armUp();
  
  // delay(1000);
  
  // stop(100);



}

int getColor()
{
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  r = pulseIn(sensorOut, LOW);
  delay(100);
  
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);

  g = pulseIn(sensorOut, LOW);
  delay(100);
  
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);

  b = pulseIn(sensorOut, LOW);
  delay(100);

  if(r<g && r<b )
    return 0;
  else if(b<g && b<r)
    return 2;
  else if(g<r && g<b)
    return 1;

    
}

void color(int i){
  if(i==2){
    digitalWrite(45,HIGH);
    digitalWrite(47,HIGH);
    digitalWrite(49,HIGH);
    digitalWrite(51,LOW);
  }else if(i==1){
    digitalWrite(45,HIGH);
    digitalWrite(47,HIGH);
    digitalWrite(49,LOW);
    digitalWrite(51,HIGH);
  }else if(i==0){
    digitalWrite(45,LOW);
    digitalWrite(47,HIGH);
    digitalWrite(49,HIGH);
    digitalWrite(51,HIGH);
  }else{
    digitalWrite(45,HIGH);
    digitalWrite(47,HIGH);
    digitalWrite(49,HIGH);
    digitalWrite(51,HIGH);
  }
}



void armDown(){
  for(int i=160;i>=35;i--){
    yServo.write(i);
    delay(1);
  }

}

void armUp(){
  xServo.write(120);
  for(int i=30;i<=150;i++){
    yServo.write(i);
    delay(5);
  }
  //yServo.write(160);
}

void grab(){
  for(int i=90;i<=120;i++){
    xServo.write(i);
    delay(1);
  }
}

void loose(){
  for(int i=130;i>=90;i--){
  xServo.write(i);
  delay(1);
  }
}

int getDistance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  double duration;
  int distance;

  duration =  pulseIn(echo, HIGH);
  
  distance = duration * 0.034 / 2;
 
  return distance;
}

void calibrate()
{   
    int r1 = analogRead(A0);
    forward(maxSpeed,200);
    int r2 = analogRead(A0);
    jPivot = (r1+r2)/2;
    jPivot = 150;
    int a_readings[8];
    a_readings[0] = analogRead(A10);//13
    a_readings[1] = analogRead(A11);//12
    a_readings[2] = analogRead(A15);//9
    a_readings[3] = analogRead(A14);//8
    a_readings[4] = analogRead(A8);//14
    a_readings[5] = analogRead(A9);//15
    a_readings[6] = analogRead(A12);//11
    a_readings[7] = analogRead(A13);//10

    int min = 10000;
    int max = -1;
    for(int i=0;i<8;i++){
      min = min<a_readings[i] ? min : a_readings[i];
      max = max>a_readings[i] ? max : a_readings[i];
    }
    pivot = (min+max)/2;
}

int leftRead(){
  return analogRead(A2)>jPivot ? 1 : 0;
}

int rightRead(){
  return analogRead(A0)>jPivot ? 1 : 0;
}


void memTest(){
  for(int i=0;i<path.length();i++){
    char temp = path.charAt(i);
    if(temp=='B')forward(maxSpeed,50);
    else if(temp=='F')forward(maxSpeed,200);
    else if(temp=='L')backward(maxSpeed,50);
    else backward(maxSpeed,200);
    stop(1000);
  }
}

void pathShort(){    
    bool loopFlag = true;
    while(loopFlag){
      loopFlag = false;
      for(int i=1;i<path.length()-1;i++){
       
        if(path.charAt(i)=='B'){
        
          loopFlag = true;
          int sum = 0;
          if(path.charAt(i-1)=='L') sum += 5;
          else if(path.charAt(i-1)=='R') sum += 10;
          else sum += 20;

          if(path.charAt(i+1)=='L') sum += 5;
          else if(path.charAt(i+1)=='R') sum += 10;
          else sum += 20;
            
          String temp = path.substring(i-1,i+2);
         
          if(sum==15 || sum==40)
             path.replace(temp,"B");
          else if(sum==20 || sum==10)
             path.replace(temp,"F");
          else if(sum==25)
             path.replace(temp,"R");
          else //(sum==30)
             path.replace(temp,"L");
            break;
        }
        
      }
    }
    
}

void lineFollow(){

  if(getDistance()<=12 && !box){
    box = true;
    stop(100);
    loose();
    stop(100);
    armDown();
    stop(500);
    for(int i=0;i<300;i++){
      int ee = getErr();
      pid(ee);
    }
    
    stop(500);
    grab();
    delay(500);
    armUp();
    delay(500);
    xServo.write(90);
    delay(100);
    xServo.write(120);
    delay(500);
    yServo.write(160);
    delay(300);
    col = getColor();
    color(col);
    while(true)lineFollow();

  }
	int err = getErr();
	if(left==1 || right==1){//junction
    
		stop(100);

    left = leftRead();
    right = rightRead();

		if(left==1 && right==1){//T
        color(0);
   
        forward(maxSpeed,150);
        getErr();
        stop(1000);


        if(c!=8 && c!= 0 && box){
          if(col==0){
            Right(maxSpeed,1);
          }else if(col==1){
            Left(maxSpeed,1);
          }else if(col==2){
            forward(maxSpeed,200);
          }
          while(true){
            int e = getErr();
            if(c>=4)
            {
              stop(1000);
              backward(maxSpeed,400);
              stop(500);
              armDown();
              delay(500);
              loose();
              delay(600);
              backward(maxSpeed,400);
              armUp();
              while(true)stop(100);
              
            }
            pid(e);
          }
        }
        //stop(100);

        if(memCount!=-1){
          //stop(1000);
          char temp = path.charAt(memCount);
         // if(memCount==0)backward(maxSpeed,200);
          if(temp == 'R')
            Right(maxSpeed,1);
          else if(temp == 'L')
            Left(maxSpeed,1);
          else if(temp == 'B')
            turn180(maxSpeed);
          else
            forward(maxSpeed,200);

          memCount++;
        }else
          Right(maxSpeed,1);
        
        path += "R";
        color(8);
		}else if(left==1 && right==0){//Left
			color(1);

      forward(maxSpeed,200);
			getErr();

			if(c!=0)//  -|
      {
          if(memCount!=-1){
            //stop(1000);
            char temp = path.charAt(memCount);
            if(temp == 'R')
              Right(maxSpeed,1);
            else if(temp == 'L')
              Left(maxSpeed,1);
            else if(temp == 'B')
              turn180(maxSpeed);
            else
              forward(maxSpeed,200);

            memCount++;
          }else
            forward(maxSpeed,200);
            
            path+="F";
        }else//L
        {
          Left(maxSpeed,1);
        }
			
		}else{//Right
    color(2);
    
      forward(maxSpeed,200);
			getErr();
      if(c!=0)//  -|
      {
          if(memCount!=-1){
            //stop(1000);
          char temp = path.charAt(memCount);
          if(temp == 'R')
            Right(maxSpeed,1);
          else if(temp == 'L')
            Left(maxSpeed,1);
          else if(temp == 'B')
            turn180(maxSpeed);
          else
            forward(maxSpeed,200);

          memCount++;
        }else
          
          Right(maxSpeed,200);
          
          path+="R";
      }
			else//R
      {
        Right(maxSpeed,1);
      }
		}
    color(88);
	}else if(c>=6 && err>=-50 && err<=50 && yCount==0){ //Y
  yCount++;
    dd = 200;
      color(0);
      stop(100);
      if(leftRead()==1 || rightRead()==1)
        return;
      forward(maxSpeed,100);
      
      Right(maxSpeed,1);
      stop(100);
      
      bool flag =  (leftRead()==1 || rightRead()==1);
      while(!flag){
        err = getErr();
        pid(err);
        
        flag = (leftRead()==1 || rightRead()==1);
        
      }
      forward(maxSpeed,100);
      
      Left(maxSpeed,1);
      
      flag =  (leftRead()==1 || rightRead()==1);
      while(!flag){
        err = getErr();
        pid(err);
        
        flag =  (leftRead()==1 || rightRead()==1);
      }
      
      forward(maxSpeed,100);
      
      Right(maxSpeed,1);
      color(8);
      dd=300;
	}else if(c==0)//LineEnd
  {
    stop(100);
    color(1);
    turn180(maxSpeed);
    path+="B";
    color(8);
  }
	else
		pid(err);
}   

void pid(int err){
    int p = err * Kp;
    totalErr += (err/100);
    int i = totalErr*Ki;
    int d = (err - prevError) * Kd;
    prevError = err;
    int correction = p+i+d;
    if(correction>0){
      leftF(maxSpeed);
      rightF(maxSpeed-correction);
    }else{
      correction = correction * -1;
      rightF(maxSpeed);
      leftF(maxSpeed-correction);
    }
   
}

int getErr(){
    int a_readings[8];
    a_readings[0] = analogRead(A10);//13
    a_readings[1] = analogRead(A11);//12
    a_readings[2] = analogRead(A15);//9
    a_readings[3] = analogRead(A14);//8
    a_readings[4] = analogRead(A8);//14
    a_readings[5] = analogRead(A9);//15
    a_readings[6] = analogRead(A12);//11
    a_readings[7] = analogRead(A13);//10

    bool d_readings[8];
    int sum = 0;
    int count = 0;
    
    for(int i=0;i<8;i++){
        if(a_readings[i]<pivot){
            d_readings[i] = true;
           
            sum += i+1;
            count++;
            continue;
        }
        d_readings[i] = false;
        
    }
  
	c = count;
	left = leftRead();
	right = rightRead();
  return setPoint - ((sum*100)/count);
}

void leftF(int spd){
  if(spd<0)spd = 0;
  digitalWrite(24,HIGH);
  digitalWrite(22,LOW);
  analogWrite(7,spd);
}

void rightF(int spd){
  if(spd<0)spd = 0;
  digitalWrite(28,HIGH);
  digitalWrite(26,LOW);
  analogWrite(6,spd);
}

void leftB(int spd){
  if(spd<0)spd = 0;
  digitalWrite(24,LOW);
  digitalWrite(22,HIGH);
  analogWrite(7,spd);
}

void rightB(int spd){
  if(spd<0)spd = 0;
  digitalWrite(28,LOW);
  digitalWrite(26,HIGH);
  analogWrite(6,spd);
}

void forward(int spd , int dlay){
	digitalWrite(24,HIGH);
	digitalWrite(22,LOW);
	analogWrite(7,spd);
	digitalWrite(28,HIGH);
  digitalWrite(26,LOW);
  analogWrite(6,spd);
	delay(dlay);
}

void backward(int spd , int dlay){
  
	digitalWrite(24,LOW);
	digitalWrite(22,HIGH);
	analogWrite(7,spd);
	digitalWrite(28,LOW);
  digitalWrite(26,HIGH);
  analogWrite(6,spd);
	delay(dlay);
}

void stop(int dlay){
	digitalWrite(24,HIGH);
	digitalWrite(22,HIGH);
	analogWrite(7,maxSpeed);
	digitalWrite(28,HIGH);
  digitalWrite(26,HIGH);
  analogWrite(6,maxSpeed);
	delay(dlay);
}

void Left(int spd , int state){
  digitalWrite(24,LOW);
  digitalWrite(22,HIGH);
  analogWrite(7,spd);
  digitalWrite(28,HIGH);
  digitalWrite(26,LOW);
  analogWrite(6,spd);
  delay(dd);

  bool flag = (analogRead(A8)<pivot || analogRead(A14)<pivot) && leftRead()==0;
  while(!flag){
      analogWrite(7,spd);
      analogWrite(6,spd);
      flag = (analogRead(A8)<pivot || analogRead(A14)<pivot) && leftRead()==0;
  }
}

void Right(int spd , int state){
    digitalWrite(24,HIGH);
    digitalWrite(22,LOW);
    analogWrite(7,spd);
    digitalWrite(28,LOW);
    digitalWrite(26,HIGH);
    analogWrite(6,spd);
    delay(dd);
  
  bool flag = (analogRead(A8)<pivot || analogRead(A14)<pivot) && rightRead()==0;
  while(!flag){
    analogWrite(7,spd);   
    analogWrite(6,spd);
    
    flag = (analogRead(A8)<pivot || analogRead(A14)<pivot) && rightRead()==0;
  }
}

void turn180(int spd ){
  stop(100);
  forward(255,200);

  digitalWrite(24,HIGH);
  digitalWrite(22,LOW);
    digitalWrite(28,LOW);
  digitalWrite(26,HIGH);
  analogWrite(7,spd);

  analogWrite(6,spd);
  delay(400);

  bool flag = analogRead(A8)<pivot || analogRead(A14)<pivot;
  while(!flag){
    analogWrite(7,spd);
    analogWrite(6,spd);
    flag = (analogRead(A8)<pivot || analogRead(A14)<pivot);
  }
}
