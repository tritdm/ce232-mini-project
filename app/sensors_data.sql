DROP TABLE IF EXISTS temp_data;
DROP TABLE IF EXISTS humi_data;

PRAGMA temp_store = 2;

CREATE TABLE temp_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    created TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    data TEXT NOT NULL
);

CREATE TABLE humi_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    created TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    data TEXT NOT NULL
);

INSERT INTO humi_data (data) VALUES (80.00);

INSERT INTO temp_data (data) VALUES (30.00);