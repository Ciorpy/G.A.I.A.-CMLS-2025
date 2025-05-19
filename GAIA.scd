s.options.blockSize = 64;
s.reboot; // Riavvia il server audio
NetAddr.langPort; // Verifica la porta del linguaggio per OSC
s.options.outDevice = "CABLE Input (VB-Audio Virtual Cable)"; // Uscita audio virtuale

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
	~tonalCenter = (~tempRaw.linlin(2000, 3000, 0, 11)).floor.clip(0, 11);
	~synth.set(\tonalCenter, ~tonalCenter);
}, "/sensors/temp");

OSCdef(\hum, { |msg|
	~humRaw = msg[1];
	~hum = ~humRaw.linlin(3000, 8000, 0.0, 1.0).clip(0.0, 1.0);
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


(
~reader = Routine({

	var normTemp;
    var normHum;
    var normLight;

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


OSCFunc.trace(true);  // Attiva la stampa di tutti i messaggi OSC ricevuti
OSCFunc.trace(false); // Disattivala quando non serve


(
SynthDef(\GAIA, { |	tonalCenter = 0, hum = 0.5, light = 0.5, amp = 0.5, triggerBar = 0, durBar = 10, triggerNote = 0, noteVal = 0, durNote = 0.5, out = 0 |

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

	// === BASSO ===
	var bassMidi = 24 + tonalCenter + Select.kr(row, basses.collect(_.first));
	var bassFreq = bassMidi.midicps;
	var step = 12;
	var freq1 = bassFreq;
	var freq2 = (bassMidi + step).midicps;
	var modFreq = light.linlin(0, 1, 12, 0);
	var lfo = SinOsc.kr(modFreq).range(0.7, 1.3);

	var bassSig = (
		LFPar.ar(freq1, 0.25, 0.5 * lfo) + LFPar.ar(freq2, 0.5, 0.3 * lfo)
	);

	// === ACCORDI ===
	var chord = Select.kr(row, chords).collect { |n| 48 + tonalCenter + n };
	var chordFreqs = chord.collect(_.midicps);
	var numHarm = light.linlin(0, 1, 1, 12).round;

	var chordSig = Mix.fill(chordFreqs.size, { |i|
		Blip.ar(chordFreqs[i], numHarm, 0.2)
	});

	// === SOMMA BASSO + ACCORDI CON MODULAZIONE===
	var lfoFreq = hum.linlin(0, 1, 0.2, 0.05); // 5–20 sec
	var ampLfo = SinOsc.kr(lfoFreq).range(0.3, 0.7);
	var harmonyDry = (bassSig + chordSig) * ampLfo;

	// === Riverbero controllato da umidità ===
	var dryWet = hum.linlin(0.0, 1.0, 0.1, 0.6);
	var room = hum.linlin(0.0, 1.0, 0.5, 1.0);

	var harmony = FreeVerb.ar(
		harmonyDry, mix: dryWet, room: room, damp: 0.6
	);

	// === Note singole ===
	var noteFreq = (72 + tonalCenter + noteVal).midicps;

	var noteEnv = EnvGen.kr(
		Env.perc(0.01, durNote),
		gate: Trig1.kr(triggerNote, 0.001)
	);

	var noteSig = noteEnv * SinOsc.ar(noteFreq, 0, 0.3);

	// === Mix e Output ===
	var sig = harmony + noteSig;
	Out.ar(out, [harmony, harmony]);
}).add;
)


(
~reader.play;  // Avvia la console lettura
~synth = Synth(\GAIA);  // Avvia il synth
)
