<?php

require_once __DIR__ . "/config.php";

header("Content-Type: application/json");

if ($_SERVER["REQUEST_METHOD"] !== "POST") {
    http_response_code(405);
    echo json_encode(["success" => false, "message" => "Method not allowed"]);
    exit;
}

$apiKey = isset($_GET["api_key"]) ? $_GET["api_key"] : "";

if ($apiKey === "" || $apiKey !== $VALID_API_KEY) {
    http_response_code(401);
    echo json_encode(["success" => false, "message" => "Unauthorized"]);
    exit;
}

$body = file_get_contents("php://input");

if (empty($body)) {
    http_response_code(400);
    echo json_encode(["success" => false, "message" => "Empty request body"]);
    exit;
}

$decoded = json_decode($body);
if ($decoded === null && json_last_error() !== JSON_ERROR_NONE) {
    http_response_code(400);
    echo json_encode(["success" => false, "message" => "Invalid JSON: " . json_last_error_msg()]);
    exit;
}

$dataDir = dirname($DATA_PATH);
if (!is_dir($dataDir)) {
    mkdir($dataDir, 0755, true);
}

$result = file_put_contents($DATA_PATH, $body, LOCK_EX);

if ($result === false) {
    http_response_code(500);
    echo json_encode(["success" => false, "message" => "Failed to write data"]);
    exit;
}

echo json_encode(["success" => true, "message" => "Data received successfully"]);
