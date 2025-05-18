Server.killAll;
s;
s.boot;
{SinOsc.ar(440)}.play









(
SynthDef(\EnoAmbient, {
	arg temp=0.5, hum=0.5, light=0.5, amp=1;

	var baseFreq = temp.linexp(0,1,220,880);
	var note = baseFreq.cpsmidi;
	var snd, snd2;
	var notes = [
		[5,7,4,2,0,12,7,5,7,4,2,0],
		[5,7,4,2,0,12,4,7,5,0],
		[-5,2,0,4,7,12,5,2,7,4,0,7,2,5,5,2,4,0],
		[7,7,2,4,4,4,2,0,7,0,0],
	];
	var chords = [
		[0,4,7,12],
		[4,7,11,16],
		[-3,0,4,7],
		[-3,0,5,9],
	];
	var seq, seqnote, notepulse, noterate;
	var chords, planeenv, planenotechange;
	var noise_env, noise, delaytime, string, pianosnd, pianosnd2;

	// Pianist sound setup
	planenotechange = Dust.kr(hum.linlin(0,1,1/60,1/10));
	planeenv = 1 - EnvGen.kr(Env.perc(3,10,0.9), planenotechange);
	chords = Demand.kr(TDelay.kr(planenotechange,3),0,Dxrand(chords,inf));

	notepulse = 1;
	noterate = TChoose.kr(Dust.kr(notepulse)+Impulse.kr(0),[0.02,0.05,1,2,0.5,0.25,2]/2)*Rand(0.78,1.32);
	notepulse = Impulse.kr(noterate);
	seq = Demand.kr(Dust.kr(0.1)+Impulse.kr(0),0,Dxrand(notes,inf));
	seqnote = Demand.kr(notepulse,0,Dseq(seq,inf));

	// Bass note synthesis
	snd = SinOsc.ar((note-24).midicps,mul:SinOsc.kr(light.linlin(0,1,2,4)));
	snd = snd + SinOsc.ar((note-12).midicps,mul:SinOsc.kr(light.linlin(0,1,0.001,0.01)).range(0.001,0.05));

	// Chord synthesis with envelopes
	snd = snd + (planeenv * Mix.ar(Array.fill(8,{arg i;
		var chord;
		chord = Saw.ar((note + chords[i%4] + Select.kr(DC.kr(i%4) < 1, [24.neg, 36.neg])).midicps, mul: 0.9);
		chord = LPF.ar(chord, LinExp.kr(SinOsc.kr(rrand(1/30,1/10), rrand(0, 2*pi)), -1, 1, baseFreq, baseFreq * 5));
		chord = DelayC.ar(chord, rrand(0.01, 0.03), LFNoise1.kr(Rand(5, 10), 0.01, 0.02) / 15);
		Pan2.ar(chord, VarLag.kr(LFNoise0.kr(1/3), 3, warp:\sine)) / 7
	})));

	// Noise and string setup
	noise_env = Decay2.ar(Impulse.ar(noterate));
	noise = LFNoise2.ar(light.linlin(0, 1, 2000, 5000)) * noise_env;

	// Delay time calculation
	delaytime = 1.0 / ((note + seqnote - 12).midicps);
	string = Mix.new(CombL.ar(noise, delaytime, delaytime, 6.0));
	pianosnd = RLPF.ar(string, 2 * ((note + seqnote - 12).midicps), 4.0) * amp;
	pianosnd = HPF.ar(pianosnd, 40);

	// Additional sound synthesis
	pianosnd2 = EnvGen.ar(Env.perc(release: 4), notepulse) * MoogFF.ar(Pulse.ar((note + seqnote).midicps, mul: 1.0), (note).midicps * 1.5);
	snd = snd + SelectX.ar(SinOsc.kr(LFNoise0.kr(0.1).range(0.01, 0.1)).range(0.1, 0.9), [pianosnd * 0.3, pianosnd2]);

	// Further processing
	snd = LPF.ar(snd, (note + 36).midicps);
	snd = HPF.ar(snd, 120);
	snd = snd * EnvGen.ar(Env.new([0, 0, 1], [0.5, 3]));
	snd = FreeVerb.ar(snd, 0.45, 2.0, 0.5);

	// Reverb and delay
	snd2 = DelayN.ar(snd, 0.03, 0.03);
	snd2 = CombN.ar(snd2, 0.1, { Rand(0.01, 0.099) }!32, 4);
	snd2 = SplayAz.ar(2, snd2);
	snd2 = LPF.ar(snd2, 1500);
	5.do { snd2 = AllpassN.ar(snd2, 0.1, { Rand(0.01, 0.099) }!2, 3) };

	// Final output
	Out.ar(0, (snd2 * 0.1 + snd) * amp);
}).add;
)


