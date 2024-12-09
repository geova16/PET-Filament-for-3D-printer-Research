// Include the AccelStepper library:
#include "AccelStepper.h"
#include <Wire.h> // Biblioteca para comunicação I2C
#include <Adafruit_GFX.h> // Biblioteca gráfica para displays
#include <Adafruit_SSD1306.h> // Biblioteca para o display OLED


// Define os pinos para o controle de temperatura
#define pino_termistor 13 // Pino para o termistor NTC 100k
#define pino_mosfet 33 // Pino para o MOSFET que controla a potência da extrusora

// Define os pinos para os LEDs
#define led_PID 5 // LED para indicar o estado do PID
#define led_motor 19 // LED para indicar o estado do motor
#define led_SensorFila 18 // LED para indicar o estado do sensor de filamento
#define pino_buzzer 4 //BUZZER para indicações

// Define os pinos para as chaves
#define pino_chaveMotor 16 // Chave para controlar o motor
#define pino_chavePID 17 // Chave para controlar o PID

// Define os pinos para o motor de passo
//#define pino_motorDePasso 27 // Pino para controlar o motor de passo
#define pino_potenciometroMotorDePasso 25 // Pino para o potenciômetro que ajusta a velocidade do motor de passo
//#define pino_direcmotor 26 // Pino para controlar a direção do motor de passo (deixar sempre alto ou baixo)

// Define o pino para o sensor de filamento
#define pino_sensorFilamento 14 // Pino para o sensor de filamento

// Define as dimensões do display OLED
#define SCREEN_WIDTH 128 // Largura do display OLED
#define SCREEN_HEIGHT 64 // Altura do display OLED
// Define stepper motor connections and motor interface type. 
// Motor interface type must be set to 1 when using a driver
#define dirPin 26
#define stepPin 27
#define motorInterfaceType 1

double leituraTemperatura = 30, Output;

int contador_amostral = 0, leitura_amostral [10], analog_amostral = 0;

//auxiliares para millis()
unsigned long tempoatt = millis();
unsigned long tempoatt2 = millis();
unsigned long tempoatt3 = millis();
unsigned long tempoatt4 = millis();
unsigned long tempoatt5 = millis();


// Cria um objeto para o display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  // Inicia o display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Endereço I2C 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Não prossiga se falhar
  }

  Serial.begin(9600);

  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(1000);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // Configura os pinos para o controle
  pinMode(pino_termistor, INPUT);
  pinMode(pino_mosfet, OUTPUT);

  // Configura os pinos para as sinalizações
  pinMode(led_PID, OUTPUT);
  pinMode(led_motor, OUTPUT);
  pinMode(led_SensorFila, OUTPUT);

  // Configura os pinos para as chaves
  pinMode(pino_chaveMotor, INPUT_PULLUP);
  pinMode(pino_chavePID, INPUT_PULLUP);

}

void loop() {

  //executa o controle
    controle ();
    
    //executa o comportamento dos leds 
    Leds ();

    //executa o display oled
    displayOLED ();

    //executa o motor de passo
    //motorDePasso();

    // Imprime as informações no monitor serial para plotagem
    serialPrinter();

  
}

void controle (void){
  
  // Leitura da temperatura
  leituraTemperatura = celsius(pino_termistor);

  if(digitalRead(pino_chavePID) == LOW){
    if(230 - leituraTemperatura >= 80){
      Output = 255;
    }
    else if(80 > 230 - leituraTemperatura > 40){
      Output = 155;
    }
    else if(230 - leituraTemperatura <= 40){
      Output = 75;
    }
  }

  if(digitalRead(pino_chavePID) == HIGH){
    Output = 0;
  } 
  
  analogWrite(pino_mosfet, Output);
}
/*
double celsius(int pin) {
  uint16_t adc = analogRead(pin);
  // Conversão do valor ADC para resistência
  double resistance = (4095.0 / adc) - 1;
  resistance = 1000 / resistance;

  // Cálculo da temperatura em Celsius usando a fórmula de Steinhart-Hart
  double steinhart;
  steinhart = resistance / 1000;     // (R/Ro)
  steinhart = log(steinhart);                      // ln(R/Ro)
  steinhart /= 3950;                       // 1/B * ln(R/Ro)
  steinhart += 1.0 / (25 + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                     // Inverte
  steinhart -= 273.15;                             // Converte para Celsius

  
  return steinhart;
}*/

