import asyncio
import websockets
import pygame
import json

# Setup Pygame
pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("No joystick connected!")
    exit()

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"Initialized Joystick: {joystick.get_name()}")

def scale(value, src_min, src_max, dst_min, dst_max):
    src_range = src_max - src_min
    dst_range = dst_max - dst_min
    value_scaled = (value - src_min) / src_range
    return dst_min + (value_scaled * dst_range)

def clamp(value, min_value, max_value):
    return max(min(value, max_value), min_value)

def apply_deadzone(value, threshold=0.1):
    """Ignore small joystick movements."""
    if abs(value) < threshold:
        return 0.0
    return value

def cubic_scaling(value, gain=1.0):
    """Apply cubic scaling for smoother control."""
    return gain * (value * abs(value))

async def send_data(websocket):
    print("Client connected!")
    try:
        while True:
            pygame.event.pump()

            axis_0 = joystick.get_axis(0)  # left stick X (turn)
            axis_1 = joystick.get_axis(1)  # left stick Y (throttle)

            # Invert Y axis (common for forward stick)
            throttle = -axis_1
            turn = axis_0

            # Apply deadzone
            throttle = apply_deadzone(throttle)
            turn = apply_deadzone(turn)

            # Apply cubic scaling
            throttle = cubic_scaling(throttle, gain=1.0)
            turn = cubic_scaling(turn, gain=0.8)  # you can use less turn if you want softer turning

            # Calculate motor outputs
            left_motor = throttle + turn
            right_motor = throttle - turn

            # Normalize motor outputs
            max_mag = max(abs(left_motor), abs(right_motor), 1.0)
            left_motor /= max_mag
            right_motor /= max_mag

            # Scale to -255..255
            left_motor_pwm = int(scale(left_motor, -1, 1, -255, 255))
            right_motor_pwm = int(scale(right_motor, -1, 1, -255, 255))

            # Clamp final outputs
            left_motor_pwm = clamp(left_motor_pwm, -255, 255)
            right_motor_pwm = clamp(right_motor_pwm, -255, 255)

            # Prepare and send message


            await websocket.send(f"{left_motor_pwm},{right_motor_pwm}")

            await asyncio.sleep(0.05)  # 50ms delay (20 Hz)
    except websockets.ConnectionClosed:
        print("Client disconnected!")

async def main():
    async with websockets.serve(send_data, "0.0.0.0", 8765):
        await asyncio.Future()

asyncio.run(main())
