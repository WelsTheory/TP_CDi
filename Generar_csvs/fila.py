import csv

# Abrir el archivo CSV en modo escritura
with open('DAC_U.csv', 'r') as f:
    reader = csv.reader(f)
    data = list(reader)

# Seleccionar la columna que deseas imprimir horizontalmente
columna = [fila[0] for fila in data][:391]

# Concatenar los elementos de la columna en una cadena separada por comas
fila = ','.join(columna)

# Escribir la fila en un nuevo archivo CSV
with open('DAC_U.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow([fila])