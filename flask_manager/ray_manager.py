import psycopg2
import ray

# Подключение к базе данных
def get_db_connection():
    return psycopg2.connect(
        host="database",  # имя контейнера с базой данных
        database="your_db_name",
        user="your_user",
        password="your_password"
    )

# Ray-акторы для управления устройствами
@ray.remote
class DeviceActor:
    def __init__(self, device_id, device_type):
        self.device_id = device_id
        self.device_type = device_type
        self.status = "inactive"

    def activate(self):
        self.status = "active"
        return f"Device {self.device_id} activated."

    def deactivate(self):
        self.status = "inactive"
        return f"Device {self.device_id} deactivated."

    def get_status(self):
        return {"device_id": self.device_id, "device_type": self.device_type, "status": self.status}

# Сохранение устройства в базе данных
def register_device_in_db(device_id, device_type):
    conn = get_db_connection()
    cursor = conn.cursor()
    try:
        cursor.execute("""
            INSERT INTO devices (device_id, device_type, status)
            VALUES (%s, %s, %s)
            ON CONFLICT (device_id) DO NOTHING
        """, (device_id, device_type, 'inactive'))
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e
    finally:
        cursor.close()
        conn.close()

# Регистрация устройства через Ray
def register_device(device_id, device_type):
    try:
        # Сохранение в базу данных
        register_device_in_db(device_id, device_type)

        # Создание Ray-актора для управления устройством
        device_actor = DeviceActor.remote(device_id, device_type)
        return {"status": "success", "message": f"Device {device_id} ({device_type}) registered successfully!"}
    except Exception as e:
        return {"status": "error", "message": str(e)}


