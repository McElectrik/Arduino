#include <IRremote.h>  //биб-ка ИК
#include <avr/pgmspace.h> //биб-ка для работы с массивом данных
#include "sounddata.h"    // массив данных

unsigned long oldtime,     // программный таймер
               timeFire,  // скорострельность
               timemillis, //  программный таймер
               timeResist; // время уезвимости
               
byte RECV_PIN = 11;    // выход ИК-приемника
byte button = 2;       //кнопка выстрела
int i,i_end;          // начало конец звук фрагмента
unsigned long ct8khz;  // выборка байта с частотой ~ 8000 Гц
byte speakerPin = 10;  //выход звука
byte LedGameOver = 13; // светодиод повязки
int life;     // жизни        
int bullet;   // патроны
boolean flagGameOver;  //состояние игры
IRsend irsend;             // выход ик вывод 3
IRrecv irrecv(RECV_PIN);  //определяем вход
decode_results results;   //результат декодирования


     
void Playsound ()   // подпрограмма вывода звука
     {
       if (i_end>0)           // если байт конца фрагмента равен 0 то звука нет
        {
         if (ct8khz < micros())   // если таймер переполнился 
           {  
	     analogWrite(speakerPin, pgm_read_byte(&data[i])); // помещаем байт из массива Шим генерато

             i++;        // увеличить счетчик  в массиве
               ct8khz = micros()+115;  // выборка байта с частотой ~ 8000 Гц
            }
	      if (i>i_end)      // если конец фрагмента  конец воспроизведения
                  {
                    i=0;
		    i_end=0;
                    
                  }  
        }
     }
   
void Updategame()  //обновление игры
  { 
   timemillis = millis(); 
    if (timeResist < timemillis)     // Шок 2 сек
     {
      
      if (life > 0) digitalWrite(LedGameOver, LOW);  // выкл светодиод 
     }
     if (life < 1 && flagGameOver == LOW )
       {
        flagGameOver = HIGH;
        
        i=9600;             // звук game
        i_end=18299;         // over
        
        digitalWrite(LedGameOver, HIGH);  // вкл светодиод
       }
     
       
      
  }   
  
  
void UpdateIRin() // обработка принятых данных
   {
    if (irrecv.decode(&results)) 
     {
       //Serial.println(results.value); //"показываем" принятый код
       if (results.value == 16748655) // Пополнение life bullet  
        {
          timeResist = timemillis+2000;
          life = 3;   // + 3 жизни          
          digitalWrite(LedGameOver, LOW);
          bullet = 100;  //  +100 патронов
          i=18300;     //  звук
          i_end=20299; //   перезаряда
          flagGameOver = LOW;
        }
     if (life > 0)   // если живой
     {   
       
      if (results.value == 1000 && timeResist < timemillis) // если выстрел противника 
        {
          
          timeResist = timemillis+2000;  // уязвимость 2с
          
          life--;      //  -1 жизнь 
          i=4800;   //  звук
          i_end=9599;   //  попадания
          digitalWrite(LedGameOver, HIGH); // вкл светодиод
        }
                
       if (results.value == 16753245) // Game Over   ( Кнопка пульта )
        {
          life = 0;  // 0 жизней
        }
      
    
                      
   }
   irrecv.resume();   
  }
 }
 void UpdateIRout()   // подпрограмма выстрела
   {
    if (life > 0 && timeResist < timemillis)  // если живой и уязвимый
    {
     if (digitalRead(button) == LOW && timeFire < timemillis ) // если нажат курок
      {
       if (bullet > 0) // если есть патроны
         {
          irsend.sendRC5(1000, 10);  //выстрел ( ик данные )
           
            bullet--; // -1 патрон
             
             i=0;    // звук
            i_end=4799; // выстрела
            
              
            irrecv.enableIRIn();      // запускаем прием
         }
          else              // иначе если нет патронов
           {
              i=19000;    // звук
            i_end=20000 ; // осечки
           }       
         timeFire = timemillis + 11
         0;  //скорострельность  
        }  
       }  
      }     
 
void setup() 
   {
    //Serial.begin(9600);
     irrecv.enableIRIn();      // "запускаем" прием
       //pinMode(13, OUTPUT);    // назначить выводу порт ввода
    
 
    
      pinMode(button,  INPUT_PULLUP);           // назначить выводу порт ввода
    digitalWrite(button, HIGH);                 // включить подтягивающий резистор
    
    pinMode(LedGameOver,OUTPUT);     // назначить выводу порт ввода
    digitalWrite(LedGameOver,LOW);   // выкл светодиод
    
     TCCR1A = TCCR1A & 0xe0 | 1;     // ШИМ 8 разрядов
     TCCR1B = TCCR1B & 0xe0 | 0x09;  //   62,5 кГц
 
    }
    
    
    
void loop() {
   

  
        
   
 Playsound(); 
  
  UpdateIRin(); 
      
  Updategame();
  
  UpdateIRout();
  
  
}
