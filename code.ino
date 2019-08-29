#include "Arduino.h"


const int pivot = 950;

const int setPoint = 450;

const double Kp =1.2;
const double Kd =0.5;
const double Ki =0.0;
int totalErr = 0;
int prevError = 0;

const int leftIR = 53;
const int rightIR = 52;

const int maxSpeed = 250;

int left = 0;
int right = 0;
int c = 0;

String path = String("");
int memCount = -1;

void setup(){
  Serial.begin(9600);
 
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(22,OUTPUT);
  pinMode(28,OUTPUT);

  pinMode(leftIR,INPUT);
  pinMode(rightIR,INPUT);
}

bool loopFlag = true;

void loop(){
  forward(maxSpeed,200);
  while(loopFlag)
      lineFollow();
 // pathShort();
  memCount=0;

  

  while(false)
    lineFollow();
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
	int err = getErr();
	if(left==1 || right==1){//junction
		stop(100);

    left = digitalRead(leftIR);
    right = digitalRead(rightIR);

		if(left==1 && right==1){//T
        forward(maxSpeed,150);
        getErr();

        if(c==8){
          //path end
          stop(5000);
          loopFlag = false;
          memTest();
          stop(3000);
           pathShort();
           memTest();
          pathShort();
          memCount=0;
          while(true)
          lineFollow();
        }

        stop(100);

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
		}else if(left==1 && right==0){//Left
			
      forward(maxSpeed,200);
			getErr();

			if(c!=0)//  -|
      {
          if(memCount!=-1){
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
      forward(maxSpeed,200);
			getErr();
      if(c!=0)//  -|
      {
          if(memCount!=-1){
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
	}else if(c>=6 && err==0){ //Y
      stop(100);
      if(digitalRead(leftIR)==1 || digitalRead(rightIR)==1)
        return;
      forward(maxSpeed,100);
      
      Right(maxSpeed,1);
      stop(100);
      
      bool flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      while(!flag){
        err = getErr();
        pid(err);
        
        flag = (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
        
      }
      forward(maxSpeed,100);
      
      Left(maxSpeed,1);
      
      flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      while(!flag){
        err = getErr();
        pid(err);
        
        flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      }
      
      forward(maxSpeed,100);
      
      Right(maxSpeed,1);
	}else if(c==0)//LineEnd
  {
    turn180(maxSpeed);
    path+="B";
  }
	else
		pid(err);
}   

void pid(int err){
    int p = err * Kp;
    totalErr += (err/100);
    int i = 0;
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
    a_readings[0] = analogRead(A8);
    a_readings[1] = analogRead(A9);
    a_readings[2] = analogRead(A10);
    a_readings[3] = analogRead(A11);
    a_readings[4] = analogRead(A12);
    a_readings[5] = analogRead(A13);
    a_readings[6] = analogRead(A14);
    a_readings[7] = analogRead(A15);

    bool d_readings[8];
    int sum = 0;
    int count = 0;
    for(int i=0;i<8;i++){
        if(a_readings[i]>pivot){
            d_readings[i] = true;
            sum += i+1;
            count++;
            continue;
        }
        d_readings[i] = false;
    }
	c = count;
	left = digitalRead(leftIR);
	right = digitalRead(rightIR);
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
  forward(maxSpeed,150);
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
  delay(200);

  bool flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(leftIR)==0;
  while(!flag){
      analogWrite(7,spd);
      analogWrite(6,spd);
      flag = analogRead(A12)>pivot || analogRead(A11)>pivot && digitalRead(leftIR)==0;
  }
}

void Right(int spd , int state){
    digitalWrite(24,HIGH);
    digitalWrite(22,LOW);
    analogWrite(7,spd);
    digitalWrite(28,LOW);
    digitalWrite(26,HIGH);
    analogWrite(6,spd);
    delay(200);

  bool flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(rightIR)==0;
  while(!flag){
    analogWrite(7,spd);   
    analogWrite(6,spd);
    flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(rightIR)==0;
  }
}

void turn180(int spd ){
  stop(100);

  digitalWrite(24,HIGH);
  digitalWrite(22,LOW);
  analogWrite(7,spd);
  digitalWrite(28,LOW);
  digitalWrite(26,HIGH);
  analogWrite(6,spd);
  delay(500);

  bool flag = analogRead(A12)>pivot || analogRead(A11)>pivot;
  while(!flag){
    analogWrite(7,spd);
    analogWrite(6,spd);
    flag = (analogRead(A12)>pivot || analogRead(A11)>pivot);
  }
}