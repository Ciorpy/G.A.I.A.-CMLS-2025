/*
=== GUIDA ALL'UTILIZZO ===

Con Arduino avviare in quest'ordine:
1. "SETUP E CONTROLLI" (solo ciò che è necessario),
2. "VARIABILI E RICEZIONE OSC",
3. "ROUTINE DI STAMPA",
4. "SYNTH",
5. "START WITH ARDUINO".

Senza Arduino e quindi con simulatore:
1. "SETUP E CONTROLLI" (solo ciò che è necessario),
2. "SYNTH",
3. "SIMULATORE ARDUINO",
4. "START WITH SIMULATORE".
*/

// === SETUP E CONTROLLI ===
s.reboot; // Riavvia il server audio
s.options.outDevice = "Output (VB-Audio Point)"; // Uscita audio virtuale
s.options.numOutputBusChannels = 6; // 6 uscite

NetAddr.langPort; // Verifica la porta del linguaggio per OSC
OSCFunc.trace(true);  // Attiva la stampa di tutti i messaggi OSC ricevuti
OSCFunc.trace(false); // Disattivala quando non serve


// === VARIABILI E RICEZIONE OSC ===
(
// === Variabili globali ===
~tempRaw = 0; ~tonalCenter = 0;
~humRaw = 0; ~hum = 0.5;
~lightRaw = 0; ~light = 0.5;
~triggerBar = 0;
~durBar = 10000; // durata della battuta, in millisecondi
~triggerNote = 0;
~noteVal = 0;
~durNote = 0.5;  // secondi


// === Ricezione OSC e aggiornamento synth ===
OSCdef(\temp, { |msg|
	~tempRaw = msg[1];
	~temp = ~tempRaw.linlin(2500, 3000, 0.0, 1.0).clip(0.0, 1.0);
	~synth.set(\temp, ~temp);
	~tonalCenter = (~tempRaw.linlin(2500, 3000, 0, 11)).floor.clip(0, 11);
	~synth.set(\tonalCenter, ~tonalCenter);
}, "/sensors/temp");

OSCdef(\hum, { |msg|
	~humRaw = msg[1];
	~hum = ~humRaw.linlin(5000, 8000, 0.0, 1.0).clip(0.0, 1.0);
	~synth.set(\hum, ~hum);
}, "/sensors/humi");

OSCdef(\light, { |msg|
	~lightRaw = msg[1];
	~light = ~lightRaw.linlin(0, 4095, 0.0, 1.0).clip(0.0, 1.0);
	~synth.set(\light, ~light);
}, "/sensors/rldr");

OSCdef(\triggerBar, { |msg|
	~triggerBar = msg[1];
	~synth.set(\triggerBar, ~triggerBar);
}, "/trigger/tbar");

OSCdef(\durBar, { |msg|
	~durBar = msg[1] / 1000;  // Converte da ms a secondi
	~synth.set(\durBar, ~durBar);
}, "/duratat/dbar");

OSCdef(\triggerNote, { |msg|
	~triggerNote = msg[1];
	~synth.set(\triggerNote, ~triggerNote);
}, "/trigger/tnot");

OSCdef(\durNote, { |msg|
	~durNote = msg[1] / 1000; // da ms a secondi
	~synth.set(\durNote, ~durNote);
}, "/duratat/dnot");

OSCdef(\noteVal, { |msg|
	~noteVal = msg[1];
	~synth.set(\noteVal, ~noteVal);
}, "/musnote/note");
)


// === ROUTINE DI STAMPA ===
(
~reader = Routine({
	loop {
		("Temp: " ++ (~tempRaw / 100).round(0.01) ++ " °C"
		 ++ " | Hum: " ++ (~humRaw / 100).round(0.01) ++ " %"
		 ++ " | Light: " ++ ~lightRaw).postln;

		("Tonal Center: " ++ ~tonalCenter
		 ++ " | triggerBar: " ++ ~triggerBar
		 ++ " | durBar: " ++ ~durBar ++ " s").postln;

		("Note: " ++ ~noteVal
		 ++ " | Trig: " ++ ~triggerNote
		 ++ " | Dur: " ++ ~durNote ++ " s").postln;

		"---------------------------".postln;

		0.2.wait;
	}
});
)


