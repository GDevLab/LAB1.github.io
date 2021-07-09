# บันทึกการทำงาน

ใน LAB นี้เราจะทำการทดสอบ update firmware ให้กับ ESP8266 โดยให้ ESP8266 ตรวจสอบ version update อัตโนมัติ และถ้ามี firmware version update ก็ให้ download firmware จาก WebServer ผ่าน HTTP

**_ข้อมูลอ้างอิง_** : เว็บไซต์ arduino-esp8266.readthedocs.io > OTA Updates > [HTTP Server](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#http-server)

VDO ตัวอย่างนี้เป็นการอัพเดทเฟริมแวร์ผ่านทาง [github](https://www.youtube.com/watch?v=XM8GfFqBgSA)

สร้างฐานข้อมูล

~~~DB
-- phpMyAdmin SQL Dump
-- version 4.5.1
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Nov 15, 2016 at 05:07 PM
-- Server version: 10.1.16-MariaDB
-- PHP Version: 5.6.24

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

--
-- Database: `ESP8266OTA`
--

-- --------------------------------------------------------

--
-- Table structure for table `esp8266`
--

CREATE TABLE `esp8266` (
  `id` int(11) NOT NULL,
  `did` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `esp8266`
--

--
-- Indexes for dumped tables
--

--
-- Indexes for table `esp8266`
--
ALTER TABLE `esp8266`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `esp8266`
--
ALTER TABLE `esp8266`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
~~~

File add.php

~~~PHP
<?php
$did = $_GET['did'];
 
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "tutor";
 
// Create connection
$conn = new mysqli($servername, $username,$password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 
$val = $_GET['temp'];
$sql = "INSERT INTO esp8266(temp) VALUES ($val);";
 
if ($conn->query($sql) === TRUE) {
    echo "save OK";
} else {
    echo "Error:" . $sql . "<br>" . $conn->error;
}
 
$conn->close();
?>
~~~

~~~PHP

~~~

กำหนดตัวแปรค่าคงที่ส่วนกลางสำหรับเวอร์ชันเฟิร์มแวร์ปัจจุบัน

~~~C++
const char fwVersion = 1.0.0;
~~~

กำหนดตัวแปรค่าคงที่ URL Address เพื่อบอกที่อยู่ของเฟิร์มแวร์

~~~C++
const char* fwUrlBase = "http://192.168.1.1/";
~~~

Function เชคอัพเดทเฟริม์แวร์

**_Tip_** : คำสั่ง `WiFi.macAddress()` ใช้รับ MAC Address

~~~C++
// Function Check Update Firmware Version
void checkForUpdates() {
  String mac = getMAC();
  String fwURL = String( fwUrlBase );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  Serial.println( "Checking for firmware updates." );
  Serial.print( "MAC address: " );
  Serial.println( mac );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwVersionURL );
~~~

ตรวจสอบการตอบกลับจากฝั่ง Server

- รหัสตอบกลับ 200 และส่งภาพเฟิร์มแวร์ 
- รหัสตอบกลับ 304 เพื่อแจ้ง ESP ว่าไม่จำเป็นต้องอัปเดต

~~~C++

~~~

ดาวน์โหลดสตริงเวอร์ชันและแปลงเป็นจำนวนเต็ม 32 บิต














Simple updater ดาวน์โหลดไฟล์ทุกครั้งที่มีการเรียกใช้ฟังก์ชัน

~~~C++
WiFiClient client;
ESPhttpUpdate.update(client, "192.168.0.2", 80, "/arduino.bin");
~~~

การอัพเดทไปยังสคริปต์บนเซิร์ฟเวอร์ เมื่อมีการเรียกใช้ฟังก์ชันสตริงเวอร์ชัน ESP8266 จะส่งการร้องขอไปยังเซิฟเวอร์ เพื่อตรวจสอบหาเวอร์ชั่นอัพเดท และ สคริปต์ฝั่งเซิร์ฟเวอร์สามารถใช้สตริงนี้เพื่อตรวจสอบว่าควรทำการอัปเดตหรือไม่

สคริปต์ฝั่งเซิร์ฟเวอร์สามารถตอบสนองได้ดังนี้: - รหัสตอบกลับ 200 และส่งภาพเฟิร์มแวร์ - หรือรหัสตอบกลับ 304 เพื่อแจ้ง ESP ว่าไม่จำเป็นต้องอัปเดต

~~~C++
String binFile = SERVER;
binFile += SKETCH_BIN;

t_httpUpdate_return ret = ESPhttpUpdate.update(binFile);
// Example
// t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");
// or
// t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.0.2", 80, "/esp/update/arduino.php", "optional current version string here");

switch(ret) {
    case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        USE_SERIAL.println(binFile);
        break;
    case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;
    case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
}
~~~

จากตัวอย่างข้างต้นเป็นการใช้งาน `t_httpUpdate_return ret = ESPhttpUpdate.update(binFile)` เพื่อตรวจสอบการอัพเดท

ตัวอย่าง code php ฝั่งเซิร์ฟเวอร์

~~~PHP
<?PHP

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
    if(!isset($_SERVER[$name])) {
        return false;
    }
    if($value && $_SERVER[$name] != $value) {
        return false;
    }
    return true;
}

function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

if(!check_header('User-Agent', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater!\n";
    exit();
}

if(
    !check_header('x-ESP8266-STA-MAC') ||
    !check_header('x-ESP8266-AP-MAC') ||
    !check_header('x-ESP8266-free-space') ||
    !check_header('x-ESP8266-sketch-size') ||
    !check_header('x-ESP8266-sketch-md5') ||
    !check_header('x-ESP8266-chip-size') ||
    !check_header('x-ESP8266-sdk-version')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater! (header)\n";
    exit();
}

$db = array(
    "18:FE:AA:AA:AA:AA" => "DOOR-7-g14f53a19",
    "18:FE:AA:AA:AA:BB" => "TEMP-1.0.0"
);

if(!isset($db[$_SERVER['x-ESP8266-STA-MAC']])) {
    header($_SERVER["SERVER_PROTOCOL"].' 500 ESP MAC not configured for updates', true, 500);
}

$localBinary = "./bin/".$db[$_SERVER['x-ESP8266-STA-MAC']].".bin";

// Check if version has been set and does not match, if not, check if
// MD5 hash between local binary and ESP8266 binary do not match if not.
// then no update has been found.
if((!check_header('x-ESP8266-sdk-version') && $db[$_SERVER['x-ESP8266-STA-MAC']] != $_SERVER['x-ESP8266-version'])
    || $_SERVER["x-ESP8266-sketch-md5"] != md5_file($localBinary)) {
    sendFile($localBinary);
} else {
    header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
}

header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);
~~~
