#include <WiFi.h>
#include <WebServer.h>

// =========================
// WIFI
// =========================

const char *ssid = "SENTRO_BOT";
const char *password = "HMSE2026";

WebServer server(80);

// =========================
// ULTRASONIC
// =========================

#define FRONT_TRIG 5
#define FRONT_ECHO 18

#define LEFT_TRIG 19
#define LEFT_ECHO 21

#define RIGHT_TRIG 22
#define RIGHT_ECHO 23

// =========================
// MOTOR
// =========================

// Motor kiri
#define LEFT_IN1 25
#define LEFT_IN2 26
#define LEFT_EN 33

// Motor kanan
#define RIGHT_IN1 27
#define RIGHT_IN2 32
#define RIGHT_EN 14

// =========================
// SETTINGS
// =========================

int motorSpeed = 180;

bool autoMode = false;

unsigned long lastAutoAction = 0;

// =========================
// HTML
// =========================

const char MAIN_PAGE[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>SENTRO BOT</title>

<style>

* {
    box-sizing: border-box;
}

body {
    font-family: Arial, sans-serif;
    background: #0a314b;
    color: white;
    text-align: center;
    margin: 0;
    padding: 20px;
}

h1 {
    margin: 5px 0;
    font-size: 32px;
}

.subtitle {
    margin-top: 0;
    color: #b8d4e5;
}

.status {
    background: #123f5c;
    padding: 18px;
    border-radius: 15px;
    margin: 20px auto;
    max-width: 500px;
}

.mode {
    font-size: 28px;
    font-weight: bold;
    margin: 10px;
}

.mode-manual {
    color: #2ecc71;
}

.mode-auto {
    color: #f1c40f;
}

button {
    border: none;
    border-radius: 12px;
    font-size: 18px;
    font-weight: bold;
    cursor: pointer;
    transition: 0.15s;
}

button:active {
    transform: scale(0.95);
}

/* =========================
   MODE BUTTON
========================= */

.modeButton {
    width: 240px;
    height: 55px;
    background: #9b59b6;
    color: white;
    margin-bottom: 20px;
}

/* =========================
   CONTROL
========================= */

.control-panel {
    max-width: 500px;
    margin: auto;
}

.control-row {
    display: flex;
    justify-content: center;
    gap: 10px;
}

.control {
    width: 110px;
    height: 60px;
    margin: 6px;
    color: white;
}

.forward {
    background: #2ecc71;
}

.backward {
    background: #3498db;
}

.left,
.right {
    background: #f1c40f;
    color: #222;
}

.stop {
    background: #e74c3c;
}

/* =========================
   MONITORING
========================= */

.monitor {
    max-width: 600px;
    margin: 25px auto;
    background: #123f5c;
    padding: 20px;
    border-radius: 15px;
}

.monitor h2 {
    margin-top: 0;
}

.sensor-container {
    display: flex;
    justify-content: center;
    gap: 10px;
    flex-wrap: wrap;
}

.sensor {
    background: #0a314b;
    padding: 15px;
    border-radius: 12px;
    min-width: 140px;
}

.sensor-title {
    font-size: 14px;
    color: #b8d4e5;
}

.distance {
    font-size: 28px;
    font-weight: bold;
}

.cm {
    font-size: 14px;
}

/* =========================
   AUTO STATUS
========================= */

.auto-status {
    margin-top: 20px;
    padding: 15px;
    background: #0a314b;
    border-radius: 12px;
}

#robotStatus {
    font-size: 22px;
    font-weight: bold;
}

.warning {
    color: #e74c3c;
}

.safe {
    color: #2ecc71;
}

</style>

</head>


<body>

<h1>SENTRO</h1>

<p class="subtitle">
Software Engineering Navigation & Tracking Robot
</p>


<!-- =========================
     MODE
========================= -->

<div class="status">

<p>MODE ROBOT</p>

<div id="mode" class="mode mode-manual">
MANUAL
</div>

<button class="modeButton" onclick="toggleMode()">
UBAH MODE
</button>

</div>


<!-- =========================
     CONTROL
========================= -->

<div class="control-panel">

<h2>CONTROL</h2>

<div>
<button class="control forward"
ontouchstart="send('forward')"
onmousedown="send('forward')"
ontouchend="send('stop')"
onmouseup="send('stop')">

MAJU

</button>
</div>


<div class="control-row">

<button class="control left"
onclick="send('left')">

KIRI

</button>


<button class="control stop"
onclick="send('stop')">

STOP

</button>


<button class="control right"
onclick="send('right')">

KANAN

</button>

</div>


<div>

