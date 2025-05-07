s;
s.boot;

(
    // Configura la porta seriale
    ~port = SerialPort.new("/dev/tty.usbmodem1411", 9600); // Assicurati che il nome della porta sia corretto per il tuo sistema

    // Variabili per memorizzare i dati
    ~temp = 0.5;
    ~hum = 0.5;
    ~light = 0.5;

    // Routine per ricevere i dati
    ~charArray = [];
    ~getValues = Routine.new({
        var ascii;
        {
            ascii = ~port.read.asAscii;
            if(ascii.isDecDigit, { ~charArray = ~charArray.add(ascii); });

            if(ascii == $a, {
                // Converte i dati in numeri
                ~val1 = ~charArray.collect(_.digit).convertDigits;
                ~charArray = [];
            });

            if(ascii == $b, {
                ~val2 = ~charArray.collect(_.digit).convertDigits;
                ~charArray = [];
            });

		    if(ascii == $c, {
                ~val3 = ~charArray.collect(_.digit).convertDigits;
                ~charArray = [];
            });
        }.loop;
    }).play;
)

// Esegui il synth, passando i parametri
(
SynthDef(\EnoAmbient, {
	arg temp=0.5, hum=0.5, light=0.5, amp=0.5;

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
	var planenotes, planeenv, planenotechange;
	var noise_env, noise, delaytime, string, pianosnd, pianosnd2;

	// Pianist sound setup
	planenotechange = Dust.kr(hum.linlin(0,1,1/60,1/10));
	planeenv = 1 - EnvGen.kr(Env.perc(3,10,0.9), planenotechange);
	planenotes = Demand.kr(TDelay.kr(planenotechange,3),0,Dxrand(chords,inf));

	notepulse = 1;
	noterate = TChoose.kr(Dust.kr(notepulse)+Impulse.kr(0),[0.02,0.05,1,2,0.5,0.25,2]/2)*Rand(0.78,1.32);
	notepulse = Impulse.kr(noterate);
	seq = Demand.kr(Dust.kr(0.1)+Impulse.kr(0),0,Dxrand(notes,inf));
	seqnote = Demand.kr(notepulse,0,Dseq(seq,inf));

	// Bass note synthesis
	snd = SinOsc.ar((note-24).midicps,mul:SinOsc.kr(light.linlin(0,1,0.001,0.01)).range(0.05,0.15));
	snd = snd + SinOsc.ar((note-12).midicps,mul:SinOsc.kr(light.linlin(0,1,0.001,0.01)).range(0.001,0.05));

	// Chord synthesis with envelopes
	snd = snd + (planeenv * Mix.ar(Array.fill(8,{arg i;
		var chord;
		chord = Saw.ar((note + planenotes[i%4] + Select.kr(DC.kr(i%4) < 1, [24.neg, 36.neg])).midicps, mul: 0.9);
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

 ~synth = Synth("EnoAmbient");
{SinOsc.ar(440)}.play;

// Esegui il synth e aggiorna i parametri
(~control = Routine.new({ {
                ~synth.set(\temp,  ~val1.linexp(-10, 60, 0, 1),  // Mappatura della temperatura nel range 0–1
                \hum,   ~val2.linlin(0, 100, 0, 1),   // Mappatura dell'umidità nel range 0–1
                \light, ~val3.linlin(1, 4095, 0, 1)   // Mappatura della luce nel range 0–1
            );
            0.2.wait;  // Aspetta 2 secondi prima di aggiornare nuovamente i dati
}.loop;}
    ).play;
)