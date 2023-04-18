import pandas as pd

# Leer el archivo CSV
df = pd.read_csv('Identificacion_ls.csv')

# Seleccionar la columna que deseas imprimir horizontalmente
columna = df['DAC']

# Transponer la columna en una fila
fila = columna.T

# Escribir la fila en un nuevo archivo CSV
fila.to_csv('DAC_U.csv', index=False)