<button class="control backward"
ontouchstart="send('backward')"
onmousedown="send('backward')"
ontouchend="send('stop')"
onmouseup="send('stop')">

MUNDUR

</button>

</div>

</div>


<!-- =========================
     MONITORING
========================= -->

<div class="monitor">

<h2>MONITORING SENSOR</h2>

<div class="sensor-container">


<div class="sensor">

<div class="sensor-title">
DEPAN
</div>

<div class="distance" id="front">
--
</div>

<div class="cm">
cm
</div>

</div>


<div class="sensor">

<div class="sensor-title">
KIRI
</div>

<div class="distance" id="left">
--
</div>

<div class="cm">
cm
</div>

</div>


<div class="sensor">

<div class="sensor-title">
KANAN
</div>

<div class="distance" id="right">
--
</div>

<div class="cm">
cm
</div>

</div>


</div>


<!-- =========================
     AUTO STATUS
========================= -->

<div class="auto-status">

<p>STATUS ROBOT</p>

<div id="robotStatus">
STANDBY
</div>

</div>

</div>


<script>


// =========================
// CONTROL
// =========================

function send(command)
{

    fetch('/control?cmd=' + command);

}


// =========================
// MODE
// =========================

function toggleMode()
{

    fetch('/mode')

    .then(response => response.text())

    .then(data => {

        updateMode(data);

    });

}


function updateMode(data)
{

    const mode =
        document.getElementById('mode');

    mode.innerText = data;

    mode.classList.remove(
        'mode-manual',
        'mode-auto'
    );


    if(data === "AUTO")
    {

        mode.classList.add(
            'mode-auto'
        );

    }
    else
    {

        mode.classList.add(
            'mode-manual'
        );

    }

}


// =========================
// MONITORING
// =========================

function updateMonitoring()
{

    fetch('/sensor')

    .then(response => response.json())

    .then(data => {

        document.getElementById(
            'front'
        ).innerText = data.front;


        document.getElementById(
            'left'
        ).innerText = data.left;


        document.getElementById(
            'right'
        ).innerText = data.right;


        const status =
            document.getElementById(
                'robotStatus'
            );


        if(data.mode === "AUTO")
        {

            if(data.front <= 20)
            {

                status.innerText =
                    "⚠ HALANGAN TERDETEKSI";

                status.className =
                    "warning";

            }
            else
            {

                status.innerText =
                    "✓ JALAN AMAN";

                status.className =
                    "safe";

            }

        }
        else
        {

            status.innerText =
                "MANUAL CONTROL";

            status.className =
                "";

        }

    })

    .catch(error => {

        console.log(
            "Monitoring error:",
            error
        );

    });

}


// Update sensor setiap 500 ms

setInterval(
    updateMonitoring,
    500
);


// Jalankan pertama kali

updateMonitoring();


</script>

</body>

</html>

)rawliteral";

// =========================
// ULTRASONIC FUNCTION
// =========================

long readDistance(int trigPin, int echoPin)
{

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    long duration = pulseIn(
        echoPin,
        HIGH,
        30000);

    if (duration == 0)
    {
        return 999;
    }

    return duration * 0.034 / 2;
}

// =========================
// MOTOR CONTROL
// =========================

void leftMotor(int speed)
{

    if (speed > 0)
    {

        digitalWrite(LEFT_IN1, HIGH);
        digitalWrite(LEFT_IN2, LOW);
    }
    else if (speed < 0)
    {

        digitalWrite(LEFT_IN1, LOW);
        digitalWrite(LEFT_IN2, HIGH);
    }
    else
    {

        digitalWrite(LEFT_IN1, LOW);
        digitalWrite(LEFT_IN2, LOW);
    }

    analogWrite(LEFT_EN, abs(speed));
}

void rightMotor(int speed)
{

    if (speed > 0)
    {

        digitalWrite(RIGHT_IN1, HIGH);
        digitalWrite(RIGHT_IN2, LOW);
    }
    else if (speed < 0)
    {

        digitalWrite(RIGHT_IN1, LOW);
        digitalWrite(RIGHT_IN2, HIGH);
    }
    else
    {

        digitalWrite(RIGHT_IN1, LOW);
        digitalWrite(RIGHT_IN2, LOW);
    }

    analogWrite(RIGHT_EN, abs(speed));
}

// =========================
// MOVEMENT
// =========================

void forward()
{

    leftMotor(motorSpeed);
    rightMotor(motorSpeed);
}

void backward()
{

    leftMotor(-motorSpeed);
    rightMotor(-motorSpeed);
}

void turnLeft()
{

    leftMotor(-motorSpeed);
    rightMotor(motorSpeed);
}

