# Hakchi zDOOM (With Brutal Doom)

This is a fork of zDoom with the addition of Hakchi specific stuff and patched Brutal Doom.

Technically we will break this repo out so people can use Hakchi zDOOM to load any pk3 they wish however the main objective is to make Brutal Doom run to begin with.

## Things to do / need fixing

- Add mapping for (S)NESC controller to the SDL2 input (Similar is required for Hakchi Quake)
- Fix ALSA sound (Haven't got a clue why it's not working)
- Check cMake to see if the optimisations are correct and actually working.
- Work out best configuration for it to run optimally on the mini

## How to build / run

- Run `cmake . `
- Copy compiled zdoom and zdoom.pk3 to `hakchi/zDOOM_files/`
- Put the doom.wad into `hakchi/zDOOM_files/`
- Zip all files within `hakchi/` as `CLV-Z-HAKCHI_ZDOOM`
- Install like a game into hakchi using the `CLV-Z-HAKCHI_ZDOOM.zip`

## Things to note.

- If you are running fat32. Don't. You can't sym link it seems and it's a bitch of an issue.

You need to comment the below out of the hakchi shell script:
```bash
#Dynamically link the SDL2.0 library on the mini (Doesn't work on FAT32! You will need to rip it from the mini instead)
[ ! -L $zDOOMTrueDir/lib/libSDL2-2.0.so.0 ] && ln -sf "/usr/lib/libSDL2.so" "$zDOOMTrueDir/lib/libSDL2-2.0.so.0"
```

And rip the SDL2 lib directly from the mini into the game lib folder with the correct filename.
(Why did they use non standard library names I don't know...)
