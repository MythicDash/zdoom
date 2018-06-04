#!/bin/sh

#Load in the console's enviornment variables
source /etc/preinit
script_init

# Kill it! Kill it with fire!
pkill -KILL clover-mcp

#Clear cache and inodes for good measure...
echo 3 > /proc/sys/vm/drop_caches

dd if=/dev/zero of=/dev/fb0 #Clear FB just in case...

WorkingDir=$(pwd)
GameName=$(echo $WorkingDir | awk -F/ '{print $NF}')
ok=0

if [ -f "/usr/share/games/$GameName/$GameName.desktop" ]; then
  zDOOMTrueDir=$(grep /usr/share/games/$GameName/$GameName.desktop -e 'Exec=' | awk '{print $2}' | sed 's/\([/\t]\+[^/\t]*\)\{1\}$//')
  ok=1
fi

if [ "$ok" == 1 ]; then

  decodepng "$zDOOMTrueDir/Hakchi_zDOOM_assets/zDoomsplash-min.png" > /dev/fb0;

  #Dynamically link the SDL2.0 library on the mini
  [ ! -L $zDOOMTrueDir/lib/libSDL2-2.0.so.0 ] && ln -sf "/usr/lib/libSDL2.so" "$zDOOMTrueDir/lib/libSDL2-2.0.so.0"

  #Load in the extra libraries required to run on SNESC
  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$zDOOMTrueDir/lib
  export LD_LIBRARY_PATH

  #Change the HOME environment variable for running on the mini...
  mkdir -p /var/lib/hakchi/rootfs/etc/zdoom
  export HOME="/var/lib/hakchi/rootfs/etc/zdoom"

  cd $zDOOMTrueDir/zDOOM_files

  $zDOOMTrueDir/zDOOM_files/zdoom -iwad doom.wad -file brutalv20b_hakchi.pk3 &> /media/zdoom.log #Please god just fucking work.

  echo 3 > /proc/sys/vm/drop_caches #Clear down after ourselves...

  /etc/init.d/S81clover-mcp start #Restart Clover UI and MCP
  
fi