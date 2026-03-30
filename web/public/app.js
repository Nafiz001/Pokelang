const editor = document.getElementById("editor");
const output = document.getElementById("output");
const runBtn = document.getElementById("runBtn");
const traceBtn = document.getElementById("traceBtn");
const loadHelloBtn = document.getElementById("loadHello");
const loadSimpleBtn = document.getElementById("loadSimple");
const loadIfElseBtn = document.getElementById("loadIfElse");
const loadFunctionLoopBtn = document.getElementById("loadFunctionLoop");
const loadCountTenBtn = document.getElementById("loadCountTen");
const formatIndentBtn = document.getElementById("formatIndent");
const clearEditorBtn = document.getElementById("clearEditor");

const helloProgram = `Move EmptyBall StartBattle fob fcb OpenBall
    pikapika fob "Hello from PokemonLang Studio" fcb dot
    back 0 dot
CloseBall
`;

const simpleProgram = `Move EmptyBall StartBattle fob fcb OpenBall
    Pikachu health evo 100 dot

    Brock fob health > 80 fcb OpenBall
        pikapika fob "Excellent health" fcb dot
    CloseBall Jessie OpenBall
        pikapika fob "Needs healing" fcb dot
    CloseBall

    back 0 dot
CloseBall
`;

const ifElseProgram = `Move EmptyBall StartBattle fob fcb OpenBall
  Pikachu healthPoints evo 72 dot

  Brock fob healthPoints > 80 fcb OpenBall
        pikapika fob "Excellent health" fcb dot
  CloseBall Misty fob healthPoints >= 50 fcb OpenBall
        pikapika fob "Moderate health" fcb dot
    CloseBall Jessie OpenBall
        pikapika fob "Critical health" fcb dot
    CloseBall

    back 0 dot
CloseBall
`;

const functionLoopProgram = `Move Pikachu add fob Pikachu a , Pikachu b fcb OpenBall
    back a atk b dot
CloseBall

Move EmptyBall StartBattle fob fcb OpenBall
    Pikachu i evo 0 dot
    Pikachu total evo 0 dot

    AshTrain fob i evo 1 dot i <= 5 dot i evo i atk 1 fcb OpenBall
        total evo add fob total , i fcb dot
    CloseBall

    Brock fob total == 15 fcb OpenBall
        pikapika fob "Sum from 1 to 5 is correct" fcb dot
    CloseBall Jessie OpenBall
        pikapika fob "Sum calculation failed" fcb dot
    CloseBall

    back 0 dot
CloseBall
`;

const countTenProgram = `Move EmptyBall StartBattle fob fcb OpenBall
    Pikachu i evo 0 dot

    AshTrain fob i evo 1 dot i <= 10 dot i evo i atk 1 fcb OpenBall
    CloseBall

    Brock fob i == 11 fcb OpenBall
        pikapika fob "Counted 1 to 10 successfully" fcb dot
    CloseBall Jessie OpenBall
        pikapika fob "Count failed" fcb dot
    CloseBall

    back 0 dot
CloseBall
`;

editor.value = helloProgram;

loadHelloBtn.addEventListener("click", () => {
  editor.value = helloProgram;
});

loadSimpleBtn.addEventListener("click", () => {
  editor.value = simpleProgram;
});

loadIfElseBtn.addEventListener("click", () => {
  editor.value = ifElseProgram;
});

loadFunctionLoopBtn.addEventListener("click", () => {
  editor.value = functionLoopProgram;
});

loadCountTenBtn.addEventListener("click", () => {
  editor.value = countTenProgram;
});

function countMatches(text, regex) {
  const found = text.match(regex);
  return found ? found.length : 0;
}

function countLeadingCloseBall(trimmedLine) {
  let rest = trimmedLine;
  let count = 0;
  while (rest.startsWith("CloseBall")) {
    count += 1;
    rest = rest.slice("CloseBall".length).trimStart();
  }
  return count;
}

function formatIndentationCode(source) {
  const lines = source.replace(/\r\n/g, "\n").split("\n");
  let depth = 0;
  const out = [];

  for (const rawLine of lines) {
    const trimmed = rawLine.trim();
    if (!trimmed) {
      out.push("");
      continue;
    }

    const leadingClose = countLeadingCloseBall(trimmed);
    const lineDepth = Math.max(depth - leadingClose, 0);
    out.push(`${" ".repeat(lineDepth * 4)}${trimmed}`);

    const opens = countMatches(trimmed, /\bOpenBall\b/g);
    const closes = countMatches(trimmed, /\bCloseBall\b/g);
    depth = Math.max(depth + opens - closes, 0);
  }

  return out.join("\n");
}

formatIndentBtn.addEventListener("click", () => {
  editor.value = formatIndentationCode(editor.value);
  output.textContent = "Indentation formatted using OpenBall/CloseBall depth rule (4 spaces per level).";
});

clearEditorBtn.addEventListener("click", () => {
  editor.value = "";
  output.textContent = "Editor cleared.";
});

async function executeCode(traceMode) {
  runBtn.disabled = true;
  traceBtn.disabled = true;
  output.textContent = traceMode ? "Running with trace..." : "Running...";

  try {
    const res = await fetch("/api/run", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ code: editor.value, trace: traceMode }),
    });

    const data = await res.json();
    output.textContent = data.output || "No output.";
  } catch (err) {
    output.textContent = `Request failed: ${err.message}`;
  } finally {
    runBtn.disabled = false;
    traceBtn.disabled = false;
  }
}

runBtn.addEventListener("click", async () => {
  await executeCode(false);
});

traceBtn.addEventListener("click", async () => {
  await executeCode(true);
});
