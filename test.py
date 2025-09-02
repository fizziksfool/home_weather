import subprocess
from pathlib import Path

# --- CONFIG ---
ino_folder = Path(
    r"C:\Users\erbej\Desktop\Python Projects\home_weather\BME680_dataLogger"
)  # Folder containing your .ino file
board_fqbn = "arduino:avr:uno"  # Uno FQBN
port = "COM3"  # Replace with the port your Uno is on


# --- FUNCTION TO FIND MAIN .ino FILE ---
def find_main_ino(folder: Path) -> Path:
    """Return the main .ino file in the folder. Raises an error if none found."""
    ino_files = list(folder.glob("*.ino"))
    if not ino_files:
        raise FileNotFoundError(f"No .ino files found in folder {folder}")
    if len(ino_files) > 1:
        print(
            f"Warning: Multiple .ino files found in {folder}. Using {ino_files[0].name}"
        )
    return ino_files[0]


main_ino = find_main_ino(ino_folder)

# Arduino CLI requires the folder name to match the main .ino file name.
# So we temporarily rename the folder if needed.
if ino_folder.name != main_ino.stem:
    temp_folder = ino_folder.parent / main_ino.stem
    if temp_folder.exists():
        raise FileExistsError(
            f"Cannot rename folder to {temp_folder}. Folder already exists."
        )
    ino_folder.rename(temp_folder)
    ino_folder = temp_folder


# --- FUNCTION TO RUN COMMANDS ---
def run_cmd(cmd_list):
    """Run a subprocess command and print output."""
    print("Running:", " ".join(cmd_list))
    result = subprocess.run(cmd_list, capture_output=True, text=True)
    print(result.stdout)
    if result.returncode != 0:
        print(result.stderr)
        raise RuntimeError(f"Command failed: {' '.join(cmd_list)}")


# --- COMPILE ---
run_cmd(["arduino-cli", "compile", "--fqbn", board_fqbn, str(ino_folder)])

# --- UPLOAD ---
run_cmd(["arduino-cli", "upload", "-p", port, "--fqbn", board_fqbn, str(ino_folder)])

print("Upload complete!")
