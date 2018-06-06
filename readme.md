# Hakchi zDOOM (With Brutal Doom)

This is a fork of zDoom with the addition of Hakchi specific stuff and patched Brutal Doom.

Technically we will break this repo out so people can use Hakchi zDOOM to load any pk3 they wish however the main objective is to make Brutal Doom run to begin with.

## Things to do / need fixing

- Add mapping for (S)NESC controller to the SDL2 input (Similar is required for Hakchi Quake)
- Fix ALSA sound (Haven't got a clue why it's not working)
- Check cMake to see if the optimisations are correct and actually working.
- Work out best configuration for it to run optimally on the mini

## How to build / run

- Run `cmake . `
- Copy compiled zdoom and zdoom.pk3 to `hakchi/zDOOM_files/`
- Put the doom.wad into `hakchi/zDOOM_files/`
- Zip all files within `hakchi/` as `CLV-Z-HAKCHI_ZDOOM`
- Install like a game into hakchi using the `CLV-Z-HAKCHI_ZDOOM.zip`

