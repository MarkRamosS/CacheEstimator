import subprocess
import os

subprocess.run("ls")
for file in os.listdir('dataset/input/'):
    print(file)
