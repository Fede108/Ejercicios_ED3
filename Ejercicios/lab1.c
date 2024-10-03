#define MAX_SIZE 100

#include <stdio.h>
#include <stdint.h>

// Definición de la estructura del mensaje
typedef struct {
    uint8_t  type;  // Tipo de mensaje (por ejemplo, 1 para texto, 2 para comando, etc.)
    uint16_t length;       // Longitud de los datos del mensaje
    char data[MAX_SIZE];        // Datos del mensaje (por ejemplo, texto, comandos, etc.)
} Mensaje;

void inicializar_mensaje(Mensaje *msg, uint8_t type) {
    msg->type = type;
    switch (type) {
        case 1:  // Tipo 1
        *(msg->data) = "primer mensaje";

        case 2:  // Tipo 1
        *(msg->data) = "segundo mensaje";
    
    }
    // Calcula la longitud del mensaje y la asigna al campo length
    msg->length = strlen(msg->data);
}


int main(void)

{
    Mensaje mensaje;
    Mensaje *mensajePtr = &mensaje;
    
    void (*func_ptr)(Mensaje*, uint8_t ) = inicializar_mensaje;

    func_ptr(mensajePtr, 1);

    // Imprimir el mensaje inicializado
    printf("Tipo: %d\n", mensajePtr->type);
    printf("Longitud: %d\n", mensajePtr->length);
    printf("Datos: %s\n", mensajePtr->data);
        
}