(
// Gestione simulazione dati Arduino + sintesi
var synth;

s.waitForBoot({
    // Creo il synth una volta sola
    synth = Synth(\Eno_v2, [
        \temp,  0.5,
        \hum,   0.5,
        \light, 0.5
    ]);

    // Routine che simula stringhe di Arduino
    Routine({
        var rawTemp, rawHum, rawLight;
        var normTemp, normHum, normLight;
        var tempT2, humT15, lightLog;
        var tempNoise, humNoise, lightNoise;
        var parsedString;

        var counter = 0;

        loop({
            if(counter % 10 == 0) {
                rawTemp  = rrand(20.0, 60.0); //-10 - 60
                rawHum   = rrand(20.0, 80.0); //0 - 100
            };

            rawLight = rrand(1, 4095);

            // Normalizzazione corretta (nota che range corretto ora è 20-60 temp, 20-80 hum, 1-4095 light)
            normTemp  = (rawTemp - 20) / (60 - 20);
            normHum   = (rawHum - 20) / (80 - 20);
            normLight = (rawLight - 1) / (4095 - 1);

            tempT2    = normTemp.squared;
            humT15    = normHum.pow(1.5);
            lightLog  = (log(normLight * 9 + 1)) / log(10);

            tempNoise  = normTemp + gauss(0, 0.02);
            humNoise   = normHum  + gauss(0, 0.02);
            lightNoise = normLight + gauss(0, 0.02);

            parsedString = [
                rawTemp, rawHum, rawLight,
                normTemp, normHum, normLight,
                //tempT2, humT15, lightLog,
                //tempNoise, humNoise, lightNoise
            ];
            ("DATA," ++ parsedString.collect({ |x| x.round(0.001) }).join(",")).postln;

            // Qui aggiorniamo i valori ambientali trasformati
            synth.set(
                \temp,  parsedString[3].clip(0, 1),
                \hum,   parsedString[4].clip(0, 1),
                \light, parsedString[5].clip(0, 1)
            );

            counter = counter + 1;
            0.2.wait;
        });
    }).play(AppClock);
});
)






