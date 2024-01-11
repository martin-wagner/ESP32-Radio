// Default preferences in raw data format for PROGMEM
//
#define defaultprefs_version 1808016
const char defprefs_txt[] PROGMEM = R"=====(
# Example configuration
# Programmable input pins:
#gpio_00 = uppreset = 1
#gpio_12 = upvolume = 2
#gpio_13 = downvolume = 2
#gpio_14 = stop
#gpio_17 = resume
#gpio_34 = station = icecast.omroep.nl:80/radio1-bb-mp3
#
# MQTT settings
mqttbroker = none
mqttport = 1883
mqttuser = none
mqttpasswd = none
mqqprefix = none
# Enter your WiFi network specs here:
wifi_00 = SSID1/PASSWD1
#wifi_01 = SSID2/PASSWD2
#
volume = 100
toneha = 0
tonehf = 0
tonela = 0
tonelf = 0
#
preset = 1
preset_00 = edge67.streamonkey.net/gong-live/stream/mp3?aggregator=user #  0 - Radio Gong Würzburg
preset_01 = dispatcher.rndfnk.com/br/puls/live/mp3/mid          #  1 - BR Puls
preset_02 = stream.srg-ssr.ch/drsvirus/mp3_128.m3u              #  2 - SRF Virus
preset_03 = mp3ad.egofm.c.nmdn.net/egofm_128/livestream.mp3?    #  3 - Ego FM
preset_04 = stream.sunshine-live.de/live/mp3-192         #  4 - Sunshine Live
preset_05 = media-sov.musicradio.com:80/GoldMP3                 #  5 - Gold UK
preset_06 = liveradio.swr.de/d9zadj3/dasding/                   #  6 - SWR Dasding
preset_07 = mp3ad.egofm.c.nmdn.net/egofmpure_128/livestream.mp3? #  7 - Ego FM Pure
preset_08 = stream.sunshine-live.de/house/mp3-192/stream.sunshine-live.de/ #  8 - Sunshine Live House
preset_09 = stream.sunshine-live.de/classics/mp3-192/stream.sunshine-live.de/ # 9 - Sunshine Live Classics
preset_10 = airspectrum.cdnstream1.com:8114/1648_128            # 10 - Easy Hits Florida 128k
preset_11 = airspectrum.cdnstream1.com:8000/1261_192            # 11 - Magic Oldies Florida
preset_12 = icecast.omroep.nl:80/radio1-bb-mp3                  # 12 - Radio 1, NL

#
# Clock offset and daylight saving time
clk_server = pool.ntp.org                            # Time server to be used
clk_offset = 1                                       # Offset with respect to UTC in hours
clk_dst = 1                                          # Offset during daylight saving time (hours)
# Some IR codes
ir_40BF = upvolume = 2
ir_C03F = downvolume = 2
# GPIO pinnings
pin_ir = 35                                          # GPIO Pin number for IR receiver VS1838B
#
pin_sd_cs = 22                                       # GPIO Pin number for SD card "CS"
#
pin_vs_cs = 5                                        # GPIO Pin number for VS1053 "CS"
pin_vs_dcs = 32                                      # GPIO Pin number for VS1053 "DCS"
pin_vs_dreq = 4                                      # GPIO Pin number for VS1053 "DREQ"
)=====" ;
