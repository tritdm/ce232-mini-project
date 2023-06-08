import sqlite3

connection = sqlite3.connect('sensors_data.db')


with open('sensors_data.sql') as f:
    connection.executescript(f.read())

cur = connection.cursor()

cur.execute("INSERT INTO humi_data (data) VALUES (?)",
            ('50.00',)
            )

cur.execute("INSERT INTO temp_data (data) VALUES (?)",
            ('37.00',)
            )

connection.commit()
connection.close()
