from flask import Flask, request, jsonify
import docker
import re
import unidecode
import os
import shutil
import logging
import ray

# Настройка логирования
logging.basicConfig(level=logging.DEBUG)

# Инициализация Flask приложения и клиента Docker
app = Flask(__name__)
client = docker.from_env()

# Инициализация Ray
ray.init(ignore_reinit_error=True)


# Функция для очистки имени
def sanitize_name(name):
    sanitized = unidecode.unidecode(name)
    sanitized = re.sub(r'[^a-zA-Z0-9_-]', '_', sanitized)
    return sanitized.lower()


# Эндпоинт для регистрации устройства
@app.route('/register_device', methods=['POST'])
def register_device():
    data = request.json
    device_name = data.get('device_name')
    device_type = data.get('device_type')
    room_name = data.get('room_name')

    if not device_name or not room_name:
        logging.error("Missing required parameters: device_name or room_name")
        return jsonify({"error": "Device name and room name are required"}), 400

    sanitized_device_name = sanitize_name(device_name)
    sanitized_room_name = sanitize_name(room_name)
    container_name = f"device_{sanitized_room_name}_{sanitized_device_name}"

    build_dir = os.path.join('/tmp', container_name)
    logging.debug(f"Build directory: {build_dir}")

    try:
        os.makedirs(build_dir, exist_ok=True)

        # Создание Dockerfile для устройства
        dockerfile_content = f"""
        FROM python:3.9-slim
        COPY . /app
        WORKDIR /app
        RUN pip install flask
        CMD ["python", "device_simulator.py"]
        """
        dockerfile_path = os.path.join(build_dir, 'Dockerfile')
        with open(dockerfile_path, 'w') as dockerfile:
            dockerfile.write(dockerfile_content)
        logging.debug(f"Dockerfile created at {dockerfile_path}")

        # Создание скрипта симулятора устройства
        device_simulator = f"""
from flask import Flask, jsonify

app = Flask(__name__)

@app.route('/status', methods=['GET'])
def status():
    return jsonify({{
        "device_name": "{device_name}",
        "device_type": "{device_type}",
        "room_name": "{room_name}",
        "status": "active"
    }})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
        """
        simulator_path = os.path.join(build_dir, 'device_simulator.py')
        with open(simulator_path, 'w') as simulator_file:
            simulator_file.write(device_simulator)
        logging.debug(f"Device simulator script created at {simulator_path}")

        # Сборка образа
        logging.info(f"Building Docker image: {container_name}")
        image, build_logs = client.images.build(path=build_dir, tag=f"{container_name}:latest")
        for log in build_logs:
            logging.debug(log)

        # Проверка наличия сети
        network_name = "smarthousesystem_app-network"
        networks = [n.name for n in client.networks.list()]
        logging.debug(f"Available networks: {networks}")
        if network_name not in networks:
            logging.error(f"Network {network_name} does not exist")
            raise ValueError(f"Network {network_name} does not exist")

        # Создание контейнера
        logging.info(f"Creating container: {container_name}")
        container = client.containers.create(
            image=container_name,
            name=container_name,
            detach=True,
            environment={
                "DEVICE_NAME": device_name,
                "DEVICE_TYPE": device_type,
                "ROOM": room_name
            },
            network=network_name
        )
        logging.debug(f"Container created: {container}")

        return jsonify({"message": f"Image and container for {container_name} created successfully"}), 201
    except Exception as e:
        logging.exception("An error occurred while creating the image and container")
        return jsonify({"error": str(e)}), 500
    finally:
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
            logging.debug(f"Temporary build directory {build_dir} removed")


# Эндпоинт для управления устройством (стартап/стоп)
@app.route('/toggle_device', methods=['POST'])
def toggle_device():
    data = request.json
    device_name = data.get('device_name')
    room = data.get('room_name')
    action = data.get('action')  # "start" или "stop"

    if not device_name or not room or action not in ["start", "stop"]:
        return jsonify({'error': 'Invalid parameters'}), 400

    sanitized_name = f"device_{sanitize_name(room)}_{sanitize_name(device_name)}"

    try:
        app.logger.debug(f"Looking for container: {sanitized_name}")
        container = client.containers.get(sanitized_name)

        if action == "start":
            if container.status != "running":
                container.start()
                return jsonify({'message': f'Device {device_name} started successfully', 'success': 'true'}), 200
            else:
                return jsonify({'error': f'Device {device_name} is already running', 'success': 'false'}), 400
        elif action == "stop":
            if container.status == "running":
                container.stop()
                return jsonify({'message': f'Device {device_name} stopped successfully', 'success': 'true'}), 200
            else:
                return jsonify({'error': f'Device {device_name} is not running', 'success': 'false'}), 400
    except docker.errors.NotFound:
        app.logger.error(f"Container not found: {sanitized_name}")
        return jsonify({'error': f'Container for {device_name} not found'}), 404
    except Exception as e:
        app.logger.error(f"Unexpected error: {str(e)}")
        return jsonify({'error': str(e)}), 500


# Эндпоинт для получения статуса устройства
@app.route('/device_status', methods=['GET'])
def device_status():
    device_name = request.args.get('device_name')
    room_name = request.args.get('room_name')  # Учитываем комнату

    if not device_name or not room_name:
        return jsonify({'error': 'Device name and room name are required'}), 400

    sanitized_device_name = sanitize_name(device_name)
    sanitized_room_name = sanitize_name(room_name)

    container_name = f"device_{sanitized_room_name}_{sanitized_device_name}"
    try:
        container = client.containers.get(container_name)
        return jsonify({
            "device_name": device_name,
            "room_name": room_name,
            "status": container.status
        }), 200
    except docker.errors.NotFound:
        return jsonify({'error': f'Container for {device_name} in room {room_name} not found'}), 404
    except Exception as e:
        return jsonify({'error': str(e)}), 500


# Эндпоинт для взаимодействия с Ray (если нужно)
@app.route('/create_ray_actor', methods=['POST'])
def create_ray_actor():
    data = request.json
    device_name = data.get('device_name')
    device_type = data.get('device_type')

    if not device_name or not device_type:
        return jsonify({'error': 'Device name and type are required'}), 400

    # Создаем Ray-актора для устройства
    @ray.remote
    class DeviceActor:
        def __init__(self, device_name, device_type):
            self.device_name = device_name
            self.device_type = device_type

        def get_device_status(self):
            return {"device_name": self.device_name, "device_type": self.device_type, "status": "active"}

    device_actor = DeviceActor.remote(device_name, device_type)
    device_status = ray.get(device_actor.get_device_status.remote())

    return jsonify({"device_status": device_status}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
