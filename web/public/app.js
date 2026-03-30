const editor = document.getElementById("editor");
const output = document.getElementById("output");
const runBtn = document.getElementById("runBtn");
const loadHelloBtn = document.getElementById("loadHello");
const loadSimpleBtn = document.getElementById("loadSimple");
const loadIfElseBtn = document.getElementById("loadIfElse");
const loadFunctionLoopBtn = document.getElementById("loadFunctionLoop");
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
  Pikachu hp evo 72 dot

  Brock fob hp > 80 fcb OpenBall
    pikapika fob "Excellent health" fcb dot
  CloseBall Misty fob hp >= 50 fcb OpenBall
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

clearEditorBtn.addEventListener("click", () => {
  editor.value = "";
  output.textContent = "Editor cleared.";
});

runBtn.addEventListener("click", async () => {
  runBtn.disabled = true;
  output.textContent = "Running...";

  try {
    const res = await fetch("/api/run", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ code: editor.value }),
    });

    const data = await res.json();
    output.textContent = data.output || "No output.";
  } catch (err) {
    output.textContent = `Request failed: ${err.message}`;
  } finally {
    runBtn.disabled = false;
  }
});