// === SYNTH ===
(
SynthDef(\GAIA, { | temp = 0.5,	tonalCenter = 0, hum = 0.5, light = 0.5, amp = 0.5, triggerBar = 0, durBar = 10, triggerNote = 0, noteVal = 0, durNote = 0.5, out = 0 |

	// === Parametri generali ===
	var rowCount = 9;

	// === Matrici armoniche ===
	var basses = [
		[0], [2], [7], [5], [7], [9], [4], [0], [2]
	];

	var chords = [
		[0, 5, 12], [2, 9, 16], [0, 7, 14],
		[5, 9, 14], [7, 11, 16], [9, 14, 21],
		[4, 7, 12], [4, 11, 16], [0, 9, 14]
	];

	// === Triggering ===
	var barTrig = Trig1.kr(triggerBar, 0.001);
	var row = Demand.kr(barTrig, 0, Dseq((0..(rowCount - 1)), inf));

	// === Bass ===
	var bassMidi = 24 + tonalCenter + Select.kr(row, basses.collect(_.first));
	var bassFreq = bassMidi.midicps;
	var step = 12;
	var freq1 = bassFreq;
	var freq2 = (bassMidi + step).midicps;
	var modFreq = light.linlin(0, 1, 12, 0);
	var lfo = SinOsc.kr(modFreq).range(0.7, 1.3);

	// === Chord ===
	var chord = Select.kr(row, chords).collect { |n| 48 + tonalCenter + n };
	var chordFreqs = chord.collect(_.midicps);
	var numHarm = light.linlin(0, 1, 1, 12).round;

	// === Modulazione===
	var lfoFreq = temp.linlin(0, 1, 0.2, 5); // 5–0.2 sec
	var ampLfo = SinOsc.kr(lfoFreq).range(0.3, 0.7);
	var durSound = hum.linlin(0, 1, (durBar / 2), 0.5);

	// === Inviluppo per accordi ===
	var envHarmony = EnvGen.kr(Env.linen(
		durSound, (durBar - (2 * durSound)), durSound
	), gate: barTrig);

	// === Segnali ===
	var bassSig = envHarmony * ampLfo * (
		LFPar.ar(freq1, 0.25, 0.5 * lfo) + LFPar.ar(freq2, 0.5, 0.3 * lfo)
	);
	var chordSig = envHarmony * ampLfo * Mix.fill(chordFreqs.size, { |i|
		Blip.ar(chordFreqs[i], numHarm, 0.2)
	});

	// === Note singole ===
	var noteFreq = (72 + tonalCenter + noteVal).midicps;

	var noteEnv = EnvGen.kr(
		Env.perc(0.01, durNote),
		gate: Trig1.kr(triggerNote, 0.001)
	);

	var noteSig = noteEnv * SinOsc.ar(noteFreq, 0, 0.3);

	Out.ar(out, [bassSig + chordSig + noteSig, bassSig + chordSig + noteSig]);
	//Out.ar(out, [chordSig, chordSig, bassSig, bassSig, noteSig, noteSig]);
}).add;
)

// === START WITH ARDUINO ===
(
~reader.play;  // Avvia la console lettura
~synth = Synth(\GAIA);  // Avvia il synth
)

// === SIMULATORE ARDUINO ===
(
var now;

// DESTINATARI DI SERVIZIO
~pr = NetAddr("127.0.0.1", 57130); // Processing
~jc = NetAddr("127.0.0.1", 57140); // JUCE

// === INIZIALIZZAZIONE ===
~tempRaw = 2500;
~humRaw = 5000;
~lightRaw = 2048;

~durBar = rrand(5000, 15000);   // ms
~durNote = rrand(1500, 3000);   // ms
~noteVal = rrand(0, 11);

~triggerBar = 0;
~triggerNote = 0;

~barStart = Main.elapsedTime;
~noteStart = Main.elapsedTime;

// === LOOP DI SIMULAZIONE ===
~simulateArduino = Routine({
	loop {
		now = Main.elapsedTime;

		// === TRIGGER BAR ===
		if (((now - ~barStart) * 1000) >= ~durBar) {
			~triggerBar = 1;
			~durBar = rrand(5000, 15000);
			~barStart = now;
		} {
			~triggerBar = 0;
		};

		// === TRIGGER NOTE ===
		if (((now - ~noteStart) * 1000) >= ~durNote) {
			~triggerNote = 1;
			~durNote = rrand(1500, 3000);
			~noteVal = rrand(0, 11);
			~noteStart = now;
		} {
			~triggerNote = 0;
		};

		// === SENSORI SIMULATI ===
		~tempRaw = rrand(1500, 3500);
		~humRaw = rrand(1000, 9000);
		~lightRaw = rrand(0, 4095);

		// === AGGIORNA SINTETIZZATORE DIRETTAMENTE ===
		~synth.set(
			\tonalCenter, ((~tempRaw.linlin(1500, 3500, 0, 11)).floor.clip(0, 11)),
			\hum, (~humRaw.linlin(1000, 9000, 0.0, 1.0).clip(0, 1)),
			\light, (~lightRaw.linlin(0, 4095, 0.0, 1.0).clip(0, 1)),
			\triggerBar, ~triggerBar,
			\durBar, (~durBar / 1000),
			\triggerNote, ~triggerNote,
			\durNote, (~durNote / 1000),
			\noteVal, ~noteVal
		);

		// === DEBUG STAMPA ===
		("Temp: " ++ (~tempRaw / 100).round(0.01) ++ " °C"
		 ++ " | Hum: " ++ (~humRaw / 100).round(0.01) ++ " %"
		 ++ " | Light: " ++ ~lightRaw).postln;

		("Tonal Center: " ++ (~tempRaw.linlin(1500, 3500, 0, 11)).floor
		 ++ " | triggerBar: " ++ ~triggerBar
		 ++ " | durBar: " ++ (~durBar / 1000).round(0.01) ++ " s").postln;

		("Note: " ++ ~noteVal
		 ++ " | Trig: " ++ ~triggerNote
		 ++ " | durNote: " ++ (~durNote / 1000).round(0.01) ++ " s").postln;

		"--------------------------".postln;

		// === INVIO A PROCESSING ===
		~pr.sendMsg("/sensors/temp", ~tempRaw);
		~pr.sendMsg("/sensors/humi", ~humRaw);
		~pr.sendMsg("/sensors/rldr", ~lightRaw);

		// === INVIO A JUCE ===
		~jc.sendMsg("/sensors/temp", ~tempRaw);
		~jc.sendMsg("/sensors/humi", ~humRaw);
		~jc.sendMsg("/sensors/rldr", ~lightRaw);

		0.2.wait;
	}
}).play(SystemClock);
)


// === START WITH SIMULATORE ===
~synth = Synth(\GAIA);  // Avvia il synth

