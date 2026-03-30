const http = require("http");
const fs = require("fs");
const path = require("path");
const { spawn } = require("child_process");
const { randomUUID } = require("crypto");

const HOST = "127.0.0.1";
const PORT = process.env.POKELANG_PORT ? Number(process.env.POKELANG_PORT) : 3001;
const ROOT = path.resolve(__dirname, "..");
const PUBLIC_DIR = path.join(__dirname, "public");
const TEMP_DIR = path.join(__dirname, "temp");

if (!fs.existsSync(TEMP_DIR)) {
  fs.mkdirSync(TEMP_DIR, { recursive: true });
}

function sendJson(res, statusCode, payload) {
  res.writeHead(statusCode, { "Content-Type": "application/json; charset=utf-8" });
  res.end(JSON.stringify(payload));
}

function sendFile(res, filePath) {
  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
      res.end("Not found");
      return;
    }

    const ext = path.extname(filePath).toLowerCase();
    const contentType =
      ext === ".html"
        ? "text/html; charset=utf-8"
        : ext === ".css"
        ? "text/css; charset=utf-8"
        : ext === ".js"
        ? "application/javascript; charset=utf-8"
        : "text/plain; charset=utf-8";

    res.writeHead(200, { "Content-Type": contentType });
    res.end(data);
  });
}

function runProgram(code) {
  return new Promise((resolve) => {
    const runId = randomUUID();
    const sourcePath = path.join(TEMP_DIR, `program-${runId}.poke`);

    fs.writeFileSync(sourcePath, code, "utf8");

    const ps = spawn(
      "powershell",
      [
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        path.join(ROOT, "run_program.ps1"),
        "-InputFile",
        sourcePath,
        "-Run",
      ],
      { cwd: ROOT }
    );

    let stdout = "";
    let stderr = "";
    let killedForTimeout = false;

    const timeout = setTimeout(() => {
      killedForTimeout = true;
      ps.kill("SIGTERM");
    }, 15000);

    ps.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    ps.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    ps.on("close", (codeValue) => {
      clearTimeout(timeout);

      try {
        fs.unlinkSync(sourcePath);
      } catch (_) {}

      if (killedForTimeout) {
        resolve({ ok: false, output: "Execution timed out after 15 seconds." });
        return;
      }

      const combined = [stdout.trim(), stderr.trim()].filter(Boolean).join("\n\n");
      resolve({ ok: codeValue === 0, output: combined || "No output." });
    });
  });
}

function runCompilerCheck(code) {
  return new Promise((resolve) => {
    const runId = randomUUID();
    const sourcePath = path.join(TEMP_DIR, `check-${runId}.poke`);
    const compilerExe = path.join(ROOT, "pokelang_compiler.exe");

    fs.writeFileSync(sourcePath, code, "utf8");

    if (!fs.existsSync(compilerExe)) {
      try {
        fs.unlinkSync(sourcePath);
      } catch (_) {}
      resolve({
        ok: false,
        output:
          "Compiler check requires pokelang_compiler.exe. Build first with raw commands or build script.",
      });
      return;
    }

    const proc = spawn(compilerExe, [sourcePath], { cwd: ROOT });

    let stdout = "";
    let stderr = "";
    let killedForTimeout = false;

    const timeout = setTimeout(() => {
      killedForTimeout = true;
      proc.kill("SIGTERM");
    }, 15000);

    proc.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    proc.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    proc.on("close", (codeValue) => {
      clearTimeout(timeout);

      try {
        fs.unlinkSync(sourcePath);
      } catch (_) {}

      if (killedForTimeout) {
        resolve({ ok: false, output: "Compiler check timed out after 15 seconds." });
        return;
      }

      const combined = [stdout.trim(), stderr.trim()].filter(Boolean).join("\n\n");
      resolve({ ok: codeValue === 0, output: combined || "No compiler output." });
    });
  });
}

function runTrace(code) {
  return new Promise((resolve) => {
    const runId = randomUUID();
    const sourcePath = path.join(TEMP_DIR, `trace-${runId}.poke`);
    const compilerExe = path.join(ROOT, "pokelang_compiler.exe");

    fs.writeFileSync(sourcePath, code, "utf8");

    if (!fs.existsSync(compilerExe)) {
      try {
        fs.unlinkSync(sourcePath);
      } catch (_) {}
      resolve({
        ok: false,
        output:
          "Trace mode requires pokelang_compiler.exe. Build first with raw commands or build script.",
      });
      return;
    }

    const proc = spawn(compilerExe, [sourcePath, "--trace"], { cwd: ROOT });

    let stdout = "";
    let stderr = "";
    let killedForTimeout = false;

    const timeout = setTimeout(() => {
      killedForTimeout = true;
      proc.kill("SIGTERM");
    }, 15000);

    proc.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    proc.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    proc.on("close", (codeValue) => {
      clearTimeout(timeout);

      try {
        fs.unlinkSync(sourcePath);
      } catch (_) {}

      if (killedForTimeout) {
        resolve({ ok: false, output: "Trace execution timed out after 15 seconds." });
        return;
      }

      const combined = [stdout.trim(), stderr.trim()].filter(Boolean).join("\n\n");
      resolve({ ok: codeValue === 0, output: combined || "No trace output." });
    });
  });
}

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://${req.headers.host}`);

  if (req.method === "GET" && (url.pathname === "/" || url.pathname === "/index.html")) {
    sendFile(res, path.join(PUBLIC_DIR, "index.html"));
    return;
  }

  if (req.method === "GET" && url.pathname === "/styles.css") {
    sendFile(res, path.join(PUBLIC_DIR, "styles.css"));
    return;
  }

  if (req.method === "GET" && url.pathname === "/app.js") {
    sendFile(res, path.join(PUBLIC_DIR, "app.js"));
    return;
  }

  if (req.method === "POST" && url.pathname === "/api/run") {
    let body = "";
    req.on("data", (chunk) => {
      body += chunk.toString();
      if (body.length > 500000) {
        req.destroy();
      }
    });

    req.on("end", async () => {
      try {
        const data = JSON.parse(body || "{}");
        const code = typeof data.code === "string" ? data.code : "";
        const trace = data.trace === true;

        if (!code.trim()) {
          sendJson(res, 400, { ok: false, output: "Please enter PokemonLang code." });
          return;
        }

        if (trace) {
          const traceResult = await runTrace(code);
          sendJson(res, traceResult.ok ? 200 : 400, traceResult);
          return;
        }

        const checkResult = await runCompilerCheck(code);
        if (!checkResult.ok) {
          sendJson(res, 400, checkResult);
          return;
        }

        const result = await runProgram(code);
        sendJson(res, result.ok ? 200 : 400, result);
      } catch (err) {
        sendJson(res, 500, { ok: false, output: `Server error: ${err.message}` });
      }
    });

    return;
  }

  res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
  res.end("Not found");
});

server.listen(PORT, HOST, () => {
  console.log(`PokemonLang Studio running at http://${HOST}:${PORT}`);
});
