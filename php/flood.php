<?php
header('Content-Type: application/json');

// Kết nối MySQL
$servername = "localhost";
$username = "user";
$password = "";
$dbname = "db_flood";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die(json_encode(["status" => "error", "message" => "Connection failed: " . $conn->connect_error]));
}

// Nhận dữ liệu từ ESP8266
$water_level = isset($_POST['water_level']) ? floatval($_POST['water_level']) : null;

if ($water_level === null) {
    echo json_encode(["status" => "error", "message" => "Invalid data"]);
    exit;
}

// Lưu vào database
$sql = "INSERT INTO flood_data (water_level, timestamp) VALUES (?, NOW())";
$stmt = $conn->prepare($sql);
$stmt->bind_param("d", $water_level);

if ($stmt->execute()) {
    echo json_encode(["status" => "success", "water_level" => $water_level]);
} else {
    echo json_encode(["status" => "error", "message" => "Database error"]);
}

$stmt->close();
$conn->close();
?>