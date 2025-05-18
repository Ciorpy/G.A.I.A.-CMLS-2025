s.reboot; // rebooting server

(
// Raw values
~tempRaw = 0;
~humRaw = 0;
~lightRaw = 0;

// Normalized values (for synth)
~temp = 0.5;
~hum = 0.5;
~light = 0.5;

OSCdef(\temp, { |msg|
	~tempRaw = msg[1];
	~temp = ~tempRaw.linlin(2000, 3000, 0.0, 1.0).clip(0.0, 1.0);
}, "/sensors/temp");

OSCdef(\hum, { |msg|
	~humRaw = msg[1];
	~hum = ~humRaw.linlin(3000, 8000, 0.0, 1.0).clip(0.0, 1.0);
}, "/sensors/humi");

OSCdef(\light, { |msg|
	~lightRaw = msg[1];
	~light = ~lightRaw.linlin(0, 4095, 0.0, 1.0).clip(0.0, 1.0);
}, "/sensors/rldr");
)


(
~reader = Routine({
	loop {
		~synth.set(
			\temp, ~temp,
			\hum, ~hum,
			\light, ~light
		);
		("Temp: " ++ (~tempRaw / 100).round(0.01) ++ " °C"
		++ " | Hum: " ++ (~humRaw / 100).round(0.01) ++ " %"
		++ " | Light: " ++ ~lightRaw).postln;
		0.2.wait;
	}
});
)


(// Define and play synth
SynthDef(\EnoAmbient, {
	arg temp=0.5, hum=0.5, light=0.5, amp=0.5;
	var baseFreq;
	var note;
	var noterate;
	var notepulse;
	var seqnote;
	var seq;
	var chords;
	var notes;
	var planenotechange;
	var planeenv;
	var planenotes;
	var snd;
	var pianosnd;
	var pianosnd2;
	var noise_env;
	var noise;
	var delaytime;
	var string;
	var snd2;

	baseFreq = temp.linexp(0,1,400,2000);
	note = baseFreq.cpsmidi;
	chords = [[0,4,7,12],[4,7,11,16],[-3,0,4,7],[-3,0,5,9]];
	notes = [
		[5,7,4,2,0,12,7,5,7,4,2,0],
		[5,7,4,2,0,12,4,7,5,0],
		[-5,2,0,4,7,12,5,2,7,4,0,7,2,5,5,2,4,0],
		[7,7,2,4,4,4,2,0,7,0,0]
	];

	planenotechange = Dust.kr(hum.linlin(0,1,1/60,1/10));
	planeenv = 1 - EnvGen.kr(Env.perc(3,10,0.9), planenotechange);
	planenotes = Demand.kr(TDelay.kr(planenotechange,3),0,Dxrand(chords,inf));

	noterate = TChoose.kr(Dust.kr(1)+Impulse.kr(0),[0.02,0.05,1,2,0.5,0.25,2]/2)*Rand(0.78,1.32);
	notepulse = Impulse.kr(noterate);
	seq = Demand.kr(Dust.kr(0.1)+Impulse.kr(0),0,Dxrand(notes,inf));
	seqnote = Demand.kr(notepulse,0,Dseq(seq,inf));

	snd = SinOsc.ar((note-24).midicps,mul:SinOsc.kr(light.linlin(0,1,0.001,0.01)).range(0.05,0.15))
	    + SinOsc.ar((note-12).midicps,mul:SinOsc.kr(light.linlin(0,1,0.001,0.01)).range(0.001,0.05));

	snd = snd + (planeenv * Mix.ar(Array.fill(8,{ |i|
		var chord = Saw.ar((note + planenotes[i%4] + Select.kr(DC.kr(i%4) < 1, [24.neg, 36.neg])).midicps, mul: 0.9);
		chord = LPF.ar(chord, LinExp.kr(SinOsc.kr(rrand(1/30,1/10), rrand(0, 2*pi)), -1, 1, baseFreq, baseFreq * 5));
		chord = DelayC.ar(chord, rrand(0.01, 0.03), LFNoise1.kr(Rand(5, 10), 0.01, 0.02) / 15);
		Pan2.ar(chord, VarLag.kr(LFNoise0.kr(1/3), 3, warp:\sine)) / 7
	})));

	noise_env = Decay2.ar(Impulse.ar(noterate));
	noise = LFNoise2.ar(light.linlin(0, 1, 2000, 5000)) * noise_env;
	delaytime = 1.0 / ((note + seqnote - 12).midicps);
	string = Mix.new(CombL.ar(noise, delaytime, delaytime, 6.0));
	pianosnd = RLPF.ar(string, 2 * ((note + seqnote - 12).midicps), 4.0) * amp;
	pianosnd = HPF.ar(pianosnd, 40);

	pianosnd2 = EnvGen.ar(Env.perc(0.01, 4), notepulse)
		      * MoogFF.ar(Pulse.ar((note + seqnote).midicps, mul: 1.0), (note).midicps * 1.5);

	snd = snd + SelectX.ar(SinOsc.kr(LFNoise0.kr(0.1).range(0.01, 0.1)).range(0.1, 0.9), [pianosnd * 0.3, pianosnd2]);

	snd = LPF.ar(snd, (note + 36).midicps);
	snd = HPF.ar(snd, 120);
	snd = snd * EnvGen.ar(Env.new([0, 0, 1], [0.5, 3]));
	snd = FreeVerb.ar(snd, 0.45, 2.0, 0.5);

	snd2 = DelayN.ar(snd, 0.03, 0.03);
	snd2 = CombN.ar(snd2, 0.1, { Rand(0.01, 0.099) }!32, 4);
	snd2 = SplayAz.ar(2, snd2);
	snd2 = LPF.ar(snd2, 1500);
	5.do { snd2 = AllpassN.ar(snd2, 0.1, { Rand(0.01, 0.099) }!2, 3) };

	Out.ar(0, (snd2 * 0.1 + snd) * amp);
}).add;
)

~reader.play;
~synth = Synth(\EnoAmbient);
