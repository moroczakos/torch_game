const N=5;
const pDiv=document.getElementById("players");
const tDiv=document.getElementById("tests");
const bright = document.getElementById("bright");

function step(id,delta){
	const input=document.getElementById(id);
	const display=document.getElementById(id+"Val");
	
	let value=parseInt(input.value);
	
	value+=delta;

	if(id==="colors"){
		value=Math.max(1,Math.min(10,value));
	}

	if(id==="time"){
		value=Math.max(5,Math.min(300,value));
	}

	input.value=value;
	display.textContent=value;
}

/* finish game popup */

function showGameOver(passes){
	document.getElementById("passResult").textContent = passes;
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
	await fetch("/start?" + q);
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
	
	run.textContent=s.running?"yes":"no";
	holder.textContent=s.holder;
	count.textContent=s.passes;
	timeLeft.textContent=s.timeLeft;
	
	if(lastRunning && !s.running){
		showGameOver(s.passes);
	}

	lastRunning = s.running;
	
	if(s.brightness!==undefined){
		bright.value=s.brightness;
		bVal.textContent=s.brightness;
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

function selectMode(id) {
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
}

loadModes();

setInterval(refresh,1000);
refresh();