#!/bin/sh

#Load in the console's enviornment variables
source /etc/preinit
script_init

zDOOMTrueDir="$(dirname `readlink -f "$0"`)"
HOME="/var/saves/CLV-Z-HAKCHI_BRUTAL"

while [ $# -gt 0 ]; do
  touch /tmp/vars
  echo $1 >> /tmp/vars
  [ "$1" == "--reset-config" ] && cmd_reset=1 #Resets to stock bundled zdoom.ini
  [ "$1" == "--force-IDKFA-OST" ] && cmd_idkfa=1 #Force load of IDKFA OST
  [ "$1" == "--show-fps" ] && cmd_fps=1 #Display FPS in top right
  [ "$1" == "--show-pos" ] && cmd_pos=1 #Show player position in HUD
  [ "$1" == "--mp-name" ] && cmd_mpname="$2" #Set user's player name for MP
  [ "$1" == "--load-base-WAD" ] && cmd_basewad="$2" #Load specific base WAD 
  shift
done

echo "-----------" >> /tmp/vars

#Process command line parameters
[ ! -z $cmd_reset ] && cp -f "$zDOOMTrueDir/zDOOM_files/zdoom.ini" "$HOME/.config/zdoom/"
[ ! -z $cmd_fps ] && extra_params="$extra_params +vid_fps 1"
[ ! -z $cmd_pos ] && extra_params="$extra_params +idmypos 1"
[ ! -z $cmd_mpname ] && extra_params="$extra_params +name \"$cmd_mpname\""
[ ! -z $cmd_idkfa ] && idkfa="IDKFAv2.wad"

#Process base WAD (load defined WAD or load doom.wad as default)
[ ! -z $cmd_basewad ] && wad="$(ls "$zDOOMTrueDir/zDOOM_files" | grep -i "$cmd_basewad")" || wad="$(ls "$zDOOMTrueDir/zDOOM_files" | grep -i "doom.wad")" && idkfa="IDKFAv2.wad"

touch /tmp/extra
echo "-----------" >> /tmp/extra
echo $extra_params >> /tmp/extra


if [ ! -z "$wad" ]; then
 
  #Leave the below images for legal and licensing.
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
  [ -f "$HOME/.config/zdoom/zdoom.ini" ] || cp -f "$zDOOMTrueDir/zDOOM_files/zdoom.ini" "$HOME/.config/zdoom/"
  [ -f "$HOME/.config/zdoom/autoexec.cfg" ] || cp -f "$zDOOMTrueDir/zDOOM_files/autoexec.cfg" "$HOME/.config/zdoom/"
  
  chmod +x "$zDOOMTrueDir/zDOOM_files/zdoom"
  cd "$zDOOMTrueDir/zDOOM_files"
  
  ./zdoom -iwad $wad -file brutalv20b_hakchi.pk3 $idkfa $extra_params
    
fi

uistart
