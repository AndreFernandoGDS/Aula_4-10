#include <stdio.h>                 // Biblioteca padrão de entrada e saída
#include "freertos/FreeRTOS.h"     // Biblioteca principal do FreeRTOS
#include "freertos/task.h"         // Biblioteca para o gerenciamento de tarefas do FreeRTOS
#include "freertos/semphr.h"       // Biblioteca para semáforos (não usado diretamente aqui, mas incluído)
#include "freertos/event_groups.h" // Biblioteca para o uso de grupos de eventos no FreeRTOS

EventGroupHandle_t grupoEvt; // Declaração de um manipulador para o grupo de eventos
const int goCom = BIT0;      // 0b01          // Define um bit de evento para comunicação (bit 0)
const int goSensor = BIT1;   // 0b10          // Define um bit de evento para o sensor (bit 1)

void comTask(void *params)
{
    while (1)
    {
        printf("Recebido dado \n");
        xEventGroupSetBits(grupoEvt, goCom); // Define o bit `goCom` no grupo de eventos
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void sensorTask(void *params)
{
    while (1)
    {
        printf("Realizado leitura\n");
        xEventGroupSetBits(grupoEvt, goSensor); // Define o bit `goSensor` no grupo de eventos
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void procTask(void *params)
{
    while (1)
    {
        // Aguarda até que os bits `goCom` e `goSensor` sejam setados no grupo de eventos
        // `true, true` significa que os bits serão limpos após a espera
        // `portMAX_DELAY` faz com que espere indefinidamente até que ambos os bits sejam setados
        xEventGroupWaitBits(grupoEvt, goCom | goSensor, true, true, portMAX_DELAY); // goCom | goSensor, entrar en ordem com os bits menos significativos, para os mmais sig.

        // Quando ambos os bits estiverem setados, executa o código abaixo
        printf("Recebido requisição e leitura do sensor \n");
    }
}

void app_main(void)
{
    grupoEvt = xEventGroupCreate(); // Cria o grupo de eventos

    // Cria três tarefas: uma de comunicação, uma de sensor e uma de processamento
    xTaskCreate(&comTask, "Task de comunicação", 2048, NULL, 1, NULL); // Tarefa de comunicação
    xTaskCreate(&sensorTask, "Task de sensor", 2048, NULL, 1, NULL);   // Tarefa de sensor
    xTaskCreate(&procTask, "Task de execução", 2048, NULL, 1, NULL);   // Tarefa de processamento
}
