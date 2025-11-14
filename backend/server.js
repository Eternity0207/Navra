const express = require("express");
const cors = require("cors");
const { spawn } = require("child_process");
const path = require("path");

const app = express();

app.use(cors());
app.use(express.json());

app.get("/", (req, res) => {
    res.send("Backend is running!");
});

app.post("/api/route", (req, res) => {
    console.log("=== Received Request ===");
    console.log("Request body:", JSON.stringify(req.body, null, 2));

    const exePath = path.join(__dirname, "optimizer");
    console.log("Executable path:", exePath);

    const fs = require('fs');
    if (!fs.existsSync(exePath)) {
        console.error("ERROR: optimizer.exe not found at", exePath);
        return res.status(500).json({
            success: false,
            error: "C++ optimizer executable not found"
        });
    }

    const child = spawn(exePath, [], {
        cwd: path.join(__dirname, "..", "backend"),
        stdio: ['pipe', 'pipe', 'pipe']
    });



    // Track if response was sent
    let responseSent = false;

    child.on('error', (err) => {
        console.error("ERROR: Failed to spawn process:", err);
        if (!responseSent) {
            responseSent = true;
            clearTimeout(timeoutId);
            return res.status(500).json({
                success: false,
                error: `Failed to start optimizer: ${err.message}`
            });
        }
    });

    const inputData = JSON.stringify(req.body);
    console.log("Sending to C++:", inputData);

    try {
        child.stdin.write(inputData);
        child.stdin.end();
    } catch (err) {
        console.error("ERROR: Failed to write to stdin:", err);
        if (!responseSent) {
            responseSent = true;
            clearTimeout(timeoutId);
            return res.status(500).json({
                success: false,
                error: `Failed to send data to optimizer: ${err.message}`
            });
        }
    }

    let output = "";
    let errorOutput = "";

    child.stdout.on("data", (data) => {
        const chunk = data.toString();
        console.log("C++ stdout:", chunk);
        output += chunk;
    });

    child.stderr.on("data", (data) => {
        const chunk = data.toString();
        console.error("C++ stderr:", chunk);
        errorOutput += chunk;
    });

    child.on("close", (code) => {
        console.log("=== C++ Process Closed ===");
        console.log("Exit code:", code);
        console.log("Full stdout:", output);
        console.log("Full stderr:", errorOutput);

        // Clear timeout
        clearTimeout(timeoutId);

        // Don't send response if already sent
        if (responseSent) {
            console.log("Response already sent, skipping");
            return;
        }
        responseSent = true;

        // Check for output even if exit code is non-zero
        if (!output.trim()) {
            return res.status(500).json({
                success: false,
                error: `C++ program produced no output (exit code: ${code})`,
                details: errorOutput
            });
        }

        try {
            const jsonData = JSON.parse(output);
            console.log("=== Parsed JSON ===");
            console.log(jsonData);

            // Validate response structure
            if (!jsonData.routeNames || !Array.isArray(jsonData.routeNames)) {
                throw new Error("Invalid response: missing routeNames array");
            }

            // Send success response
            res.json(jsonData);

        } catch (err) {
            console.error("ERROR: Failed to parse JSON:", err.message);
            return res.status(500).json({
                success: false,
                error: "Invalid JSON from C++ program",
                details: err.message,
                raw: output.substring(0, 500)
            });
        }
    });

    // Timeout after 30 seconds
    const timeoutId = setTimeout(() => {
        if (!responseSent && !child.killed) {
            console.error("ERROR: Timeout - killing C++ process");
            responseSent = true;
            child.kill();
            res.status(500).json({
                success: false,
                error: "C++ program timeout (>30s)"
            });
        }
    }, 30000);
});

const PORT = 5000;
app.listen(PORT, () => {
    console.log(`Backend is running!`);
});