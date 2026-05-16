const translations = {
    en: {
        // Sidebar
        sidebarTitle:   "🔥 Torch Panel",
        menuDashboard:  "Dashboard",
        menuModes:      "Modes",
        menuTesting:    "Testing",
        menuStatus:     "Status",

        // Dashboard
        dashboardHeader:   "Dashboard",
        dashboardWelcome:  "Welcome to your Torch Game!",
        dashboardBody:     "Get ready to play. Up to five buoys, each glowing in its own colour, are your playing field. Whether you're playing as a single team or going head-to-head with multiple, the buoys react, shift, and challenge you in every round. Pick a game mode, configure your teams, and get started.",
        btnSelectMode:     "Select Game Mode",

        // Modes
        modesHeader:    "Mode Selection",

        // Testing
        testingHeader:  "Torch Testing",
        btnClearAll:    "Clear All Buoys",

        // Status
        statusHeader:   "Torch Status",
        voltage:        "Voltage",

        // Game settings shared
        labelPlayers:     "Players",
        labelTeams:       "Teams",
        labelDuration:    "Duration",
        labelBrightness:  "Brightness",

        // Game controls
        gameControls:   "Game Controls",
        btnStart:       "Start Game",
        btnStop:        "Stop",
        btnRestart:     "Restart",

        // Status card
        labelRunning:   "Running",
        labelHolder:    "Holder",
        labelPasses:    "Passes",
        labelTime:      "Time",
        labelPhase:     "Phase",
        labelConfirmed: "Confirmed",
        labelPending:   "Pending",
        labelTeamScores:"Team Scores",
        labelBuoys:     "Buoys",
        labelTeamTimes: "Team Times",
        labelYes:       "yes",
        labelNo:        "no",

        // Torch rows
        torch:          "Torch",
        team:           "Team",
        player:         "Player",

        // Game over modal
        gameFinished:   "🔥 Game Finished",
        totalPasses:    "Total Passes",
        btnOk:          "OK",
        tieText:        "Tie between Team",
        winsText:       "wins!",
        andText:        "and Team",

        // Info modal
        modeInfo:       "Mode Info",

        // Settings
        settingsHeader: "Settings",
        labelLanguage:  "Language",

        // Modes
        modes: {
            colorRush: {
                label:       "Color Rush (One team)",
                description: "ColorRush is a fast-paced team game in which players make passes across a field of glowing buoys. The mission? Make as many passes of your coloured buoys as possible before time runs out. Every successful pass counts. Every second counts. The team that makes the most passes within the given time wins the round.",
                details:     "Only one buoy is active at a time. Before the round begins, each team member is assigned a colour. When the clock starts, one of the buoys lights up in a random colour. If it matches your colour, rush to it and make the pass — your team will score a point. As soon as a buoy is touched, it goes dark and a new buoy lights up somewhere on the field, displaying a new random colour. The action never stops, the targets keep shifting and every second counts. The team that makes the most passes before the timeout wins the round."
            },
            masterRush: {
                label:       "Master Rush (One team)",
                description: "Master Rush is a fast-paced team game with a twist — every pass must be confirmed. Touch your coloured buoy to score, then race to the glowing white Master buoy to lock it in. Miss the Master and the point won't count. Speed and discipline win the round.",
                details:     "One buoy is designated the Master buoy for the entire round. When the clock starts, one of the remaining buoys lights up in a random colour. If it matches your colour, rush to it and make the pass — but the point is not confirmed yet. The Master buoy immediately glows white, signalling that a player must touch it to lock in the score. Once the Master is touched, it goes dark and a new random coloured buoy lights up somewhere on the field. The action never stops, the targets keep shifting and every confirmation counts. The team that makes the most confirmed passes before the timeout wins the round."
            },
            colorClush: {
                label:       "Color Clash (Multiple teams)",
                description: "ColorClash is a fast-paced multiplayer game where two or more teams compete on the same field at the same time. Each team has their own colour — and their own buoy to chase. The team that makes the most passes before time runs out wins the round.",
                details:     "Multiple buoys are active at once — one for each team. Before the round begins, each team is assigned a colour. When the clock starts, one buoy per team lights up in that team's colour. If your team's buoy is lit, rush to it and make the pass — your team scores a point. The moment a buoy is touched, it instantly reactivates on a different buoy somewhere on the field. All teams are chasing their targets simultaneously, paths cross, and the field becomes a blur of colour and movement. The team with the most passes before the timeout wins the round."
            },
            colorConquest: {
                label:       "Color Conquest (Two teams)",
                description: "Two teams battle to dominate the field. Touch a buoy to claim it for your team — then defend it, or steal it back. The team that holds the most buoys for the longest time wins.",
                details:     "The field starts dark — no buoys are active. Two teams are assigned a colour each: red and blue. When a player touches any buoy, it lights up in their team's colour (or touch it twice to get their team's colour), claiming it. From that moment, time starts counting for that team. Any buoy can be stolen at any time — touch an opponent's buoy and it instantly switches to your colour, transferring the time count to you. At the end of the round, the team whose colour has been lit up for the longest combined time across all buoys wins."
            }
        }
    },

    hu: {
        sidebarTitle:   "🔥 Jelzőfény Panel",
        menuDashboard:  "Főoldal",
        menuModes:      "Játékmódok",
        menuTesting:    "Tesztelés",
        menuStatus:     "Állapot",

        dashboardHeader:   "Főoldal",
        dashboardWelcome:  "Üdvözlünk a Jelzőfény Játékban!",
        dashboardBody:     "Állj készen a játékra! Legfeljebb öt bója, mindegyik saját színnel világít – ez a pályád. Akár egy csapatban játszol, akár több csapat méri össze erejét, a bóják reagálnak, váltanak és kihívás elé állítanak minden körben. Válassz játékmódot, állítsd be a csapatokat, és kezdjük!",
        btnSelectMode:     "Játékmód kiválasztása",

        modesHeader:    "Játékmód választása",

        testingHeader:  "Jelzőfény tesztelés",
        btnClearAll:    "Összes bója törlése",

        statusHeader:   "Jelzőfény állapot",
        voltage:        "Feszültség",

        labelPlayers:     "Játékosok",
        labelTeams:       "Csapatok",
        labelDuration:    "Időtartam",
        labelBrightness:  "Fényerő",

        gameControls:   "Játékvezérlés",
        btnStart:       "Játék indítása",
        btnStop:        "Leállítás",
        btnRestart:     "Újraindítás",

        labelRunning:   "Fut",
        labelHolder:    "Tartó",
        labelPasses:    "Átadások",
        labelTime:      "Idő",
        labelPhase:     "Fázis",
        labelConfirmed: "Megerősített",
        labelPending:   "Függőben",
        labelTeamScores:"Csapatpontok",
        labelBuoys:     "Bóják",
        labelTeamTimes: "Csapatidők",
        labelYes:       "igen",
        labelNo:        "nem",

        torch:          "Bója",
        team:           "Csapat",
        player:         "Játékos",

        gameFinished:   "🔥 Játék vége",
        totalPasses:    "Összes átadás",
        btnOk:          "OK",
        tieText:        "Döntetlen a következők között:",
        winsText:       "nyert!",
        andText:        "és",

        modeInfo:       "Mód info",

        settingsHeader: "Beállítások",
        labelLanguage:  "Nyelv",

        modes: {
            colorRush: {
                label:       "Color Rush (Egy csapat)",
                description: "A ColorRush egy gyors csapatjáték, amelyben a játékosok passzolnak a világító bóják között. A cél? Minél több saját színű bójának passzolj az idő lejárta előtt. Minden sikeres átadás számít. Minden másodperc számít. A legtöbb átadást elérő csapat nyeri a kört.",
                details:     "Egyszerre csak egy bója aktív. A kör kezdete előtt minden csapattag kap egy színt. Amikor az óra elindul, az egyik bója véletlenszerű színnel felgyullad. Ha egyezik a te színeddel, rohanj oda és érintsd meg a bóját — csapatod pontot szerez. Amint egy bóját megérintenek, kialszik, és egy másik gyullad fel a pályán, új véletlenszerű színnel. A játék sosem áll meg, a célpontok folyamatosan változnak, és minden másodperc számít. A kör végéig legtöbb átadást elérő csapat nyer."
            },
            masterRush: {
                label:       "Master Rush (Egy csapat)",
                description: "A Master Rush egy gyors csapatjáték egy csavarral — minden átadást meg kell erősíteni. Érintsd meg a saját színű bójád a pontért, majd rohanj a fehéren világító Master bójához a megerősítésért. Ha kihagyod a Mastert, a pont nem számít. Gyorsaság és fegyelem dönt.",
                details:     "Az egész kör során egy bója a Master bója. Amikor az óra elindul, az egyik többi bója véletlenszerű színnel felgyullad. Ha egyezik a te színeddel, rohanj oda és add tovább a labdát — de a pont még nem megerősített. A Master bója azonnal fehéren felgyullad, jelezve, hogy valakinek meg kell érintenie a pont rögzítéséhez. Ha a Mastert megérintik, kialszik, és egy új véletlenszerű színű bója gyullad fel valahol a pályán. A játék sosem áll meg, a célpontok folyamatosan váltakoznak, és minden megerősítés számít. A legtöbb megerősített átadást elérő csapat nyeri a kört."
            },
            colorClush: {
                label:       "Color Clash (Több csapat)",
                description: "A ColorClash egy gyors többcsapatos játék, ahol két vagy több csapat egyszerre verseng ugyanazon a pályán. Minden csapatnak saját színe van — és saját bójája, amit kergetnie kell. A legtöbb átadást elérő csapat nyeri a kört.",
                details:     "Egyszerre több bója is aktív — csapatonként egy. A kör elején minden csapat kap egy színt. Amikor az óra elindul, csapatonként egy bója felgyullad az adott csapat színével. Ha a csapatod bójája világít, rohanj oda és add tovább — csapatod pontot szerez. Amint egy bóját megérintenek, azonnal újraaktiválódik valahol máshol a pályán. Minden csapat egyszerre hajszolja a célpontját, az utak keresztezik egymást, és a pálya szín- és mozgáskavalkáddá válik. Az időkorlát előtt legtöbb átadást elérő csapat nyer."
            },
            colorConquest: {
                label:       "Color Conquest (Két csapat)",
                description: "Két csapat küzd a pálya uralmáért. Érintj meg egy bóját, hogy a csapatodnak foglald el — majd védd meg, vagy visszaszerezheted. A csapat nyer, amelyik a legtöbb ideig tartja a legtöbb bóját.",
                details:     "A pálya sötéten indul — egyetlen bója sem aktív. Két csapat kap egy-egy színt: piros és kék. Ha egy játékos megérint egy bóját, az felgyullad a csapata színével, elfoglalva azt. Ettől a pillanattól az idő elkezd számolni az adott csapatnak. Bármely bóját bármikor el lehet venni — érintsd meg az ellenfél bóját, és azonnal a te színedre vált, az időszámlálás is átkerül hozzád. A kör végén az a csapat nyer, amelyiknek a színe összesítve a legtöbb ideig világított az összes bóján."
            }
        }
    }
};

