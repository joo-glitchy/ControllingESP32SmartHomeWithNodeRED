# ControllingESP32SmartHomeWithNodeRED
# Smart House Prototype – MQTT & Node-RED Testing

## About the Project

This project was developed as a preliminary prototype for my main Smart House project. The purpose was to experiment with the individual components of the smart house and test the communication between the hardware, MQTT, and Node-RED before moving on to the final implementation.

Using an ESP32 and components from the Keyestudio Smart Home Kit, I tested how different parts of the house could be controlled through messages sent using MQTT and managed through a Node-RED flow.

## Objectives

- Test the individual smart-house components, including the light, fan, door, window, and buzzer.
- Establish communication between the ESP32 and an MQTT broker.
- Test MQTT message publishing and subscription.
- Connect Node-RED to MQTT to send commands to the smart-house components.
- Verify that commands sent through the Node-RED interface were received and executed by the hardware.
- Identify potential issues with the hardware and communication setup before developing the main project.

## Technologies Used

- **ESP32** – Microcontroller used to control the hardware.
- **Arduino IDE** – Used to develop and upload the microcontroller code.
- **MQTT** – Messaging protocol used for communication between Node-RED and the ESP32.
- **Node-RED** – Used to create a flow-based interface for sending commands to the smart-house components.
- **Keyestudio Smart Home Kit** – Provided the components used for testing.

## Features Tested

- **Light control:** Switching the light ON and OFF.
- **Fan control:** Switching the fan ON and OFF using its motor driver.
- **Window control:** Opening and closing the window mechanism.
- **Door control:** Opening and closing the door mechanism.
- **Buzzer control:** Activating and deactivating the buzzer.
- **MQTT communication:** Receiving commands through subscribed MQTT topics.
- **Node-RED integration:** Using dashboard-style input nodes to send commands to the hardware.

## How It Works

Node-RED sends commands to an MQTT broker using specific topics for each component. The ESP32 subscribes to these topics and listens for incoming messages. When a command is received, the ESP32 processes the message and controls the corresponding component.

The MQTT topics used in the prototype are:

| Component | MQTT Topic | Commands |
|---|---|---|
| Light | `house/light` | `ON`, `OFF` |
| Fan | `house/fan` | `ON`, `OFF` |
| Window | `house/window` | `OPEN`, `CLOSE` |
| Door | `house/door` | `OPEN`, `CLOSE` |
| Buzzer | `house/buzzer` | `ON`, `OFF` |

## Purpose of This Prototype

This repository represents an initial testing stage rather than the final Smart House project. It helped me explore the individual hardware components, understand MQTT communication, and test the connection between Node-RED and the physical house.

The experience gained from this prototype provided a foundation for the development of the main project, where these components and communication methods could be integrated into a more complete system.

## Project Status

**Status:** Preliminary prototype / testing phase.

This project focuses on experimentation, component testing, and communication between the hardware, MQTT, and Node-RED. It is separate from the final implementation of the Smart House project.
