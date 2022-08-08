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
# Some preset examples
preset_00 = edge67.streamonkey.net/gong-live/stream/mp3?aggregator=user #  0 - Radio Gong Würzburg
preset_01 = stream.live.vc.bbcmedia.co.uk/bbc_6music            #  1 - BBC 6 Music
preset_02 = dispatcher.rndfnk.com/br/puls/live/mp3/mid          #  2 - BR Puls
preset_03 = mp3ad.egofm.c.nmdn.net/egofm_128/livestream.mp3?    #  3 - Ego FM
preset_04 = sunshinelive.hoerradar.de/sunshinelive-live-mp3-hq  #  4 - Sunshine Live
preset_05 = media-sov.musicradio.com:80/GoldMP3                 #  5 - Gold UK
preset_06 = liveradio.swr.de/d9zadj3/dasding/                   #  6 - SWR Dasding
preset_07 = stream.live.vc.bbcmedia.co.uk/bbc_radio_one         #  7 - BBC 1
preset_08 = stream.live.vc.bbcmedia.co.uk/bbc_radio_two         #  8 - BBC 2
preset_09 = mp3ad.egofm.c.nmdn.net/egofmpure_128/livestream.mp3? #  9 - Ego FM Pure
preset_10 = sunshinelive.hoerradar.de/sunshinelive-house-mp3-hq #  10 - Sunshine Live House
preset_11 = sunshinelive.hoerradar.de/sunshinelive-classics-mp3-hq # 11 - Sunshine Live Classics
preset_12 = airspectrum.cdnstream1.com:8114/1648_128            # 12 - Easy Hits Florida 128k
preset_13 = airspectrum.cdnstream1.com:8000/1261_192            # 13 - Magic Oldies Florida
preset_14 = icecast.omroep.nl:80/radio1-bb-mp3                  # 14 - Radio 1, NL
preset_15 = stream.srg-ssr.ch/drsvirus/mp3_128.m3u              # 15 - SRF Virus

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
#pin_enc_clk = 25                                     # GPIO Pin number for rotary encoder "CLK"
#pin_enc_dt = 26                                      # GPIO Pin number for rotary encoder "DT"
#pin_enc_sw = 27                                      # GPIO Pin number for rotary encoder "SW"
#
#pin_tft_cs = 15                                      # GPIO Pin number for TFT "CS"
#pin_tft_dc = 2                                       # GPIO Pin number for TFT "DC"
#
pin_sd_cs = 22                                       # GPIO Pin number for SD card "CS"
#
pin_vs_cs = 5                                        # GPIO Pin number for VS1053 "CS"
pin_vs_dcs = 32                                      # GPIO Pin number for VS1053 "DCS"
pin_vs_dreq = 4                                      # GPIO Pin number for VS1053 "DREQ"
)=====" ;
