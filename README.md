# EnvoSYNTH - CMLS Project 2025
EnvoSYNTH is an ambient synth that takes its data both from the environment and from the user interaction.
This synth was created as the final project for the Computer Music: Languages and Systems course of the master degree Acoustic Engineering of Politecnico di Milano.
Because of this, it involves most of the technologies explained during the course, adding also some extra that were needed to make the project work.
Here is the list of every technology involved:
- Arduino: collects real world data (temperature, humidity and luminosity) thanks to sensors;
- Processing: creates a user interface that allows interaction, controls audio effects;
- SuperCollider: generates ambiental sound;
- JUCE: applies effects to the sound and reproduces it;
- Visual Studio 2022: used to run JUCE projects;
- VCable / BlackHole: tool that depends on the OS that is running on the user machine, allows the audio transfer from SuperCollider to JUCE;
- OSC: protocol that sends the data collected by Arduino to SuperCollider and the data changed by the user to JUCE.

[You can find a scheme that resumes the workflow of the application by clicking on this link.]()

## 