void turnRight()
{

    leftMotor(motorSpeed);
    rightMotor(-motorSpeed);
}

void stopRobot()
{

    leftMotor(0);
    rightMotor(0);
}

// =========================
// MANUAL CONTROL
// =========================

void handleControl()
{

    if (!server.hasArg("cmd"))
    {
        server.send(400, "text/plain", "Missing command");
        return;
    }

    String command = server.arg("cmd");

    // Manual control hanya aktif
    // ketika mode MANUAL

    if (!autoMode)
    {

        if (command == "forward")
        {

            forward();
        }
        else if (command == "backward")
        {

            backward();
        }
        else if (command == "left")
        {

            turnLeft();
        }
        else if (command == "right")
        {

            turnRight();
        }
        else if (command == "stop")
        {

            stopRobot();
        }
    }

    server.send(200, "text/plain", "OK");
}

// =========================
// MODE
// =========================

void handleMode()
{

    autoMode = !autoMode;

    stopRobot();

    if (autoMode)
    {

        server.send(
            200,
            "text/plain",
            "AUTO");
    }
    else
    {

        server.send(
            200,
            "text/plain",
            "MANUAL");
    }
}

// =========================
// AUTONOMOUS
// =========================

void autonomousMode()
{

    long front = readDistance(
        FRONT_TRIG,
        FRONT_ECHO);

    long left = readDistance(
        LEFT_TRIG,
        LEFT_ECHO);

    long right = readDistance(
        RIGHT_TRIG,
        RIGHT_ECHO);

    Serial.print("Front: ");
    Serial.print(front);

    Serial.print(" | Left: ");
    Serial.print(left);

    Serial.print(" | Right: ");
    Serial.println(right);

    // =========================
    // TIDAK ADA HALANGAN
    // =========================

    if (front > 35)
    {

        forward();
    }

    // =========================
    // ADA HALANGAN DEPAN
    // =========================

    else
    {

        stopRobot();

        delay(100);

        // pilih arah yang lebih kosong

        if (left > right)
        {

            turnLeft();

            delay(400);
        }
        else
        {

            turnRight();

            delay(400);
        }

        stopRobot();

        delay(100);
    }
}

// ========================= // SENSOR API // =========================
void handleSensor()
{
    long front = readDistance(FRONT_TRIG, FRONT_ECHO);
    long left = readDistance(LEFT_TRIG, LEFT_ECHO);
    long right = readDistance(RIGHT_TRIG, RIGHT_ECHO);
    String json = "{";
    json += "\"front\":";
    json += front;
    json += ",\"left\":";
    json += left;
    json += ",\"right\":";
    json += right;
    json += ",\"mode\":\"";
    json += autoMode ? "AUTO" : "MANUAL";
    json += "\"";
    json += "}";
    server.send(200, "application/json", json);
}

// =========================
// SETUP
// =========================

void setup()
{

    Serial.begin(115200);

    // Ultrasonic

    pinMode(FRONT_TRIG, OUTPUT);
    pinMode(FRONT_ECHO, INPUT);

    pinMode(LEFT_TRIG, OUTPUT);
    pinMode(LEFT_ECHO, INPUT);

    pinMode(RIGHT_TRIG, OUTPUT);
    pinMode(RIGHT_ECHO, INPUT);

    // Motor

    pinMode(LEFT_IN1, OUTPUT);
    pinMode(LEFT_IN2, OUTPUT);
    pinMode(LEFT_EN, OUTPUT);

    pinMode(RIGHT_IN1, OUTPUT);
    pinMode(RIGHT_IN2, OUTPUT);
    pinMode(RIGHT_EN, OUTPUT);

    stopRobot();

    // =========================
    // WIFI ACCESS POINT
    // =========================

    WiFi.softAP(
        ssid,
        password);

    Serial.println();

    Serial.println(
        "SMARTBOT WiFi started");

    Serial.print(
        "IP Address: ");

    Serial.println(
        WiFi.softAPIP());

    // =========================
    // WEB SERVER
    // =========================

    server.on(
        "/",
        []()
        {
            server.send(
                200,
                "text/html",
                MAIN_PAGE);
        });

    server.on(
        "/control",
        handleControl);

    server.on(
        "/mode",
        handleMode);

    server.on("/sensor", handleSensor);

    server.begin();

    Serial.println(
        "Web server started");
}

// =========================
// LOOP
// =========================

void loop()
{

    server.handleClient();

    if (autoMode)
    {

        // Jalankan autonomous
        // setiap 100ms

        if (
            millis() -
                lastAutoAction >=
            100)
        {

            lastAutoAction = millis();

            autonomousMode();
        }
    }
}