double celsius(int pin) {
  uint16_t adc = analogRead(pin);
  // Conversão do valor ADC para resistência
  double resistance = (4095.0 / adc) - 1;
  //double resistance = (adc/4095.0) - 1;
  resistance = 1000 * resistance;

  //double resistance = ((1000/4095) * adc) / (1 / (1 - (adc/4095.0)));

  // Cálculo da temperatura em Celsius usando a fórmula de Steinhart-Hart
  double steinhart;
  steinhart = resistance / 1000;     // (R/Ro)
  steinhart = log(steinhart);                      // ln(R/Ro)
  steinhart /= 3950;                       // 1/B * ln(R/Ro)
  steinhart += 1.0 / (25 + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                     // Inverte
  steinhart -= 273.15;                             // Converte para Celsius

  
  return steinhart;
}

/*
double celsius(int pin){
  int adc = analogRead(pin);
  
  double v0 = 3.3 * adc / 4095.0;

  double Rt = 3.3 * 1000 / (v0 - 1000.0);

  double Rx = 100000.0 * exp(-3950/(273.0 + 25.0));

  double steinhart = (3950 / ( log(Rt / Rx))) - 275.0;

  return steinhart;

}
*/

/*long int leituraAmostral(int pino){

  if(millis() - tempoatt5 > 50){
    leitura_amostral[contador_amostral] = analogRead(pino);
    contador_amostral ++;
    tempoatt5 = millis();
  

    if(contador_amostral == 10){

      for(int j = 0; j < 11; j++){
        analog_amostral = analog_amostral +  leitura_amostral[j];
      }

      analog_amostral = analog_amostral / 10;

    }
  }
  return analog_amostral;
}*/

void motorDePasso (void){
  // Se a chave do motor estiver pressionada e o potenciômetro do motor de passo for maior que 10
  if(digitalRead(pino_chaveMotor) == LOW){
    stepper.setSpeed(-600);
    stepper.runSpeed();
  }
}

void serialPrinter (void){
  Serial.println(leituraTemperatura);
}

void Leds (void){
  if(digitalRead(pino_chavePID) == LOW){
    // Acende o LED do PID
    analogWrite(led_PID, 50);
  }
  if(digitalRead(pino_chavePID) == HIGH){
    // Apaga o LED do PID
    analogWrite(led_PID, 0);
  }
  if(digitalRead(pino_chaveMotor) == LOW){
    // Acende o LED do motor
    analogWrite(led_motor, 50);
  }
  if(digitalRead(pino_chaveMotor) == HIGH){
    // Apaga o LED do motor
    analogWrite(led_motor, 0);
  }

  if(millis() - tempoatt3 > 100){
    digitalWrite(led_SensorFila, !digitalRead(led_SensorFila));
    tempoatt3 = millis();
  }
    
  
}

void displayOLED (void){
  if (millis() - tempoatt2 > 800){
  // Limpa o display
  display.clearDisplay();
  // Configura o tamanho do texto
  display.setTextSize(1);
  // Configura a cor do texto
  display.setTextColor(WHITE);
  // Configura a posição do cursor
  display.setCursor(10, 7);
  // Imprime as informações no display
  display.print("Temp: "); display.print(leituraTemperatura);display.print("/");display.print(230);display.println("C");
  display.println(" ");
  display.setCursor(10, 22);
  display.print("Vel: "); display.print(0/10);display.println("60/100");
  display.println(" ");
  display.setCursor(10, 37);
  display.print("PWM: "); display.print(Output);display.println("/255");
  display.println(" ");
  display.setCursor(10, 52);
  display.print("PET Eletrica UFC");

  // Atualiza o display
  display.display();

  tempoatt2 = millis();

  }
}
