#############################################################

# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
TARGET = eagle
#FLAVOR = release
FLAVOR = debug

#EXTRA_CCFLAGS += -u

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)
SUBDIRS=    \
	user 

endif # } PDIR
BINDIR = ./bin
APPDIR = .
LDDIR = $(SDK_PATH)/ld

CCFLAGS += -Os

TARGET_LDFLAGS =		\
	-nostdlib		\
	-Wl,-EL \
	--longcalls \
	--text-section-literals

ifeq ($(FLAVOR),debug)
    TARGET_LDFLAGS += -g -O2
endif

ifeq ($(FLAVOR),release)
    TARGET_LDFLAGS += -g -O0
endif

COMPONENTS_eagle.app.v6 = \
	user/libuser.a 

LINKFLAGS_eagle.app.v6 = \
	-L$(SDK_PATH)/lib        \
	-nostdlib	\
    -T$(LD_FILE)   \
	-Wl,--no-check-sections	\
	-Wl,--gc-sections	\
    -u call_user_start	\
	-Wl,-static						\
	-Wl,--start-group					\
	-lc					\
	-lgcc					\
	-lphy	\
	-lpp	\
	-lnet80211	\
	-llwip	\
	-lwpa	\
	-lcrypto	\
	-lmain	\
	-lupgrade\
	-ldriver \
	-lhal					\
	$(DEP_LIBS_eagle.app.v6)					\
	-Wl,--end-group

#	-ljson	\
#	-lsmartconfig \
#	-lmbedtls	\
#	-lpwm	\
	
DEPENDS_eagle.app.v6 = \
                $(LD_FILE) \
                $(LDDIR)/eagle.rom.addr.v6.ld

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!  # Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
CONFIGURATION_DEFINES =	-DICACHE_FLASH \
                        -DGLOBAL_DEBUG_ON

DEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)

DDEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

INCLUDES := $(INCLUDES) \
	-I $(PDIR)include 

PDIR = $(SDK_PATH)/
sinclude $(SDK_PATH)/Makefile

PORT = /dev/ttyUSB0
HOST := 192.168.8.150

BAUDRATE = 115200
FLASH_BAUDRATE = 115200

ESPTOOL = esptool.py --baud $(FLASH_BAUDRATE) --port $(PORT)
ESPTOOL_WRITE = $(ESPTOOL)  write_flash -u --flash_mode qio --flash_freq 40m


.PHONY: map6
map6:
	make clean
	make COMPILE=gcc BOOT=new APP=0 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=6


.PHONY: map6user1
map6user1:
	make clean
	make COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=6


.PHONY: map6user2
map6user2:
	make clean
	make COMPILE=gcc BOOT=new APP=2 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=6


.PHONY: fota 
fota: map6user2
	-curl $(HOST)/firmware -XPOST -F"firmware=@$(BINDIR)/upgrade/user2.4096.new.6.bin"
	-echo

.PHONY: screen
screen:
	@screen $(PORT) $(BAUDRATE)


.PHONY: erase_flash
erase_flash:
	 $(ESPTOOL) erase_flash


.PHONY: assets_map2user1
assets_map2user1:
	$(ESPTOOL_WRITE) --flash_size 4MB-c1  \
		0x77000 assets/favicon-16x16.png

flash_map2user1: map2user1
	$(ESPTOOL_WRITE) --flash_size 1MB  \
		0x0 	$(SDK_PATH)/bin/boot_v1.7.bin \
		0x1000  $(BINDIR)/upgrade/user1.1024.new.2.bin \
		0xfc000 $(SDK_PATH)/bin/esp_init_data_default_v08.bin \
		0xfb000 $(SDK_PATH)/bin/blank.bin \
		0xfe000 $(SDK_PATH)/bin/blank.bin

.PHONY: flash_map6user1
flash_map6user1: map6user1
	$(ESPTOOL_WRITE) --flash_size 4MB-c1  \
		0x0 	 $(SDK_PATH)/bin/boot_v1.7.bin \
		0x1000   $(BINDIR)/upgrade/user1.4096.new.6.bin \
		0x3fc000 $(SDK_PATH)/bin/esp_init_data_default_v08.bin \
		0x3fb000 $(SDK_PATH)/bin/blank.bin \
		0x3fe000 $(SDK_PATH)/bin/blank.bin 

.PHONY: cleanup_map6user1
cleanup_map6user1:
	$(ESPTOOL_WRITE) --flash_size 4MB-c1  \
		0xf9000 $(SDK_PATH)/bin/blank.bin \
		0xfa000 $(SDK_PATH)/bin/blank.bin 

.PHONY: cleanup_map6user1_params
cleanup_map6user1_params:
	$(ESPTOOL_WRITE) --flash_size 4MB-c1  \
		0xf8000 $(SDK_PATH)/blank.bin \
		0xf9000 $(SDK_PATH)/blank.bin \
		0xfa000 $(SDK_PATH)/blank.bin 

.PHONY: assets_map6user1
assets_map6user1:
	$(ESPTOOL_WRITE) --flash_size 4MB-c1  \
		0x200000 assets/favicon-16x16.png 

.PHONY: erase_flash
earase_flash:
	$(ESPTOOL) earase_flash


