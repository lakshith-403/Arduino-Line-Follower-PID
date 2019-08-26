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

void loop(){
  mainCode();
}

void mainCode(){
	int err = getErr();
	if(left+right!=0){//junction
		stop(10);
		if(left==1 && right==1){//T
			forword(255,60);
			getErr();

			if(c!=0)//T
                Left(255,1);
			else//Cross
                Left(255,1);
			
		}else if(left==1 && right==0){//Left
			forword(255,60);
			getErr();

			if(c!=0)//  -|
                Left(255,1);
			else//L
                Left(255,1);
			
		}else{//Right
			forword(255,60);
			getErr();
      
			if(c!=0){//  |-
        
			}else//R
                Right(255,1);
			
		}
	}else if(c>=6){//Y
      
      forword(255,200);
      
      Left(255,1);

      bool flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      while(!flag){
        err = getErr();
        pid(err);
        flag = (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      }
      forword(255,200);
      
      Right(255,1);
      
      flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      while(!flag){
        err = getErr();
        pid(err);
        flag =  (digitalRead(leftIR)==1 || digitalRead(rightIR)==1);
      }
      forword(255,200);
      Left(255,1);
	}else if(c==0)//LineEnd
        turn180(255);
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

void forword(int spd , int dlay){
	digitalWrite(24,HIGH);
	digitalWrite(22,LOW);
	analogWrite(7,spd);
	digitalWrite(28,HIGH);
    digitalWrite(26,LOW);
    analogWrite(6,spd);
	delay(dlay);
}

void backword(int spd , int dlay){
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
	analogWrite(7,255);
	digitalWrite(28,HIGH);
    digitalWrite(26,HIGH);
    analogWrite(6,255);
	delay(dlay);
}

void Left(int spd , int state){
  bool flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(leftIR)==0;
  while(!flag){
      digitalWrite(24,LOW);
      digitalWrite(22,HIGH);
      analogWrite(7,spd*state);
      digitalWrite(28,HIGH);
      digitalWrite(26,LOW);
      analogWrite(6,spd);
      flag = analogRead(A12)>pivot || analogRead(A11)>pivot && digitalRead(leftIR)==0;
    }
}

void Right(int spd , int state){
  bool flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(rightIR)==0;
  while(!flag){
    digitalWrite(24,HIGH);
	digitalWrite(22,LOW);
	analogWrite(7,spd);
	digitalWrite(28,LOW);
    digitalWrite(26,HIGH);
    analogWrite(6,spd*state);
    flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) && digitalRead(rightIR)==0;
  }

 
}

 void turn180(int spd ){
   stop(100);
      bool flag = (analogRead(A12)>pivot || analogRead(A11)>pivot) ;
    while(!flag){
        digitalWrite(24,HIGH);
        digitalWrite(22,LOW);
        analogWrite(7,spd);
        digitalWrite(28,LOW);
        digitalWrite(26,HIGH);
        analogWrite(6,spd);
        flag = (analogRead(A12)>pivot || analogRead(A11)>pivot);
    }
  }