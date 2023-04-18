import pandas as pd

# Leer el archivo CSV
df = pd.read_csv('data_ls.csv')

# Seleccionar la columna que deseas imprimir horizontalmente
columna = df['ADC']

# Transponer la columna en una fila
fila = columna.T

# Escribir la fila en un nuevo archivo CSV
fila.to_csv('ADC_Y.csv', index=False)