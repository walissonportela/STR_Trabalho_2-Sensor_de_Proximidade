#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

int sonarTrigger= 7;
int sonarEcho = 6;
int ledRed = 3;
int ledYellow = 4;
int ledGreen = 5;
int distancia;
int tempo;

SemaphoreHandle_t xSerialSemaphore;
QueueHandle_t xFila;

//Envia valores para a fila
void Task1( void *pvParametros );
//Verifica se o valor é menor que 100 cm
void Task2( void *pvParametros );
//verifica se o valor é maior que 100 cm e menor que 150 cm
void Task3( void *pvParametros );
//verifica se o valor é maior que 150 cm
void Task4( void *pvParametros );

int criarTasks(void)
{
    if(xTaskCreate(Task1,  "Task1",  128, NULL, 1, NULL) == pdPASS)
      Serial.println("Task Task1 criada com sucesso!");
    else
      return 0;  

    if(xTaskCreate(Task2,  "Task2",  128, NULL, 2, NULL) == pdPASS)
      Serial.println("Task Task2 criada com sucesso!");
    else
      return 0;  

    if(xTaskCreate(Task3, "Task3",  128, NULL, 3, NULL) == pdPASS)
      Serial.println("Task Task3 criada com sucesso!");
    else
      return 0;  
   if(xTaskCreate(Task4, "Task4",  128, NULL, 4, NULL) == pdPASS)
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
  BaseType_t xRet;
  Serial.begin(9600);
  //xTaskCreate(Task4, "Task4",  128, NULL, 2, NULL);
    xSerialSemaphore = xSemaphoreCreateMutex();
    if (xSerialSemaphore)
  {
    xFila = xQueueCreate(10, sizeof( int ));
    if(xFila)
    {
      Serial.println("Fila criado com sucesso!");
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
  /*
   if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
         Serial.println("Inciando loop: Task1");

          xSemaphoreGive( xSerialSemaphore );

        }
  */
  for( ;; )
  {
    digitalWrite(sonarTrigger,LOW);//eSTABILIZA O SENSOR
    
     vTaskDelay(2 / portTICK_PERIOD_MS);

    digitalWrite(sonarTrigger, HIGH); //Envia um pulso para ativar
    vTaskDelay(2 / portTICK_PERIOD_MS);
    digitalWrite(sonarTrigger,LOW);

    tempo = pulseIn(sonarEcho, HIGH); //Função que recebe um pulso
    distancia = tempo/58.2; //Distancia = tempo*velocidade do sensor
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("Distancia medida:");
           Serial.print(distancia);
          Serial.print("cm \n");

          xSemaphoreGive( xSerialSemaphore );

        }
    
    vTaskDelay(500/ portTICK_PERIOD_MS);
  

  
  
    xQueueSendToBack(xFila, &distancia, portMAX_DELAY);
    //Item=distancia;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2( void *pvParametros ) {
  int  Item;
  /*
   if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
         Serial.println("Inciando loop: Task2");

          xSemaphoreGive( xSerialSemaphore );

        }
        */
//vTaskDelay( 500 / portTICK_PERIOD_MS );
 
 //vTaskDelay( 500 / portTICK_PERIOD_MS );
  //loop da task
  for( ;; )
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( Item < 100 )
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("Distância perigosa: ");
          Serial.println(Item);


          xSemaphoreGive( xSerialSemaphore );

        }
        digitalWrite(ledRed,HIGH);
         vTaskDelay( 500 / portTICK_PERIOD_MS );
         digitalWrite(ledRed,LOW);
    }
    else
    {
       vTaskDelay(500 / portTICK_PERIOD_MS);   
    }
  }
}

void Task3( void *pvParametros ) {
  int  Item;
  /*if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
         Serial.println("Inciando loop: Task3");

          xSemaphoreGive( xSerialSemaphore );

  }*/

  //loop da task
  for( ;; )
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( (Item >= 100)&& (Item <= 150))
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("Distância mediana: ");
          Serial.println(Item);

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
  /*
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
         Serial.println("Inciando loop: Task4");

          xSemaphoreGive( xSerialSemaphore );

        }
*/
  //loop da task
  while(1)
  {
    xQueuePeek( xFila, &Item, portMAX_DELAY );

    if( Item > 150 )
    {
        xQueueReceive( xFila, &Item, portMAX_DELAY );

        if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE )
        {
          Serial.print("Distância correta: ");
          Serial.println(Item);

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
