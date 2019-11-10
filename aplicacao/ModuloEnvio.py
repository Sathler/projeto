import serial
import time
import mysql.connector
from mysql.connector import Error
ser = serial.Serial('COM6', 9600, timeout=0)
connection = mysql.connector.connect(host='localhost', database='dbSE', user='root',password='')
cursor = connection.cursor()
mySql_insert_query = "INSERT INTO Produtos VALUES (\""
mySql_delete_query = "DELETE FROM Produtos WHERE Tag = \""
mySql_select_query = "SELECT * FROM PRODUTOS WHERE Tag = \""
while 1:
    time.sleep(2)
    s = ser.readline()
    if s != b'':
        tag = s.decode()
        if tag[0] == 'I':
            try:
                ins = mySql_insert_query + tag[1:] + "\");"
                result = cursor.execute(ins)
                connection.commit()
                print(tag[1:] + " registrado com sucesso.")
            except mysql.connector.IntegrityError as err:
                print("Erro: Tag " + tag[1:] + " ja esta cadastrada")
        elif tag[0] == 'R':
            rem = mySql_delete_query + tag[1:] + "\""
            result = cursor.execute(rem)
            connection.commit()
            print(tag[1:] + " removido com sucesso.")
        elif tag[0] == 'S':
            sel = mySql_select_query + tag[1:] + "\""
            cursor.execute(sel)
            record = cursor.fetchall()
            if len(record) == 0:
                print("Produto nao cadastrado.")
            else:
                consulta = ""
                for row in record:
                    print("Tag: ", row[0])
                    consulta = consulta + row[0]
                ser.write(consulta.encode())

ser.close()
