#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EMBED_TXTFILES := ca_cert.pem

COMPONENT_SRCDIRS := . lib/Blinker lib/mQttClient lib/NVS lib/UpdateFirmware \
					lib/WiFi lib/GeneralUtils lib