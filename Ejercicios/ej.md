```mermaid
stateDiagram-v2
    [*] --> EsperandoMuestras: Inicial

    EsperandoMuestras --> RecolectandoMuestras: Interrupción ADC

    RecolectandoMuestras --> SobrecargaADC: Sobrecarga detectada (burst)
    SobrecargaADC --> Configuración: del temporizador y ADC

    RecolectandoMuestras --> CalcularPromedio: N muestras recolectadas
    CalcularPromedio --> EsperandoMuestras: Promedio calculado

    Configuración --> [*]: Error manejado
