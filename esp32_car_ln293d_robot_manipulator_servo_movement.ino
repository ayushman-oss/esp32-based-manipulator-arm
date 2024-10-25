#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sstream>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <ESP32Servo.h>

#define IN1 26
#define IN2 27
#define IN3 33
#define IN4 32
#define ENA 14
#define ENB 12
#define L1 19


#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0
#define FR 6
#define FL 5
#define RR 8
#define RL 7

const int ServoPin1 = 18;
const int ServoPin2 = 5;
const int ServoPin3 = 17;
const int ServoPin4 = 16;
const int ServoPin10= 4;
const int servoPWMFreq=50;
const int servoPWMRes=10;




const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
int spedB = 0;
int spedA = 0;

const char* ssid     = "Ayken@1044";
const char* password = "Jake@1044";

AsyncWebServer server(80);;
AsyncWebSocket wsCarInput("/CarInput");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(

<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style> h1 {
     font-family: Papyrus, Fantasy;
      }
       .arrows {
           font-size: 10vh;
           color: transparent;
      }
       td.button {
           background-color: #836FFF;
           border-radius: 5%;
      }
       td.button:active {
           transform: scale(1.1, 1.1);
           background-color: #15F5BA;
           box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
      }
       .noselect {
           -webkit-touch-callout: none;
           -webkit-user-select: none;
           -khtml-user-select: none;
           -moz-user-select: none;
           -ms-user-select: none;
           user-select: none;
      }
       .slidecontainer {
           width: 100%;
      }
       .slider {
           -webkit-appearance: none;
           width: 100%;
           height: 5px;
           border-radius: 5px;
           background: #211951;
           outline: none;
           opacity: 0.5;
           -webkit-transition: .2s;
           transition: opacity .2s;
      }
       .slider:hover {
           opacity: 1;
      }
       .slider::-webkit-slider-thumb {
           -webkit-appearance: none;
           appearance: none;
           width: 22px;
           height: 20px;
           border-radius: 20%;
           background: #836FFF;
           cursor: pointer;
      }
       .slider::-moz-range-thumb {
           width: 25px;
           height: 25px;
           border-radius: 5%;
           background: #211951;
           cursor: pointer;
      }
       #info {
           margin-top: 20px;
      }
       #leftRatio, #rightRatio, #direction {
           font-size: 20px;
           margin: 10px;
      }
 </style>
  </head>
  <body class="noselect" align="center" style="background-color:#F6F1E9">
    <table id="mainTable" style="width:75vw;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <td colspan=3>
      <div class="container">
        <input type="range" min="0" max="180" value="90" class="slider" id="servo1" oninput="updateServo1(this.value)">
        <span id="servo1Value">90</span>
      </div></tr><tr><td colspan=3>
      <div class="container">
        <input type="range" min="0" max="180" value="90" class="slider" id="servo2" oninput="updateServo2(this.value)">
        <span id="servo2Value">90</span>
      </div></td></tr><tr><td colspan=3>
      <div class="container">
        <input type="range" min="0" max="180" value="90" class="slider" id="servo3" oninput="updateServo3(this.value)">
        <span id="servo3Value">90</span>
      </div></td></tr><tr><td colspan=3>
      <div class="container">
        <input type="range" min="10" max="60" value="30" class="slider" id="servo4" oninput="updateServo4( this.value)">
        <span id="servo4Value">30</span>
      </div>
    </td>
    </tr>
        
      <tr>
        <td style="color:#7469B6"><b>speed:</b></td>
        <td colspan=2>
          <div class="slidecontainer">
            <input type="range" min="0" max="255" value="0" class="slider" id="Speed" oninput='updateSpeedValue(this.value);'>
            <span id="speedValue">0</span>
          </div>
        </td>
      </tr>
      <tr>
        <td style="color:#7469B6" colspan="3" class="button" id="togglegyro" onclick="togglegyro()">Gyro Disabled</td>
      </tr>
      <tr>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","6")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","6")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8679;</span></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","1")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8679;</span></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","5")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","5")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8679;</span></td>
      </tr>
      <tr>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","3")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8678;</span></td>
        <td class="button" onclick="togglelightUp()"></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","4")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8680;</span></td>
      </tr>
      <tr>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","8")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","8")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8681;</span></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","2")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8679;</span></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","7")' ontouchend='sendButtonInput("MoveCar","0")'onmousedown='sendButtonInput("MoveCar","7")'onmouseup='sendButtonInput("MoveCar","0")'><span class="arrows">&#8679;</span></td>
      </tr>
    </tr>
    <td colspan=3>
      <div class="slidecontainer">
        <label for="time">Time (seconds):</label>
        <input type="range" min="1" max= "60" value="1" class="slider" id="time">
        <span id="timeValue">1</span>
      </div>
    </td>
    <tr>
    <td class="button" id="addRow" style="background-color: #836FFF; cursor: pointer;">Add to Table</td>
    <td class="button" id="startButton" style="background-color: #836FFF; cursor: pointer;">Start</td>
    <td class="button" id="removeLast" style="background-color: #836FFF; cursor: pointer;">Remove Last Entry</td>
    </tr>
    </table>
    <table  style="width:75vw;margin:auto;" id="servoTable">
      <thead>
        <tr>
          <th>Time</th>
          <th>Servo 1</th>
          <th>Servo 2</th>
          <th>Servo 3</th>
          <th>Servo 4</th>
        </tr>
      </thead>
      <tbody></tbody>
    </table>
    <script>
    
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";
      var websocketCarInput;
      function updateSpeedValue(val) {
        document.getElementById("speedValue").textContent = val;
        sendButtonInput("Speed",val);
      }
      function updateServo1(value) {
        document.getElementById("servo1Value").textContent =value;
        sendButtonInput("Servo1", value);
      }
      function updateServo2(value) {
        document.getElementById("servo2Value").textContent =value;
        sendButtonInput("Servo2", value);
      }
      function updateServo3(value) {
        document.getElementById("servo3Value").textContent =value;
        sendButtonInput("Servo3", value);
      }
      function updateServo4(value) {
        document.getElementById("servo4Value").textContent =value;
        sendButtonInput("Servo4", value);
      }
       document.addEventListener('keydown', function(event) {
    switch(event.key) {
      case 'ArrowUp':
        sendButtonInput("MoveCar", 1);  // Up key sends 1
        break;
      case 'ArrowDown':
        sendButtonInput("MoveCar", 2);  // Down key sends 2
        break;
      case 'ArrowLeft':
        sendButtonInput("MoveCar", 3);  // Left key sends 3
        break;
      case 'ArrowRight':
        sendButtonInput("MoveCar", 4);  // Right key sends 4
        break;
    }
  });

  document.addEventListener('keyup', function(event) {
    // Stop moving when the key is released
    switch(event.key) {
      case 'ArrowUp':
      case 'ArrowDown':
      case 'ArrowLeft':
      case 'ArrowRight':
        sendButtonInput("MoveCar", 0);  // Stop when any arrow key is released
        break;
    }
  });
      


      function initCarInputWebSocket() {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onopen = function(event) {
          var speedButton = document.getElementById("Speed");
          sendButtonInput("Speed", speedButton.value);
          var servo1button = document.getElementById("servo1");
          sendButtonInput("Servo1", 90);
          var servo1button = document.getElementById("servo2");
          sendButtonInput("Servo2", 90);
          var servo1button = document.getElementById("servo3");
          sendButtonInput("Servo3", servo1Value.value);
          var servo1button = document.getElementById("servo4");
          sendButtonInput("Servo4", servo1Value.value);

        };
        websocketCarInput.onclose = function(event) { setTimeout(initCarInputWebSocket, 2000); };
        websocketCarInput.onmessage = function(event) {};
      }

      function initWebSocket() {
        initCarInputWebSocket();
      }

      function sendButtonInput(key, value) {
        var data = key + "," + value;
        websocketCarInput.send(data);
      }
      var lightup=false;
      function togglelightUp(){
        lightup =!lightup;
        if(lightup){
        sendButtonInput("light","1");
        }
        else{
          sendButtonInput("light","0");}
      }
      
      
      var gyro = false; 
      

        function togglegyro() {
            gyro = !gyro;
            document.getElementById("togglegyro").innerText = gyro ? "Gyro Enabled"  : "Gyro Disabled";
            
            if (!gyro) {
            sendButtonInput("MoveCar", "0"); 
            document.getElementById("direction").innerText = "Direction: None";
               }
            window.addEventListener('deviceorientation',
      function(event) {
        console.log("handle orientation");
        if(!gyro){
            return;
        }

        document.getElementById('beta').textContent = (event.beta || 0).toFixed(2);
        document.getElementById('gamma').textContent = (event.gamma || 0).toFixed(2);
      
    
        const beta = (event.beta || 0).toFixed(2);  // Tilt front-to-back in degrees
        const gamma = (event.gamma || 0).toFixed(2); // Tilt left-to-right in degrees

     
        const threshold = 20;  // Threshold for detecting movement
        
        let direction = 'center';
        
        
        if (Math.abs(beta) > threshold || Math.abs(gamma) > threshold) {
          // Compare the absolute values of beta and gamma
          if (Math.abs(beta) > Math.abs(gamma)) {
              // If beta is larger in magnitude, check if it's forward or backward
              direction = beta > threshold ? 'backward' : 'forward';
              document.getElementById("direction").innerText = direction;
              sendButtonInput("MoveCar", beta > threshold ? "2" : "1");
          } else {
              // If gamma is larger in magnitude, check if it's left or right
              direction = gamma > threshold ? 'right' : 'left';
              document.getElementById("direction").innerText = direction;
              sendButtonInput("MoveCar", gamma > threshold ? "4" : "3");
          }
          return;
        } else {
          // If neither beta nor gamma exceeds the threshold, set the direction to center
          direction = 'center';
          document.getElementById("direction").innerText = direction; // Exit the loop after deciding the direction
          
      }
  });
           }
          const servos = [1, 2, 3, 4];
          const timeEntries = [];
          const timeSlider = document.getElementById('time');
          const timeOutput = document.getElementById('timeValue');
          timeSlider.addEventListener('input', () => {
            timeOutput.textContent = timeSlider.value;
          });
          document.getElementById('addRow').addEventListener('click', () => {
            const currentTime = parseInt(timeSlider.value);
            const tableBody = document.querySelector('#servoTable tbody');
          
            // Check for duplicates
            if (timeEntries.includes(currentTime)) {
              alert('Time entry already exists.');
              return;
            }
          
            // Check if the time is in ascending order
            if (timeEntries.length > 0 && currentTime <= Math.max(...timeEntries)) {
              alert('Time value must be in ascending order.');
              return;
            }
          
            // Add the new time to the array
            timeEntries.push(currentTime);
          
            // Add the new row to the table
            const newRow = document.createElement('tr');
            newRow.innerHTML = `
              <td>${currentTime}</td>
              ${servos.map(i => `<td>${document.getElementById(`servo${i}`).value}</td>`).join('')}
            `;
            tableBody.appendChild(newRow);
          });
          document.getElementById('removeLast').addEventListener('click', () => {
            const tableBody = document.querySelector('#servoTable tbody');
            if (tableBody.lastChild) {
              const lastRowTime = parseInt(tableBody.lastChild.firstChild.textContent);
              timeEntries.pop();
              tableBody.removeChild(tableBody.lastChild);
            }
          });
          let isRunning = false;
          document.getElementById('startButton').addEventListener('click', function() {
            let startButton = this;
          
            console.log("start action in play");
          
            const servoValues = [];
            const table = document.getElementById("servoTable");
            const rows = table.rows.length;
          
            // Loop through each row, skipping the header
            for (let i = 1; i < rows; i++) {  // Adjusted to start from row 1 (skipping header)
              let time = parseInt(table.rows[i].cells[0].innerText);
              let servo1 = parseInt(table.rows[i].cells[1].innerText);
              let servo2 = parseInt(table.rows[i].cells[2].innerText);
              let servo3 = parseInt(table.rows[i].cells[3].innerText);
              let servo4 = parseInt(table.rows[i].cells[4].innerText);
          
              servoValues.push({ time, servo1, servo2, servo3, servo4 });
            }
          
            // Check if servoValues array is empty
            if (servoValues.length === 0) {
              alert("Please add values to the table first.");
              return;  // Exit the function if no values are present
            }
          
            // Toggle isRunning state and button text
            if (isRunning) {
              isRunning = false;
              startButton.innerText = "Start";
              console.log("Stopping the servo updates.");
              return;  // Exit if we're stopping
            } else {
              isRunning = true;
              startButton.innerText = "Stop";
              console.log("Starting the servo updates.");
            }
          
            let allServoValues = [];
          
            for (let i = 0; i < servoValues.length - 1; i++) {
              let current = servoValues[i];
              let next = servoValues[i + 1];
              let timeDiff = next.time - current.time;
          
              // Calculate changes per second for each servo
              let servo1ChangePerSec = (next.servo1 - current.servo1) / timeDiff;
              let servo2ChangePerSec = (next.servo2 - current.servo2) / timeDiff;
              let servo3ChangePerSec = (next.servo3 - current.servo3) / timeDiff;
              let servo4ChangePerSec = (next.servo4 - current.servo4) / timeDiff;
          
              for (let t = 0; t < timeDiff; t++) {
                let newTime = current.time + t + 1; // Increment time by seconds
                let newServo1Value = current.servo1 + Math.floor(servo1ChangePerSec * t);
                let newServo2Value = current.servo2 + Math.floor(servo2ChangePerSec * t);
                let newServo3Value = current.servo3 + Math.floor(servo3ChangePerSec * t);
                let newServo4Value = current.servo4 + Math.floor(servo4ChangePerSec * t);
          
                allServoValues.push({
                  time: newTime,
                  servo1: newServo1Value,
                  servo2: newServo2Value,
                  servo3: newServo3Value,
                  servo4: newServo4Value
                });
              }
            }
          
            // Log the allServoValues array
            console.log(allServoValues);
            function updateServoValues(index) {
              if (!isRunning || index >= allServoValues.length) {
                // Exit if stopped or finished updating
                console.log("Servo updates stopped.");
                alert("servo movement complete");
                startButton.innerText = "Start"; // Reset button text
                isRunning = false;
                return;
              }
          
              let current = allServoValues[index];
              updateServo1(current.servo1); // Call the update function for servo1
              updateServo2(current.servo2); // Call the update function for servo2
              updateServo3(current.servo3); // Call the update function for servo3
              updateServo4(current.servo4); // Call the update function for servo4
              console.log("Updated Servos to:", current);
              timeSlider.value=current.time;
              timeOutput.textContent = timeSlider.value;
          
          
              // Call this function every second
              setTimeout(() => {
                updateServoValues(index + 1);
              }, 1000);
            }
          
            // Start the updates
            updateServoValues(0);
          });
          
        window.onload = function() {
        
        initWebSocket();
        
      };

    </script>
  </body>
