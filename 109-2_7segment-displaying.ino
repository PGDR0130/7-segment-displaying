/*
109-2 多元選修 微控制器應用 最後學習成果報告+紀錄
by.11311 張哲誠 (2021/7/14) 最後檢查
七段顯示器顯示 溫度+濕度
*/
#include "DHT.h"
#define DHTPIN A4 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//                            PIN  A,B,C,D,E,F,G
byte seven_seg_digits[][8] = {   { 0,0,0,0,0,0,1 },  // = 0
                                 { 1,0,0,1,1,1,1 },  // = 1
                                 { 0,0,1,0,0,1,0 },  // = 2
                                 { 0,0,0,0,1,1,0 },  // = 3
                                 { 1,0,0,1,1,0,0 },  // = 4
                                 { 0,1,0,0,1,0,0 },  // = 5
                                 { 0,1,0,0,0,0,0 },  // = 6
                                 { 0,0,0,1,1,1,1 },  // = 7
                                 { 0,0,0,0,0,0,0 },  // = 8
                                 { 0,0,0,0,1,0,0 }   // = 9
                             };
byte seven_seg_word[][8] = {     { 0,1,1,0,0,0,1 },  // = C --> 代表溫度(0) - 攝氏
                                 { 1,0,0,1,0,0,0 },  // = H --> 代表濕度(1)
                                 { 0,1,1,1,0,0,0 }   // = F --> 代表溫度(3) - 華氏
                           };

int mode = 0; //mode0 --> temperature(C); mode1 --> humidity; mode2 -->temperature(F) DEFULT mode0
float delay_time = 500;
float last_delay_time = millis(); // 因為要偵測按鈕所以不能用dealy不然程序會被卡住無法偵測
int state = 0 ;


byte result_of_number_list[2]; //公開的array
void number_list(int number){
  int len = floor(log10(number)+1); //數字的長度: floor()--> 無條件捨棄到個位數, log10()-->開log10()+1就看得出有幾位 
  if(len>2) return 0;
  for(byte i=0; i<2; i++){
    int r = number%10;
    number /= 10;
    result_of_number_list[1-i] = r;
  }
}

void writeWord(byte num){
  for(byte pin = 0; pin<7; pin++){
    digitalWrite(pin+2, seven_seg_word[num][pin]);
  }
}

void writeNumber(byte num){
  for(byte pin = 0; pin<7; pin++){
    digitalWrite(pin+2, seven_seg_digits[num][pin]);
  }
}


/*
---------------------------------------------------------------------
這個可以讓輸出相隔的自有延遲但不會引響程序。
---------------------------------------------------------------------
*/
int wait_true_out(int in_state, int in_delay, int word_type){
  if(in_state == 0){
    if(millis()-last_delay_time >= in_delay){
      writeWord(word_type);
      last_delay_time = millis();
      return 1;
    }  
  }
  else if(in_state == 1 || in_state == 2 ){
    if(millis()-last_delay_time >= in_delay){
      writeNumber(result_of_number_list[state-1]);
      last_delay_time = millis();
      return 1 ;
    }
  }
  if(state == 3) state = 0;
  return 0 ;
}


/*
set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up
set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up
set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up-set-up
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();


  for(int i=0; i<=8; i++) pinMode(i, OUTPUT);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT);

  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  digitalWrite(11, 0);
  digitalWrite(12, 1);
  
}





/*
LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP 
LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP 
*/

float sl = millis();
int now = 0;

int b1_static = 0;
int b2_static = 0;

void loop(){
  // put your main code here, to run repeatedly:
  
  int temp = dht.readTemperature(); //讀溫度
  int hum = dht.readHumidity(); //讀濕度

  int b1 = digitalRead(A2);//按鈕偵測
  int b2 = digitalRead(A3);//按鈕偵測


//---(按鈕感測)----------------------------------------------------------------------- 
  //button 1 (b1) 
  if(b1_static == 2) b1_static = 0;
  if(b1 == 0 && b1_static == 0) b1_static += 1;
  if(b1 == 1 && b1_static == 1) b1_static += 1;
  
  //button 2 (b2)
  if(b2_static == 2) b2_static = 0;
  if(mode != 1 && b2 == 0 && b2_static == 0) b2_static += 1;
  if(mode != 1 && b2 == 1 && b2_static == 1) b2_static += 1;

//---(mode 切換)---------------------------------------------------------------------
  if(b1_static == 2){
    if(mode == 0) mode = 1; else mode =0;
  }
  if(b2_static == 2){
    if(mode == 0) mode =3; else mode =0;
  }
//---(指示燈切換)----------------------------------------------------------------------

  if (mode == 0){digitalWrite(11, 0); digitalWrite(12, 1);} //mode0溫度(C) - 亮紅燈
  if (mode == 1){digitalWrite(11, 1); digitalWrite(12, 0);} //mode1濕度(%) - 亮綠燈
  if (mode == 3){                                           //mode3溫度(F) - 閃紅燈
     if (millis()-sl >= 500){
        if(now == 1) now = 0; else now =1;
        sl = millis();
     }
     digitalWrite(11 ,now);
  }
//---(7段顯示器)------------------------------------------------------------------------
  if (mode == 0){
    
    number_list(temp);
    
    int check_out = wait_true_out(state, delay_time , 0);
    if(check_out == 1) state += 1;
    }
    
  else if(mode == 1){
    number_list(hum);
    int check_out = wait_true_out(state, delay_time, 1);
    
    if(check_out == 1) state += 1; 
    }
    
  else if (mode == 3){
    number_list(int(floor(temp*(9.0/5.0)+32)));

    int check_out = wait_true_out(state, delay_time, 2);
    if (check_out == 1) state += 1;
    }
}