(
SynthDef(\Eno_v2, { |temp=0.5, hum=0.5, light=0.5, amp=0.5, out=0|

	// bass note variable
	var freqs, modFreq, bassSig, step, baseFreq, note, numOsc;

	// chords variable
	var chords, chordSig, numVoices, trigger, chord;

	//note variable
	var notes, seq, seqnote, noterate, notepulse, notesSig;

	var sig;


	// ========== BASS NOTE SYNTHESIS ========== //

	numOsc = 5;        // numero di oscillatori sinusoidali per il basso
	step = 7;          // distanza tra oscillatori in semitoni
	baseFreq = temp.linlin(0, 1, 220, 660); // temperatura mappata in Hz: 0->220Hz, 1->660Hz
	note = baseFreq.cpsmidi;               // nota MIDI corrispondente

		// LFO modulation rate (light = 0 → veloce, light = 1 → lento)
	modFreq = light.linlin(0, 1, 12, 0);  // es. 0 = buio → 12 Hz (battimenti veloci), 1 = luce → 0 Hz (no battimenti)

	// Crea array di frequenze separate da `step` semitoni
	freqs = Array.fill(numOsc, { |i| (note + (i * step)).midicps });

	// Somma oscillatori con ampiezza modulata da un LFO in base alla luce
	bassSig = Mix.fill(numOsc, { |i|
		var lfo = SinOsc.kr(modFreq);
		SinOsc.ar(freqs[i], 0, lfo * (amp / numOsc))
	});

	// ========== CHORDS ========== //

	// 4 insiemi armonici
	chords = [
		[0, 4, 7, 12],
		[4, 7, 11, 16],
		[-3, 0, 4, 7],
		[-3, 0, 5, 9],
	];

	// Numero di voci (multiplo di 4)
	numVoices = 4;

	trigger = Impulse.kr(TRand.kr(1/60, 1, Dust.kr(1))); // nuova frequenza (tra 1/60 e 1 Hz) ogni trigger (1 secondo)

	// Estrazione di un accordo casuale dalla lista ogni 1-60 secondi
	chord = Demand.kr(trigger, 0, Dxrand(chords, inf));

	// Generazione del suono d’accordo (senza inviluppi, né effetti)
	chordSig = Mix.ar(Array.fill(numVoices, { |i|
		var pitch = note + chords[i % 4] + [0, -12, -24, -36][i % 4]; //voci distanziate di un'ottava
		var freq = pitch.midicps;
		Saw.ar(freq, 0.1 /numVoices) // Semplice onda dente di sega
	}));

	// === NOTE MELODICHE ===

	// 4 sequenze melodiche (Eno-style)
	notes = [
		[5, 7, 4, 2, 0, 12, 7, 5, 7, 4, 2, 0],
		[5, 7, 4, 2, 0, 12, 4, 7, 5, 0],
		[-5, 2, 0, 4, 7, 12, 5, 2, 7, 4, 0, 7, 2, 5, 5, 2, 4, 0],
		[7, 7, 2, 4, 4, 4, 2, 0, 7, 0, 0],
	];

	// ritmo casuale (tra una nota e l'altra 0.25 secondi, 0.5 sec)
	noterate = TChoose.kr(Dust.kr(1), [0.25, 0.5, 1, 2]);
	notepulse = Impulse.kr(noterate);

	// seleziona una sequenza e poi emette note da essa
	seq = Demand.kr(Impulse.kr(0.1), 0, Dxrand(notes, inf));

	// seleziona una nota ogni notepulse
	seqnote = Demand.kr(notepulse, 0, Dseq(seq, inf));

	// converte la nota midi in frequenza
	notesSig = SinOsc.ar((note + seqnote).midicps, 0, 0.15); //Somma note (frequenza di base, dipendente da temp) e seqnote (intervallo selezionato dalla sequenza).

	// ========== MIX BASS + CHORDS ========== //
	sig = (chordSig*0.5) + (bassSig*0.2) + (notesSig*0.2);


	// ========== OUTPUT ========== //
	Out.ar(out, sig*2);
}).add;
)












(
SynthDef(\Eno_v1, { |temp=0.5, hum=0.5, light=0.5, amp=0.5, out=0|


	// Bass note synthesis
	var freqs, modFreq, sig;
	var numOsc = 4;
	var step = 6;
	var baseFreq = temp.linlin(0,1,440,880);
	var note = baseFreq.cpsmidi;

	// LFO modulation rate (light = 0 → veloce, light = 1 → lento)
	modFreq = light.linlin(0, 1, 8, 0);  // es. 0 = buio → 12 Hz (battimenti veloci), 1 = luce → 0 Hz (no battimenti)
	//sarebbe bello fare buio battimenti lenti (0.1) e luce battimenti vleoci (12) e metttere una soglia che poi switcha a o e non ci sono battimetni

	// Crea array di frequenze distanziate tra loro di `step` semitoni
	freqs = Array.fill(numOsc, { |i|
		(note + (i * step)).midicps
	});

	// Somma oscillatori con ampiezza modulata da LFO dipendente da light
	sig = Mix.fill(numOsc, { |i|
		var lfo = SinOsc.kr(modFreq);  // ampiezza modulata
		SinOsc.ar(freqs[i], 0, lfo * (amp / numOsc))
	});

	Out.ar(out, Pan2.ar(sig));
}).add;
)






