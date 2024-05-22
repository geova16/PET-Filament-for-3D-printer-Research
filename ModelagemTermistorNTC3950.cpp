// PinoTermistor - pino de conexão do termistor
// ResistorConhecido - resistencia fixa do divisor de tensão
// RnominalTermistor - resistencia quando o termistor esta na temperatura abaixo, usaremos 100000 e 25ºC
// TnominalTermistor - temperatura no qual o termistor tem a resistencia acima
// FatorBeta - fator dado no datasheet do termistor, no caso do ntc3950, beta = 3950

double LeituraParaCelsius(int PinoTermistor, int ResistorConhecido, int RnominalTermistor, int TnomimalTermsistor, int FatorBeta){
  float tensão = analogRead(PinoTermistor); //tensão lida no divisor de tensão
  
   // Conversão da tensão lida para resistência
      tensão = 1023 / tensão - 1;
      tensão = ResistorConhecido / tensão;
  
   //Calculo da temperatura de acordo com o fator Beta
    int temp = tensão / RnominalTermistor;         // (R/Ro)
    temp = log(temp);                              // ln(R/Ro)
    temp /= FatorBeta;                             // 1/B * ln(R/Ro)
    temp += 1.0 / (TnomimalTermsistor + 273.15);     // + (1/To)
    temp = 1.0 / temp;                           
    temp -= 273.15;                                //Kelvin para Graus Celsius
  return temp;
}

// TensaoNominal - tensao do divisor de tensao, 3.3 ou 5v
// PinoTermistor - pino de conexão do termistor
// ResistenciaFixa - resistencia fixa do divisor de tensão
// ResistenciaNominal - resistencia quando o termistor esta na temperatura abaixo, usaremos 100000 e 25ºC
// TemperaturaNominal - temperatura no qual o termistor tem a resistencia acima
// FatorBeta - fator dado no datasheet do termistor, no caso do ntc3950, beta = 3950

double temperatura (int TensaoNominal, int PinoTermistor, int ResistenciaFixa, int TemperaturaNominal,   int ResistenciaNonimal, int FatorBeta){
  
  int TensaoLida = analogRead(PinoTermistor);
  
  int ResistenciaTermistor = ResistenciaNonimal * exp(- FatorBeta/TemperaturaNominal); //RX = R0 * exp (- ß / T0)
  
  double tempertura = FatorBeta * log(ResistenciaFixa/ResistenciaTermistor); //T = ß * ln (R / RX)

  temperatura -= 273.15;                                //Kelvin para Graus Celsius
  
  return tempertura;
}
