HERE=`dirname "$(readlink -f "$BASH_SOURCE")"`
export SDK_PATH="$HERE/ESP8266_NONOS_SDK"
export BIN_PATH="$HERE/bin"
export ENV_TITLE=`basename "$HERE"`
mkdir -p "$HERE/bin"
source $HERE/../activate.sh