</html>
)HTMLHOMEPAGE";
void moveCar(int inputValue)
{
  Serial.printf("Got value as %d\n", inputValue);  
  switch(inputValue)
  {

    case UP:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB);
          digitalWrite(L1,LOW);
          
       break;
       case LEFT: 
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          ledcWrite(ENA,255);
          ledcWrite(ENB,255);
          digitalWrite(L1,LOW);
      break;
    case FR:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          ledcWrite(ENA,spedA*0.5);
          ledcWrite(ENB,spedB);
          digitalWrite(L1,LOW);
       break;
    case FL:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB*0.5);
          digitalWrite(L1,LOW);
       break;
    case DOWN:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB); 
          digitalWrite(L1,LOW);
      break;
    case RR:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB*0.5);
          digitalWrite(L1,LOW);
      break;
    case RL:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          ledcWrite(ENA,0.5*spedA);
          ledcWrite(ENB,spedB);
          digitalWrite(L1,LOW);
          
      break;
  
 
    case RIGHT:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB);
          digitalWrite(L1,LOW);
      break;
 
    case STOP:
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          
          delay(10);
          
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);

          delay(10);
    
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, LOW);
          
          ledcWrite(ENA,0);
          ledcWrite(ENB,0);
          digitalWrite(L1,HIGH);
          break;
  
    default:
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, LOW);
          digitalWrite(L1,HIGH);
          ledcWrite(ENA,0);
          ledcWrite(ENB,0);
 
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      moveCar(0);  
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        int valueInt = atoi(value.c_str()); 
            
        if (key == "MoveCar")
        {
          moveCar(valueInt);        
        }
        else if (key == "Speed")
        {
          spedA=valueInt;
          spedB=valueInt;
          ledcWrite(ENA,spedA);
          ledcWrite(ENB,spedB);
        }
        else if (key == "Servo1")
        { 
          ledcWrite(ServoPin1,map(valueInt,0,180,26,128)); 
          ledcWrite(ServoPin10,map(180-valueInt,0,180,26,128)); 
        }    
        else if (key == "Servo2")
        {
          ledcWrite(ServoPin2,map(valueInt,0,180,26,128));         
        }  
        else if (key == "Servo3")
        {
           ledcWrite(ServoPin3,map(valueInt,0,180,26,128));        
        }  
        else if (key == "Servo4")
        {
          ledcWrite(ServoPin4,map(valueInt,0,180,26,128));         
        }  
        else if (key == "light"){
          if(valueInt==1){
            digitalWrite(15,HIGH);
            Serial.printf("turned light on ");
            }
            else{
              digitalWrite(15,LOW);
              Serial.printf("turned light of ");
            }
          
          Serial.print(valueInt);
          }
      }
      break;
    case WS_EVT_PONG:
      moveCar(0);
    case WS_EVT_ERROR:
      moveCar(0);
      break;
    default:
      moveCar(0);
      break;  
  }
}



void setUpPinModes()
{
  
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(L1,OUTPUT);
    pinMode(15,OUTPUT);
    pinMode(ServoPin1,OUTPUT);
    pinMode(ServoPin2,OUTPUT);
    pinMode(ServoPin3,OUTPUT);
    pinMode(ServoPin4,OUTPUT);
    pinMode(ServoPin10,OUTPUT);

  ledcAttach(ENA , PWMFreq, PWMResolution);
  ledcAttach(ENB , PWMFreq, PWMResolution);
  ledcAttach(ServoPin1,servoPWMFreq,servoPWMRes);
  ledcAttach(ServoPin2,servoPWMFreq,servoPWMRes);
  ledcAttach(ServoPin3,servoPWMFreq,servoPWMRes);
  ledcAttach(ServoPin4,servoPWMFreq,servoPWMRes);
  ledcAttach(ServoPin10,servoPWMFreq,servoPWMRes);

  digitalWrite(L1,HIGH);
  digitalWrite(15,LOW);
  moveCar(STOP);
  
}


void setup(void) 
{
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");
}


void loop() 
{
  wsCarInput.cleanupClients(); 
  
  //Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}
