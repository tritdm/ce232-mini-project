import sqlite3

connection = sqlite3.connect('sensors_data.db')


with open('sensors_data.sql') as f:
    connection.executescript(f.read())

cur = connection.cursor()

cur.execute("INSERT INTO sensors_data (topic, data) VALUES (?, ?)",
            ('Humi', '50.00')
            )

cur.execute("INSERT INTO sensors_data (topic, data) VALUES (?, ?)",
            ('Temp', '37.00')
            )

connection.commit()
connection.close()
