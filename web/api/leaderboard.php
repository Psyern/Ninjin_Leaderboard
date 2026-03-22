<?php

require_once __DIR__ . "/config.php";

header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

if ($_SERVER["REQUEST_METHOD"] !== "GET") {
    http_response_code(405);
    echo json_encode(["error" => "Method not allowed"]);
    exit;
}

if (!file_exists($DATA_PATH)) {
    http_response_code(404);
    echo json_encode(["error" => "No data available"]);
    exit;
}

$data = file_get_contents($DATA_PATH);

if ($data === false) {
    http_response_code(500);
    echo json_encode(["error" => "Failed to read data"]);
    exit;
}

echo $data;
