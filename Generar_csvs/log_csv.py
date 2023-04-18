import csv

# Leer el archivo de registro
with open('log_ls.log', 'r') as f:
    lineas = f.readlines()

# Escribir cada línea del archivo de registro en el archivo CSV
with open('data_ls.csv', 'w') as f:
    writer = csv.writer(f)
    for linea in lineas:
        writer.writerow([linea.strip()])