
if [ $0 = $BASH_SOURCE ]; then
	echo "Can not run this script, try to source it"
	exit 1
fi

if [ ! -z $BACKUP_PATH ]; then
	echo "Already sourced, run deactivate to allow source again."
	return 1	
fi


HERE=`dirname "$(readlink -f "$BASH_SOURCE")"`
export BACKUP_PATH=$PATH
export BACKUP_PS1=$PS1
export BACKUP_SDK_PATH=$SDK_PATH
export BACKUP_BINDIR=$BINDIR
export BACKUP_ENV_TITLE=$ENV_TITLE
export ENVDIR=$HERE
export SDK_PATH=$HERE/sdk
export BINDIR="$HERE/bin"
export ENV_TITLE="esp-nonos"
TOOLCHAIN=$HERE/xtensa-toolchain/release/xtensa-toolchain/xtensa-lx106-elf
export PATH="$TOOLCHAIN/bin:$PATH"
export PS1="($ENV_TITLE) $PS1"


function esp.sc() {
	screen "/dev/ttyUSB$1" 115200
}

if [ ! -d "$HERE/bin" ]; then
	echo "$HERE/bin directory not exists, trying to create it"
	mkdir "$HERE/bin"
fi

echo "SDK: $SDK_PATH"
echo "BIN: $BINDIR"
echo "ENV: $ENV_TITLE"

function esp8266-newproject {
  $ENVDIR/new-project.sh $@
}
typeset -fx esp8266-newproject

function deactivate {
  export PATH=$BACKUP_PATH
  export PS1=$BACKUP_PS1
  export SDK_PATH=$BACKUP_SDK_PATH
  export BINDIR=$BACKUP_BINDIR
  export ENV_TITLE=$BACKUP_ENV_TITLE
  unset BACKUP_PATH
  unset BACKUP_PS1
  unset SDK_PATH
  unset BINDIR
  unset ENV_TITLE 
  unset ENVDIR 
  unset BACKUP_SDK_PATH
  unset BACKUP_BINDIR	
  unset BACKUP_ENV_TITLE
  unset -f deactivate
  unset -f esp.sc
  unset -f esp8266-newproject
}

