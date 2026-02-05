import serial


def run_monitor_no_reset(port="COM3", baud=9600) -> None:
    # Initialize serial connection
    ser = serial.Serial()
    ser.port = port
    ser.baudrate = baud

    # THE SECRET SAUCE: Disable DTR/RTS to prevent reset
    ser.dtr = False
    ser.rts = False

    try:
        ser.open()
        print(f"--- Monitoring {port} ---")
        print("--- Press ctrl+C to exit ---\n")
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode("utf-8", errors="ignore")
                print(line, end="")
    except KeyboardInterrupt:
        ser.close()
        print("\n--- Disconnected ---")


run_monitor_no_reset()
