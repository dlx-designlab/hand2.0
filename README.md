# Hands 2.0 README

Treasure hunting project 2025 in collaboration with Matsuhisa Lab aiming to develop an "invisible" wearable device for thumb wrestling.

_Status: Ongoing

_Last updated: August 2025 documented by Tamaki

## 📁 Software

Each script runs individually

- `Arduino_thumbwrestling.ino`  
   Arduino sketch for the **first prototype** of a judge system, using a single Arduino as the main controller.

   Video on https://drive.google.com/file/d/1VUcGh0xSxG4f2ZdRcVpi-l9c60P2i8LT/view?usp=drive_link

   _Note: We have since evolved the concept into a system with two stand-alone devices._

- `M5_capacitance.ino`  
   Script to measure the capacitance of Matsuhisa Lab’s original pressure sensor.

- `M5_thumbwrestling_com1.ino`  
   Script for thumb wrestling judge system with 3 data (touch, handshake, pressure value) displayed on M5.

   Video on https://drive.google.com/file/d/1QNyZTJC7033aZxt6TYxbK8tVRA3WX5ky/view?usp=drive_link

- `M5_thumbwrestling_com2.ino`

   A step up from `M5_thumbwrestling_com1.ino`.
   It includes dummy opponent data and makes judgments based on information from both players.
   Replace the dummy values with the communicated ones.

- `M5_thumbwrestling_onejudge.ino`

   A step behind from `M5_thumbwrestling_com1.ino`.
   Only the loser judges the game when he is pressed for 5 seconds.
  

<br><br>

## ⚙️ Hardware

- Arduino UNO for `Arduino_thumbwrestling.ino`  
- M5 Atom S3 for `M5_capacitance.ino`, `M5_thumbwrestling_com1.ino`, `M5_thumbwrestling_com2.ino`, and `M5_thumbwrestling_onejudge.ino`.


<br><br>

## 🛠️ Wiring and Behaviour
<br>

### Arduino_thumbwrestling.ino

![Arduino_wiring](images/Arduino_thumbwresting.jpg)

![Arduino_pic](images/Arduino_pic.png)

<br>

**How it works**
1. The game starts when both players join hands — detected by changes in the capacitance of wrist-mounted touch sensors.  
   If either player lets go during the game, the system resets.

2. The display shows the states of the game.

3. Either of the players wins when he presses the opponent's thumb for 5 seconds.

<br>

**Threshold Configuration**

These parameters are defined at the beginning of the script:

- Wrist touch sensor threshold  
- Pressure sensor threshold  
- Thumb touch sensor threshold  
- Required time to register a win: **5 seconds**

<br>

### M5_capacitance.ino

![cap_wiring](images/M5_capacitance_wire.jpg)
![cap_draw](images/M5_capacitance_draw.png)
![cap_pic](images/M5_capacitance_pic.png)

<br>

**How to select the resistor value**

The resistor between the capacitance sensor and M5 influences the sensitivity and the response delay.
For the first-gen capacitive sensor, use **68k Ω** or **51k Ω**.
The 68k has more delay but higher sensitivity. The 51k has less delay and faster response, but lower sensitivity. Sometimes it won't react even when pressure is applied.

<br>

**How to measure the capacitance with a tester**
![tester](images/tester.png)

If your digital multimeter has the marking ‘Cx’ around its terminals, insert the leads into those terminals to measure capacitance.

<br>

### M5_thumbwrestling_com1.ino

![wired_wiring](images/M5_fsr_wire.jpg)
![drawn_wiring](images/M5_fsr_draw.jpg)

<br>

**Operation**
1. The game starts when both players join hands — detected by changes in the capacitance of wrist-mounted touch sensors.  
   If either player lets go during the game, the system resets.
   
2. The display shows the states of the game and these 3 data: thumb touch(0/1), handshake(0,1), and pressure sensor value.

3. If your thumb touch is true and the pressure value stays above the threshold for 5 seconds, the system will judge it as a loss.

<br>

### M5_thumbwrestling_com2.ino


The wiring is the same as `M5_thumbwrestling_com2.ino`
<br>
**Operation**
1. The game starts when both players join hands — detected by changes in the capacitance of wrist-mounted touch sensors.  
   If either player lets go during the game, the system resets.
   
2. The display shows the states of the game and these 3 data: thumb touch(0/1), handshake(0,1), and pressure sensor value.

3. The system judges based on its own 3 data and the opponent's dummy data

<br>

### M5_thumbwrestling_onejudge.ino

<br>
The wiring is the same as `M5_thumbwrestling_com2.ino`

**Operation**
Almost the same as `M5_thumbwrestling_com2.ino`.
This doesn't display the data.
