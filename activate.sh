
if [ $0 = $BASH_SOURCE ]; then
	echo "Can not run this script"
	exit 1
fi

if [ ! -z $BACKUP_PATH ]; then
	echo "Already sourced, run deactivate to allow source again."
	return 1	
fi

echo "SDK: $SDK_PATH"
echo "BIN: $BIN_PATH"
echo "ENV: $ENV_TITLE"

if [ -z "$SDK_PATH" ] || [ -z "$BIN_PATH" ] || [ -z "$ENV_TITLE" ]; then
	echo "Please set SDK_PATH, ENV_TITLE and BIN_PATH"
	return 1
fi

HERE=`dirname "$(readlink -f "$BASH_SOURCE")"`
export BACKUP_PATH=$PATH
export BACKUP_PS1=$PS1
#export SDK_PATH="$HERE/ESP8266_NONOS_SDK"
#export BIN_PATH="$here/bin"
export PATH="$HERE/xtensa-toolchain/bin:$PATH"
export PS1="($ENV_TITLE) $PS1"

function deactivate {
	export PATH=$BACKUP_PATH
	export PS1=$BACKUP_PS1
	unset BACKUP_PATH
	unset BACKUP_PS1
	unset SDK_PATH
	unset BIN_PATH
	unset ENV_TITLE 
	unset -f deactivate
}

