# esp32-based-manipulator-arm

the project is based on esp32 wroom devkit i am using  a l289n motor driver along with 5 servos that have a rotation of 0-180 each 
thsi project combines the movement of a car to that of a manipukator arm i used the web server protocol for creating  a soft ap which is basically like craeting your own wifi access point after that there is the html inside the code that handles all the client side interactions on the web page then use send packets of data which tell the car what to do like to move forward or move the servos 
included is a feature the lets the user set the particulat motion that needs to be followed by the servos one can set it using the provided html page the rest of  calculations are handled by the js and the  command is sent to the server that is the esp 
the next step is to implement inverse kinematics into the servo movements for handling the hardare side of things 
servos require a lot of current for that i am using 3 ldo in parallel to provide for the current needs there is 3s 12.4 v battery pack powering the motor driver and there is also a 5 v supply for the eps 
further 
the movement can be improved if using better servos and motors
