import serial
import time
import mysql.connector
from mysql.connector import Error
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
connection = mysql.connector.connect(host='localhost', database='dbSE', user='root',password='root')
cursor = connection.cursor()
mySql_insert_query = "INSERT INTO Produtos VALUES (\""
mySql_delete_query = "DELETE FROM Produtos WHERE Tag = \""
mySql_select_query = "SELECT * FROM Produtos WHERE Tag = \""
try:
	while 1:
		time.sleep(2)
		s = ser.readline()
		if s != b'':
			##print(s)
			tag = s.decode()
			if tag[0] == 'I':
				try:
					print("Insira o nome do produto a ser registrado:")
					nome_produto = input()
					print("Insira o valor do produto a ser registrado:")
					valor_produto = input()
					ins = mySql_insert_query + tag[1:] + "\",\"" + nome_produto + "\"," + valor_produto + ");"
					result = cursor.execute(ins)
					connection.commit()
					ret = tag[1:] + " cadastrado com sucesso."	
					print(ret)
				except mysql.connector.IntegrityError as err:
					ret = "Erro: Tag " + tag[1:] + " ja esta cadastrada"
					print(ret)
				except mysql.connector.errors.ProgrammingError as err:
					ret = "Erro: uma ou mais entradas para o cadastro do produto são inválidas."
					print(ret)
				
			elif tag[0] == 'R':
				rem = mySql_delete_query + tag[1:] + "\""
				result = cursor.execute(rem)
				connection.commit()
				if cursor.rowcount > 0:
					status = "1"
					ret = tag[1:] + " removido com sucesso."
					print(ret)
				else:
					status = "0"
					ret = tag[1:] + " nao existe no banco de dados."
					print(ret)
				ser.write(status.encode())
			elif tag[0] == 'S':
				sel = mySql_select_query + tag[1:] + "\""
				cursor.execute(sel)
				record = cursor.fetchall()
				if len(record) == 0:
					status = "0"
					ret = "Produto nao cadastrado."
					print(ret)
					ser.write(status.encode())
				else:
					for row in record:
						valor = str(row[2])
						ret = "Nome: " + row[1] + " Valor: " + valor
						status = "1" + valor + ":" + row[1]
						status = status[:32]
						print(ret)
						ser.write(status.encode())   
except KeyboardInterrupt:				
	ser.close()
	print("Programa finalizado.")
