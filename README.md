# G.A.I.A. - CMLS Project 2025
G.A.I.A. (Generative Audio via Interactive Atmosphere) is an ambient synth that takes its data both from the environment and from the user interaction.
This synth was created as the final project for the Computer Music: Languages and Systems course of the master degree Acoustic Engineering of Politecnico di Milano.
Because of this, it involves most of the technologies explained during the course, adding also some extra that were needed to make the project work.
Here is the list of every technology involved:
- Arduino: collects real world data (temperature, humidity and luminosity) thanks to sensors;
- Processing: creates a user interface that allows interaction, controls audio effects;
- SuperCollider: generates ambiental sound;
- JUCE: applies effects to the sound and reproduces it;
- Visual Studio 2022: used to run JUCE projects;
- [VCable](https://vb-audio.com/Cable/?gad_source=1&gad_campaignid=266017394&gbraid=0AAAAADjKyE4mWRWFgCjUTKuyoref436kI&gclid=CjwKCAjw_pDBBhBMEiwAmY02NrUUqg9d52OqmtTlAULf3YHqZgk615ma1muYRhYF2BlX35O2aDD2JhoCdJwQAvD_BwE) / BlackHole: tool that depends on the OS that is running on the user machine, allows the audio transfer from SuperCollider to JUCE;
- OSC: protocol that sends the data collected by Arduino to SuperCollider and the data changed by the user to JUCE.

[You can find a scheme that resumes the workflow of the application by clicking on this link.](https://github.com/Ciorpy/CMLS-2025/blob/main/Documents/workflow.pdf)

## Credits
- Marco Porcella marco.porcella@mail.polimi.it
- Filippo Longhi filippo1.longhi@mail.polimi.it
- Andrea Crisafulli andrea.crisafulli@mail.polimi.it
- Giacomo De Toni giacomo1.detoni@mail.polimi.it
