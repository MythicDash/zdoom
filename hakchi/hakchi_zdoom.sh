#!/bin/sh

#Load in the console's enviornment variables
source /etc/preinit
script_init

zDOOMTrueDir="$(dirname `readlink -f "$0"`)"
HOME="/var/saves/CLV-Z-HAKCHI_ZDOOM"
wad="$(ls "$zDOOMTrueDir/zDOOM_files" | grep -i "doom.wad")"

uistop

#Clean down and overcommit memory
echo "1" > "/proc/sys/vm/overcommit_memory"
echo "3" > "/proc/sys/vm/drop_caches"

dd if=/dev/zero of=/dev/fb0 #Clear FB just in case...

if [ ! -z "$wad" ]; then
 
  decodepng "$zDOOMTrueDir/Hakchi_zDOOM_assets/HRlogo-min.png" > /dev/fb0
  sleep 1
  decodepng "$zDOOMTrueDir/Hakchi_zDOOM_assets/zDoomcred-min.png" > /dev/fb0
  sleep 2
  decodepng "$zDOOMTrueDir/Hakchi_zDOOM_assets/zDoomsplash-min.png" > /dev/fb0
  
  #Load in the extra libraries required to run on (S)NESC
  export LD_LIBRARY_PATH="$zDOOMTrueDir/lib:$LD_LIBRARY_PATH"

  #Create/copy files and folders if needed
  mkdir -p "$HOME/.config/zdoom"
  
  [ -f "$HOME/save.sram" ] || touch "$HOME/save.sram" #To prevent save state manager to wipe the saves/configs
  [ -f "$HOME/.config/zdoom/zdoom.ini" ] || cp "$zDOOMTrueDir/zDOOM_files/zdoom.ini" "$HOME/.config/zdoom/"
  [ -f "$HOME/.config/zdoom/autoexec.cfg" ] || cp "$zDOOMTrueDir/zDOOM_files/autoexec.cfg" "$HOME/.config/zdoom/"
  
  chmod +x "$zDOOMTrueDir/zDOOM_files/zdoom"
  cd "$zDOOMTrueDir/zDOOM_files"

  ./zdoom -iwad $wad -file brutalv20b_hakchi.pk3 IDKFAv2.wad
    
fi

sleep 1
uistart
