int sonarTrigger= 7;
int sonarEcho = 6;
int ledRed = 3;
int ledYellow = 4;
int ledGreen = 5;
int distancia;
int tempo;
void dist_prox(){
  if(distancia<=100){
    digitalWrite(ledRed,HIGH);
    Serial.print("Perigo!!\n");
    delayMicroseconds(1000);
    digitalWrite(ledRed,LOW);
    
  }
}

void dist_media(){
  if((distancia>100)&&(distancia<300)){
    digitalWrite(ledYellow,HIGH);
    Serial.print("Cuidado!!\n");
    delayMicroseconds(1000);
    digitalWrite(ledYellow,LOW);
  }
}

void dist_longe(){
  if(distancia>300){
    digitalWrite(ledGreen,HIGH);
    Serial.print("Atenção!!\n");
    delayMicroseconds(1000);
     digitalWrite(ledGreen,LOW);
  }
}
 
void task_sensor(){
    digitalWrite(sonarTrigger,LOW);//eSTABILIZA O SENSOR
    delayMicroseconds(2);

    digitalWrite(sonarTrigger, HIGH); //Envia um pulso para ativar
    delayMicroseconds(10);
    digitalWrite(sonarTrigger,LOW);

    tempo = pulseIn(sonarEcho, HIGH); //Função que recebe um pulso
    distancia = tempo/58.2; //Distancia = tempo*velocidade do sensor
    Serial.print("Distancia medida:");
    Serial.print(distancia);
    Serial.print("cm \n");
    delay(1000);// Mede a cada 1 segundo
}

void setup() {
  pinMode(sonarTrigger, OUTPUT);
  pinMode(ledRed,OUTPUT);
  pinMode(ledYellow,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(sonarEcho, INPUT);
  Serial.begin(9600);

}

void loop() {
  task_sensor();
  dist_prox();
  dist_media();
  dist_longe();
}
