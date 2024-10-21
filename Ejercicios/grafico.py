import numpy as np
import matplotlib.pyplot as plt

# Parámetros
MAX_RESOLUTION = 1023
wave = np.zeros(MAX_RESOLUTION)

# Función para generar una onda triangular asimétrica
def wave_function():
    # Parte ascendente: de 512 a 1023
    for i in range(MAX_RESOLUTION // 2):
        wave[i] = 512 + i
    
    # Parte descendente: de 0 a 511
    for i in range(MAX_RESOLUTION // 2):
        wave[i + 512] = i

# Generar la onda triangular
wave_function()

# Graficar la onda
plt.figure(figsize=(10, 6))
plt.plot(wave, color='red', label='Onda Triangular Asimétrica')
plt.title('Onda Triangular Asimétrica')
plt.xlabel('Tiempo (muestras)')
plt.ylabel('Tensión')
plt.grid(True)
plt.legend()
plt.show()
