import numpy as np
from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


DEVICE_FILE = "/dev/SdeC_drv4"

def write(channel):
    
    channel = 0
    
    file = open(DEVICE_FILE, "w")
    file.write("0")
    file.close()


def read_sensor():
    #cat
    file = open(DEVICE_FILE,"r")
    value = float(file.read()) 
    file.close()

    return value

fig, (ax1) = plt.subplots(1,1)
x_data, y1_data  =[], []
time = 0
line, = ax1.plot([], [], lw=7, color ='r') 
def graficar():
    time +=3
    valor1 = read_sensor()
    x_data.append(time)
    y1_data.append(valor1)
    print("Valor: ",valor1)        
         
    line.set_data(x_data, y1_data)  

  
def animate(i):
    data = pd.read_csv()