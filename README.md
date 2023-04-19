# README

# iot_project

Small project for practice the communication each other machine

## Concept and Flow

---

![](https://drive.google.com/uc?export=download&id=12uQFOd1FYK8tCztuKR8pqGL6_J-Mycsi) |![](https://drive.google.com/uc?export=download&id=1cyEyYLu4RqZiRobOs4QvsdbxGzknCNQp)
--- | --- | 

**The hotel system sends the password to the guest through primary security (sending the reservation number via M4) and secondary security (verifying the ID card and password via Arduino).** 

**Passing primary security opens the door to the hotel, and passing secondary security is required to enter the hotel and get and set up their room.**

## Proceed from M4

---


![](https://drive.google.com/uc?export=download&id=11-4P4uDzyemlccDjqBEDPKdk927KukHk) |![](https://drive.google.com/uc?export=download&id=1qEHY1LMjeFOlNp35hEXzUsPmoBwswKWf)
--- | --- | 
![](https://drive.google.com/uc?export=download&id=1TCzFLiNjlJGK7XsKt0nF4azWTWEC_A99) |![](https://drive.google.com/uc?export=download&id=1hVwLmzKvd3epg50nWWUPVeSmK_fhnvPg)


**M4 uses Bluetooth to send the reservation number entered by the reservationist to the server on the Raspberry Pi, and the server sends M4's value to the sql_client. The sql_client checks whether the value is the same as the value received in the pre-made database, and if it is, it sends the name with “correct”, and if it is incorrect, it relays “Decline” to the server and sends it back to M4. M4 parses the value received from sql_client, including 'correct', extracts the name and displays it on the LCD, and if 'Decline' is received, it turns on the LED to warn you.**

## Proceed from Arduino

---

![](https://drive.google.com/uc?export=download&id=1jhsuClXjC6sWjik1NTX4AlFmRNS9ocoU)

![](https://drive.google.com/uc?export=download&id=1xIRDuxth-_AbS_15ibh5KhMsPWRGZx5f) |![](https://drive.google.com/uc?export=download&id=1C4ZyqTwdAxVw2Za8eAPXwDEneLRb8E5B)
--- | --- | 

![](https://drive.google.com/uc?export=download&id=1z5jdZqm9CAQe5zjttUhtgsqzXW8pW7Gw)

![](https://drive.google.com/uc?export=download&id=1QL73Q3Lm0x9Hx8hlJeR8V820bU79TaaC)

![](https://drive.google.com/uc?export=download&id=1jPrg761SweZlDiXhbF0VWRmV4R6aO5Sy)

![](https://drive.google.com/uc?export=download&id=1V3vu3TNIrmc0lcZoT5hgsmN5v9mA8E2p) |![](https://drive.google.com/uc?export=download&id=1ldVTc0trTDdRWSFaa1n76LppjKEwbLlIB)
--- | --- | 
**The Arduino connects to the Raspberry Pi via WIFI. After reading the RFID card and entering the password used to make the reservation, it sends the data to the sql_client through the server, just like the M4. The sql_client sends an Id with "correct" if there is data matching the value of the RFID and password, or "Decline" if there is no data. If the Arduino receives "correct", it will enter the next step of setting the room password, and if it receives "Decline", it will return to the beginning. When the Arduino enters the room password, it will update the room password with the Id received from the sql_client, and the sql_client will compare the old password with the current password after the update, and only send the password to the Android if they are different, passing the secondary security.**

## Proceed from Arduino

---

![](https://drive.google.com/uc?export=download&id=12k9N5fM7yUxGUAtWId3ki5bcIQ-zkfbA)

- **I think I could have done a lot more with the Android add-on to add a database for scheduling.**
- **The Wi-Fi module was relatively slow to detect and pick up a signal, so I was disappointed that I didn't add more features to it because I didn't think it could do more.**
- **I think the check-in system is well made, but considering the linkage with other systems needed in a hotel, I think it would be a better project if it could also do check-out.**
- **When the value is sent directly from the SQL Client to the Arduino, if there is a small time difference, it is not received because the value comes before the Arduino restarts the loop statement, and I gave it a sleep to solve it, but if that happens, the entire code will stop, so I will have to find another way.**
- **We wanted to design more secure security using GPS, but it was difficult due to the unstable GPS module. A more sophisticated GPS module that allows for more sophisticated operations would be more complete.**
# <div align="center"> Thank you </div>