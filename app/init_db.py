import sqlite3

connection = sqlite3.connect('sensors_data.db')

with open('sensors_data.sql') as f:
    connection.executescript(f.read())

connection.commit()
connection.close()
