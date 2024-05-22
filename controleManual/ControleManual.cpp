#include <PID_v1.h>
#include <thermistor.h>



// DEFINIÇÕES DO CONTROLE
#define pino_entrada 5
#define pino_saida 4
#define pino_pot_kp 3
#define pino_pot_ki 2
#define pino_pot_kd 1
#define pino_termistor 1


//como descobrir o set point? 
//experimentalmente, modificar o valor do set point e verificar o valor retornado pela função therm1.analog2temp()
double Setpoint = 100, Input, Output, ki = 0, kd = 0, kp = 0;
double temperatura;

//Termistor
thermistor therm1(pino_termistor,0);

//Criação do Objeto e inicialização dos parâmetros
PID myPID(&Input, &Output, &Setpoint,kp,ki,kd, DIRECT);

void setup()
{
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  pinMode(pino_entrada, INPUT);
  pinMode(pino_saida, OUTPUT);
  pinMode(pino_pot_kp, INPUT);
  pinMode(pino_pot_ki, INPUT);
  pinMode(pino_pot_kd, INPUT);
  pindMode(pino_termistor, INPUT);
  
}

void loop()
{
  //------------LEITURA DE TEMPERATURA-------------------
  temperatura = therm1.analog2temp(); 

  //------------CONTROLE--------------------------------
  //definição manual dos ganhos, experimentalmente
  kp = analogRead(pino_pot_kp)/4095;
  ki = analogRead(pino_pot_ki)/4095;
  kd = analogRead(pino_pot_kd)/4095;
  //leitura do termistor
  Input = analogRead(pino_entrada);
  //PID
  myPID.Compute(); 
  //saída para a resistência de aquecimento
  analogWrite(pino_saida,Output); 

  //---------------OLED---------------------------------

  //---------------DRIVER STEP MOTOR--------------------

  Serial.print("Kp: ");
  Serial.println(kp);

  Serial.print("Ki: ");
  Serial.println(ki);

  Serial.print("Kd: ");
  Serial.println(kd);

  Serial.print("Input: ");
  Serial.println(Input);

  Serial.print("Output: ");
  Serial.println(Output);

  Serial.print("Setpoint: "); 
  Serial.println(Setpoint);

  Serial.print("Temperatura");
  Serial.println((String)temperatura);
}
