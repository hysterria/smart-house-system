CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username TEXT UNIQUE,
    password TEXT,
    role TEXT
);

CREATE TABLE IF NOT EXISTS rooms (
    id SERIAL PRIMARY KEY,
    name TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS device_types (
    id SERIAL PRIMARY KEY,
    type TEXT UNIQUE,
    parameters TEXT
);

CREATE TABLE IF NOT EXISTS devices (
    id SERIAL PRIMARY KEY,
    room_id INTEGER,
    device_type_id INTEGER,
    device_group TEXT,
    name TEXT,
    status TEXT DEFAULT 'off',
    last_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (room_id) REFERENCES rooms(id) ON DELETE CASCADE,
    FOREIGN KEY (device_type_id) REFERENCES device_types(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS sensors (
    id SERIAL PRIMARY KEY,
    room_id INTEGER,
    type TEXT,
    status TEXT DEFAULT 'active',
    last_signal TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (room_id) REFERENCES rooms(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS sensor_events (
    id SERIAL PRIMARY KEY,
    sensor_id INTEGER,
    event_type TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (sensor_id) REFERENCES sensors(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS scenarios (
    id SERIAL PRIMARY KEY,
    name TEXT UNIQUE,
    trigger_event TEXT
);

CREATE TABLE IF NOT EXISTS scenario_actions (
    id SERIAL PRIMARY KEY,
    scenario_id INTEGER,
    device_id INTEGER,
    action TEXT,
    parameters TEXT, -- JSON
    FOREIGN KEY (scenario_id) REFERENCES scenarios(id) ON DELETE CASCADE,
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);