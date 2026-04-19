const N=5;
const pDiv=document.getElementById("players");
const tDiv=document.getElementById("tests");
const bright = document.getElementById("bright");

const teamColors = [
    "#ff0000",  // 0 — red
    "#00ff00",  // 1 — green
    "#0000ff",  // 2 — blue
    "#ffff00",  // 3 — yellow
    "#00ffff",  // 4 — cyan
    "#ff00ff",  // 5 — magenta
    "#ff8000",  // 6 — orange
    "#8000ff",  // 7 — purple
];

function step(id,delta){
	const input=document.getElementById(id);
	const display=document.getElementById(id+"Val");
	
	let value=parseInt(input.value);
	
	value+=delta;

	if(id === "colors"){
    if(selectedMode === "colorClush"){
        value = Math.max(2, Math.min(8, value));
    } else {
        value = Math.max(1, Math.min(8, value));
    }
}

	if(id==="time"){
		value=Math.max(5,Math.min(300,value));
	}

	input.value=value;
	display.textContent=value;
	
	if(id === "colors"){
        updateTeamColorPreview(value);
    }
}

function updateTeamColorPreview(count){
    const preview = document.getElementById("teamColorPreview");
    if(!preview) return;
	
    const label = selectedMode === "colorClush" ? "Team" : "Player";

    preview.innerHTML = "";
    for(let i = 0; i < count; i++){
        const color = teamColors[i] ?? "#999";
        preview.innerHTML += `
        <div class="teamColorRow">
            <span class="teamDot" style="background:${color}"></span>
            <span>${label} ${i+1}</span>
        </div>`;
    }
}

/* finish game popup */

function showGameOver(passes){
    const resultDiv = document.getElementById("passResult");
    resultDiv.innerHTML = "";
    resultDiv.textContent = passes;

    document.getElementById("gameOverModal").querySelector("p").textContent = "Total Passes";
    document.getElementById("gameOverModal").classList.add("show");
}

function showGameOverTeams(teamPasses){
    const max = Math.max(...teamPasses);
    const winners = teamPasses
        .map((passes, i) => passes === max ? i + 1 : null)
        .filter(i => i !== null);

    const resultDiv = document.getElementById("passResult");
    resultDiv.innerHTML = "";
    teamPasses.forEach((passes, i) => {
        resultDiv.innerHTML += `<div class="teamScoreRow">Team ${i+1}: ${passes}</div>`;
    });

    const msgEl = document.getElementById("gameOverModal").querySelector("p");

    if(winners.length > 1){
        msgEl.textContent = "Tie between Team " + winners.join(" and Team ") + "!";
    } else {
        msgEl.textContent = "Team " + winners[0] + " wins!";
    }

    document.getElementById("gameOverModal").classList.add("show");
}

function closeModal(){
	document.getElementById("gameOverModal").classList.remove("show");
}

/* create torch UI */

for(let i=0;i<N;i++){            
	pDiv.innerHTML+=`
	<div class="torchRow">
	<span class="dot" id="dot${i}"></span>
	Torch ${i}
	</div>`;
	
	tDiv.innerHTML+=`
	<button onclick="test(${i})">Torch ${i}</button>`;            
}

/* sidebar menu */

function showMenu(id){            
	document.querySelectorAll(".menu").forEach(m=>{
		m.classList.remove("active");
	});
	
	document.getElementById(id).classList.add("active");				
}

/* brightness */

if(bright){
    bright.oninput = () => {
        bVal.textContent = bright.value;
        fetch(`/brightness?value=${bright.value}`);
    };
}

/* start game */

async function startGame(){
	const q = new URLSearchParams(new FormData(gameForm)).toString();
	await fetch("/start?" + q + "&mode=" + selectedMode);
}

/* controls */

async function stopGame(){ await fetch("/stop"); }
async function restartGame(){ await fetch("/restart"); }
async function test(i){ await fetch(`/test?id=${i}`); }

/* refresh status */

let lastRunning = false;
			
async function refresh(){            
	const r=await fetch("/status");
	const s=await r.json();
	
	if(s.online){
		for(let i=0;i<N;i++){
			document.getElementById(`dot${i}`)
			.style.background=s.online[i]?"lime":"red";
		}
	}
	
	const run = document.getElementById("run");
    const holder = document.getElementById("holder");
    const timeLeft = document.getElementById("timeLeft");
    const count = document.getElementById("count");

    if(run) run.textContent = s.running ? "yes" : "no";
    if(holder) holder.textContent = s.holder;
    if(timeLeft) timeLeft.textContent = s.timeLeft;
    if(count) count.textContent = s.passes;
	
	/* colorClush team scores */
    const teamScores = document.getElementById("teamScores");
    if(teamScores && s.teamPasses){
        teamScores.innerHTML = "";
        s.teamPasses.forEach((passes, i) => {
            teamScores.innerHTML += `
            <div class="statusRow">
                <span>Team ${i+1}</span>
                <span>${passes}</span>
            </div>`;
        });
    }

    if(lastRunning && !s.running){
        if(selectedMode === "colorClush" && s.teamPasses){
            showGameOverTeams(s.teamPasses);
        } else {
            showGameOver(s.passes);
        }
    }

    lastRunning = s.running;

    if(s.brightness !== undefined){
        const bright = document.getElementById("bright");
        if(bright){
            bright.value = s.brightness;
            document.getElementById("bVal").textContent = s.brightness;
        }
    }                       
}

/* info modal */

async function loadModes() {
    const list = document.querySelector(".modeList");
    list.innerHTML = "";

    modesData.forEach(mode => {
        list.innerHTML += `
        <div class="modeBlock">
            <button class="modeBtn" onclick="selectMode('${mode.id}')">${mode.label}</button>
            <button class="infoBtn" onclick="showInfo('${mode.id}')">ℹ</button>
        </div>`;
    });
}

function showInfo(id) {
    const mode = modesData.find(m => m.id === id);
    document.getElementById("infoTitle").textContent = mode.label;
    document.getElementById("infoText").textContent = mode.description;
	document.getElementById("infoDetails").textContent = mode.details;
	
    document.getElementById("infoModal").classList.add("show");
}

function closeInfoModal() {
    document.getElementById("infoModal").classList.remove("show");
}

let selectedMode = null;

function selectMode(id) {
	selectedMode = id;
	
    const mode = modesData.find(m => m.id === id);
    const tpl = document.getElementById(mode.template);
    const container = document.getElementById("modeControls");

    container.innerHTML = "";
    container.appendChild(tpl.content.cloneNode(true));

    // re-bind brightness slider after it's injected into the DOM
    const bright = document.getElementById("bright");
    if(bright){
        bright.oninput = () => {
            document.getElementById("bVal").textContent = bright.value;
            fetch(`/brightness?value=${bright.value}`);
        };
    }
	
	showMenu('selectedMode');
	updateTeamColorPreview(parseInt(document.getElementById("colors")?.value ?? 3));
}

loadModes();

setInterval(refresh,1000);
refresh();