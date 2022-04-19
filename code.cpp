#define NPX 256 //количество пикселей матрицы
#define PORT_MATX 13 //логический порт матрицы
#define ROTATE_MATRIX 0 //классическая

#include "Adafruit_NeoPixel.h"
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPX, PORT_MATX, NEO_GRB + NEO_KHZ800);

//пин кнопки
const int buttonPin = 8;

int x = 1; //положение по x шарика

int pol = 0;
int potolok = 15;

int gg_speed = 1;

long int level;
long int start_time = -1;
long int step_time;

long int downtime;
int downexit = -1;
int down_flag = 0;

int r=0;

//стена [level1; level2] - свободное пространство
struct wall
{
    int level1;
    int level2;
    int x;
};

wall a1;

wall wallarr[3];

int cord(int x, int y, int r){
  int c, i;
  for(i=0; i<r%4; i++){    
      c = x;
      x = y;
      y = 15-c;
  }
  int n_pin = x*16;
  if(x%2==0) n_pin+=y;
  else n_pin+=15-y;
  return n_pin;
}


void darkness(){
  int i;
  for(i=0; i<256; i++)  {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

wall get_wall(int x){

  int d = random(4, 11);
  int level1 = random(0, 15-d);
  int level2 = level1+d;
  
  wall c = {level1, level2, x};
  return c;
}

void setup() 
{ 
  a1.level1 = (int)4;
  a1.level2 = 8;
  a1.x = 6;

  wallarr[0] = get_wall(10);
  wallarr[1] = get_wall(16);
  wallarr[2] = get_wall(22);

  level = 0.5*(pol+potolok);
  
  strip.begin(); // Функция begin() настроит порт Arduino и выставит значения по умолчанию. 
  strip.setBrightness(40); //яркость
  strip.show();  // Функция show() передаст команду на выключение всем пикселям. 

  downtime = millis();
  start_time = millis();
   step_time = millis();

  //Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
} 



void loop()
{
  int i, j;

  //затяжка
  if( digitalRead(buttonPin) ){
    down_flag = 1;
  }
  
  if( (millis() - downtime) > 5 ){
    downtime = millis();
    if(down_flag){
      downexit = 1;
    }
    else{
      downexit = 0;
    }
    down_flag = 0;
  }

  //установка уровня
  if( (millis() - start_time) > 20 ){
    start_time = millis();
    if(downexit){
      level+=1;
      if(level>potolok-1){
        level = potolok-1;  
      }
    }
    else{
      level-=1;
      if(level<pol+1){
        level = pol+1;  
      }
    }

  }

  if( (millis() - step_time) > 50 ){
    step_time = millis();
    step_world();
  }

  //рисование
  draw();
}

//РИСОВАНИЕ--------------------------------------------------------------------------------------------------------
void draw(){
  int i;
  
  darkness();
  
  strip.setPixelColor(cord(x, 15-level,  r), 180, 30, 150);  

  for(i=0; i<3; i++){
    draw_wall(wallarr[i]);
  }

  strip.show();
}

//рисование стены
void draw_wall(wall c){
    int lvl;

    if(c.x>=0 && c.x<=15){
      for(lvl = 0; lvl<c.level1; lvl++){
        strip.setPixelColor(cord(c.x, 15-lvl,  r), 235, 30, 30);  
      }
  
      for(lvl = c.level2+1; lvl<16; lvl++){
        strip.setPixelColor(cord(c.x, 15-lvl,  r), 235, 30, 30);  
      }
    }
}

void step_world(){
  int i;
  
  for(i=0; i<3; i++){
    wallarr[i].x-=1;
    if(wallarr[i].x==1 && !( level>=wallarr[i].level1 && level<=wallarr[i].level2 )){
      game_over();  
    }
    if(wallarr[i].x==-1){
      wallarr[i] = get_wall(16);
    }
  }    
}

void game_over(){
  int i;
  
  darkness();
  for(i=0; i<6; i++){
    strip.setPixelColor(cord(6, 2+i,  r), 235, 30, 30);
    strip.setPixelColor(cord(9, 2+i,  r), 235, 30, 30);
  }

  for(i=0; i<4; i++){
    strip.setPixelColor(cord(6+i, 12,  r), 235, 30, 30);
  }

  strip.setPixelColor(cord(5, 13,  r), 235, 30, 30);
  strip.setPixelColor(cord(10, 13,  r), 235, 30, 30);

  strip.show();
  
  delay(2000);
  
  setup();
}
