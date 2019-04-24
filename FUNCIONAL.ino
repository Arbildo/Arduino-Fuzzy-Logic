#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

//variable a tener x cantidad de lectura 
const int numofReadings = 10;
//numeros de lecturas para tomar / articulos de la matriz
int readings[numofReadings];
//almacena la distancia leidas en el arreglo
int arrayIndex = 0;
//arrayIndex de el elemento actual en la matriz
int total = 0;
//Tiempo de pulso en Micro segundos
unsigned long pulseTime = 0;
// variable for storing the distance (cm)
unsigned long dist = 0;
// almacena el total acumulado
int averageDistance = 0;
// DYP-ME007 echo pin (digital 5)
int echoPin = 7;
int initPin = 8;
//Inicializando motor 1 derecha
int EN1 = 10; //HACE CORRER HACIA DELANTE



//Iniciando motor 2 izquierda
int EN2 = 12;

Fuzzy* fuzzy = new Fuzzy();

void setup() {
  Serial.begin(9600);
  pinMode(EN1,HIGH); //motor 1 derecha
  pinMode(EN2,HIGH); //motor 2 izquierda
  pinMode(initPin, OUTPUT); /*activación del pin 8 como salida: para el pulso ultrasónico*/
  pinMode(echoPin, INPUT); /*activación del pin 7 como entrada: tiempo del rebote del ultrasonido*/
  
for(int thisReading=0;thisReading>numofReadings;thisReading++){readings[thisReading]=0;}

FuzzyInput* distancia=new FuzzyInput(1);
FuzzySet* chica = new FuzzySet(0, 20, 20, 40); // Distancia pequena
  distancia->addFuzzySet(chica); // Adicionando o FuzzySet small em distance
  FuzzySet* seguro = new FuzzySet(30, 50, 50, 70); // Distancia segura
  distancia->addFuzzySet(seguro); // Adicionando o FuzzySet safe em distance
  FuzzySet* grande= new FuzzySet(60, 80, 80, 80); // Distancia grande
  distancia->addFuzzySet(grande); // Adicionando o FuzzySet big em distance
  
  fuzzy->addFuzzyInput(distancia); // Adicionando o FuzzyInput no objeto Fuzzy

FuzzyOutput* velocidad1=new FuzzyOutput(1);

 FuzzySet* Detenido = new FuzzySet(LOW, HIGH, HIGH, LOW); // Velocidade lenta
  velocidad1->addFuzzySet(Detenido); // Adicionando o FuzzySet slow em velocity
  FuzzySet* Normal = new FuzzySet(10, 20, 30, 40); // Velocidade normal
  velocidad1->addFuzzySet(Normal);
  
  fuzzy->addFuzzyOutput(velocidad1);
  
  
  FuzzyOutput* velocidad2=new FuzzyOutput(2);
  FuzzySet* Detenido2=new FuzzySet(HIGH,LOW,HIGH,LOW);
  velocidad2->addFuzzySet(Detenido2);
  FuzzySet* Normal2 = new FuzzySet(10,20,30,40);
  velocidad2->addFuzzySet(Normal2);
  
  fuzzy->addFuzzyOutput(velocidad2);
  
  FuzzyRuleAntecedent* ifDistanciaChica=new FuzzyRuleAntecedent();
  ifDistanciaChica->joinSingle(chica);
  FuzzyRuleConsequent* thenVelocidad1DetenidoAndVelocidad2Normal=new FuzzyRuleConsequent();
  thenVelocidad1DetenidoAndVelocidad2Normal->addOutput(Detenido);
  thenVelocidad1DetenidoAndVelocidad2Normal->addOutput(Normal2);
  FuzzyRule* Regla1=new FuzzyRule(1,ifDistanciaChica,thenVelocidad1DetenidoAndVelocidad2Normal);
    fuzzy->addFuzzyRule(Regla1);

    
  FuzzyRuleAntecedent* ifDistanciaSeguro=new FuzzyRuleAntecedent();
  ifDistanciaSeguro->joinSingle(seguro);
  FuzzyRuleConsequent* thenVelocidad1NormalAndVelocidad2Detenido=new FuzzyRuleConsequent();
  
  thenVelocidad1NormalAndVelocidad2Detenido->addOutput(Normal);
  thenVelocidad1NormalAndVelocidad2Detenido->addOutput(Detenido2);
  FuzzyRule* Regla2=new FuzzyRule(2,ifDistanciaSeguro,thenVelocidad1NormalAndVelocidad2Detenido);
    fuzzy->addFuzzyRule(Regla2);
    
    FuzzyRuleAntecedent* ifDistanciaGrande=new FuzzyRuleAntecedent();
  ifDistanciaGrande->joinSingle(grande);
  FuzzyRuleConsequent* thenVelocidad1NormalAndVelocidad2Normal=new FuzzyRuleConsequent();
  thenVelocidad1NormalAndVelocidad2Normal->addOutput(Normal);
  thenVelocidad1NormalAndVelocidad2Detenido->addOutput(Normal2);
  FuzzyRule* Regla3=new FuzzyRule(2,ifDistanciaSeguro,thenVelocidad1NormalAndVelocidad2Detenido);
    fuzzy->addFuzzyRule(Regla3);

  
}

void loop() {
  
  digitalWrite(initPin, HIGH);
  // wait 10 microseconds before turning off
  delayMicroseconds(10);
  // stop sending the pulse
  digitalWrite(initPin, LOW);
  
  // Look for a return pulse, it should be high as the pulse goes low-high-low
  pulseTime = pulseIn(echoPin, HIGH);
  // Distance = pulse time / 58 to convert to cm.
  dist = pulseTime/58;
  // subtract the last distance
  total = total - readings[arrayIndex];
  // add distance reading to array
  readings[arrayIndex] = dist;
  // add the reading to the total
  total = total + readings[arrayIndex];
  
  arrayIndex = arrayIndex + 1;
  // go to the next item in the array
  // At the end of the array (10 items) then start again
  if (arrayIndex >= numofReadings) {
    arrayIndex = 0;
  }
  // calculate the average distance
  averageDistance = total / numofReadings;
  // print out the average distance to the debugger
  
  fuzzy->setInput(1, averageDistance);
  
  fuzzy->fuzzify();

  float output1 = fuzzy->defuzzify(1);
  float output2 = fuzzy->defuzzify(2);
  output1=round(output1);
  output2=round(output2);
  
  if(averageDistance<50){
  output1=output1;
  output2=output2;
  analogWrite(EN1,output1);
  analogWrite(EN2,output2);
    digitalWrite(10,LOW); 
      digitalWrite(11,HIGH);
      digitalWrite(12,HIGH);
      digitalWrite(13,LOW);
      delay(500);
      digitalWrite(10,HIGH); 
      digitalWrite(11,LOW);
      digitalWrite(12,HIGH);
      digitalWrite(13,LOW);
  
  }
  else{
  output1=190;
  output2=190;
  analogWrite(EN1,output1);
  analogWrite(EN2,output2);
  }
  Serial.print("Distancia: ");
  Serial.print(averageDistance);
  Serial.print("Velocidade: ");
  Serial.println(output1);
  Serial.println(output2);
  // wait 100 milli seconds before looping again
  delay(100);
  
  
  // put your main code here, to run repeatedly:

}
  
  


