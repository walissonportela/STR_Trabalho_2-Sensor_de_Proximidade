#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

//Variáveis globais
int sonarTrigger= 7;
int sonarEcho = 6;
int ledRed = 3;
int ledYellow = 4;
int ledGreen = 5;
int distancia;
int tempo;
int buzzer = 2;

SemaphoreHandle_t xSerialSemaphore;
QueueHandle_t xFila;

//Envia valores para a fila
void Task1( void *pvParametros );
//Verifica se o valor recebido é menor que 100 cm
void Task2( void *pvParametros );
//verifica se o valor recebido é maior que 100 cm e menor que 150 cm
void Task3( void *pvParametros );
//verifica se o valor recebido é maior que 150 cm
void Task4( void *pvParametros );

// Cria as tarefas que vão verificar os valores recebidos 
int criarTasks(void)
{
    if(xTaskCreate(Task2,  "Task2",  128, NULL, 1, NULL) == pdPASS)
      Serial.println("Task Task2 criada com sucesso!");
    else
      return 0;  

    if(xTaskCreate(Task3,  "Task3",  128, NULL, 2, NULL) == pdPASS)
      Serial.println("Task Task4 criada com sucesso!");
    else
      return 0;  

    if(xTaskCreate(Task4, "Task4",  128, NULL, 2, NULL) == pdPASS)
      Serial.println("Task Task4 criada com sucesso!");
    else
      return 0;

    return 1;
}

void setup() {

  pinMode(sonarTrigger, OUTPUT);
  pinMode(ledRed,OUTPUT);
  pinMode(ledYellow,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(sonarEcho, INPUT);
  pinMode(buzzer, OUTPUT);
  BaseType_t xRet;
  Serial.begin(9600);

  //Tarefa que enviará os valores lidos pelo sensor para a fila
  //Cada uma das funções de verificação irá analisar os valores na fila
  xTaskCreate(Task1, "Task1",  128, NULL, 1, NULL);

  //Semáforo
  xSerialSemaphore = xSemaphoreCreateMutex();

  if (xSerialSemaphore)
  {
    xFila = xQueueCreate(10, sizeof( int ));
    if(xFila)
    {
      Serial.println("Fila criada com sucesso!");
      if(criarTasks())
      {
          xSemaphoreGive( ( xSerialSemaphore ) );
      }
      else
        Serial.println("Ocorreu erro ao criar as tasks!"); 
    } 
    else
      Serial.println("Ocorreu erro ao criar a fila!"); 
  } 
  else
    Serial.println("Ocorreu erro ao criar o semaforo!"); 

}

void loop() {
}

void Task1( void *pvParametros )
{
  int Item = 0;
  TickType_t proxTime;
  proxTime = xTaskGetTickCount();

  for( ;; )
  {
    digitalWrite(sonarTrigger,LOW); //Estabiliza o sensor
    delayMicroseconds(2);
     vTaskDelay(2 / portTICK_PERIOD_MS);

    digitalWrite(sonarTrigger, HIGH); //Envia um pulso para ativar
    vTaskDelay(2 / portTICK_PERIOD_MS);
    digitalWrite(sonarTrigger,LOW);

    tempo = pulseIn(sonarEcho, HIGH); //Recebe um pulso
    distancia = tempo/58.2; //Distancia = tempo * velocidade do sensor
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("-> Distancia medida: ");
          Serial.print(distancia);
          Serial.print(" cm \n");

          xSemaphoreGive( xSerialSemaphore );

        }
    
    vTaskDelayUntil(&proxTime, (500/portTICK_PERIOD_MS)); 
  
    xQueueSendToBack(xFila, &distancia, portMAX_DELAY);

    vTaskDelayUntil(&proxTime, (1000/portTICK_PERIOD_MS)); 
  }
}

void Task2( void *pvParametros ) {
  int  Item;

  for( ;; )
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( Item < 100 )
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("DISTÂNCIA PERIGOSA!\n");
          xSemaphoreGive( xSerialSemaphore );

        }
        digitalWrite(ledRed,HIGH);
        //digitalWrite(buzzer,HIGH);
        tone(buzzer,392);
         vTaskDelay( 500 / portTICK_PERIOD_MS );
         digitalWrite(ledRed,LOW);
         digitalWrite(buzzer,LOW);
         noTone(buzzer); 
        
    }
    else
    {
       vTaskDelay(500 / portTICK_PERIOD_MS);   
    }
  }
}

void Task3( void *pvParametros ) {
  int  Item;

  for( ;; )
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( (Item >= 100)&& (Item <= 150))
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("DISTÂNCIA RASOÁVEL!\n");
          xSemaphoreGive( xSerialSemaphore );

        }
         digitalWrite(ledYellow,HIGH);
         vTaskDelay( 500 / portTICK_PERIOD_MS );
         digitalWrite(ledYellow,LOW);
    }
    else
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);  
    }
  }
}

void Task4( void *pvParametros ) {

  int  Item;

  while(1)
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( Item > 150 )
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("DISTÂNCIA SEGURA!\n");
          xSemaphoreGive( xSerialSemaphore );

        }
         digitalWrite(ledGreen,HIGH);
         vTaskDelay( 500 / portTICK_PERIOD_MS );
         digitalWrite(ledGreen,LOW);
    }
    else
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);   
    }
  }

}