let currentLang = "hu";

function t(key) {
    return translations[currentLang][key] ?? translations["en"][key] ?? key;
}

// On load — restore saved language, fall back to browser language, then "en"
function initLanguage() {
    const saved    = localStorage.getItem("lang");
    const browser  = navigator.language?.slice(0, 2); // "hu", "en", "de" …
    const fallback = translations[browser] ? browser : "en";
    currentLang    = saved ?? fallback;

    // sync the <select> to match
    const sel = document.getElementById("langSelect");
    if (sel) sel.value = currentLang;

    applyTranslations();
}

function setLanguage(lang) {
    currentLang = lang;
    localStorage.setItem("lang", lang);
    applyTranslations();
    loadModes();            // re-render mode list (labels don't use data-i18n)
    rebuildTorchRows();     // re-render torch/test rows with new language

    // re-translate active mode header if one is selected
    if (selectedMode) {
        const header = document.getElementById("modeControls")?.querySelector(".header");
        if (header) {
            header.textContent = translations[currentLang].modes[selectedMode]?.label
                              ?? translations["en"].modes[selectedMode]?.label;
        }
    }
}

function applyTranslations() {
    document.querySelectorAll("[data-i18n]").forEach(el => {
        const key = el.getAttribute("data-i18n");
        el.textContent = t(key);
    });
}