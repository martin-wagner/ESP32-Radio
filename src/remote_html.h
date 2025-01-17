// remote.html file in raw data format for PROGMEM
//
#define remote_html_version 210514
const char remote_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <title>ESP32-radio</title>
  <meta http-equiv="content-type" content="text/html; charset=ISO-8859-1">
  <link rel="stylesheet" type="text/css" href="radio.css">
  <link rel="Shortcut Icon" type="image/ico" href="favicon.ico">
 </head>
 <body>
  <ul>
   <li><a class="pull-left" href="#">ESP32 Radio</a></li>
   <li><a class="pull-left" href="/index.html">Control</a></li>
   <li><a class="pull-left" href="/search.html">Search</a></li>
   <li><a class="pull-left active" href="/remote.html">Remote</a></li>
   <li><a class="pull-left" href="/config.html">Config</a></li>
   <li><a class="pull-left" href="/mp3play.html">MP3 player</a></li>
   <li><a class="pull-left" href="/about.html">About</a></li>
  </ul>
  <br><br><br>
  <center>
   <h1>** SABA 6-3D Remote Control **</h1>
   <button class="button" onpointerdown="httpGet('dial_fastmoveleft=30000')" onpointerup="httpGet('dial_stop')">&lt&lt</button>
   <button class="button" onpointerdown="httpGet('dial_moveleft=30000')" onpointerup="httpGet('dial_stop')">&lt</button>
   <button class="button" onpointerdown="httpGet('dial_moveleft=30000')" onpointerup="httpGet('dial_searchleft=500')">|&lt</button>
   <button class="button" onclick="httpGet('dial_stop')">STOP</button>
   <button class="button" onpointerdown="httpGet('dial_moveright=30000')" onpointerup="httpGet('dial_searchright=500')">&gt|</button>
   <button class="button" onpointerdown="httpGet('dial_moveright=30000')" onpointerup="httpGet('dial_stop')">&gt</button>
   <button class="button" onpointerdown="httpGet('dial_fastmoveright=30000')" onpointerup="httpGet('dial_stop')">&gt&gt</button>
  </center>
  <br>
  <center>
   <button class="button" onpointerdown="httpGet('amp_downvolume=15000')" onpointerup="httpGet('amp_stopvolume')">VOL-</button>
   <button class="button" onpointerdown="httpGet('amp_upvolume=15000')" onpointerup="httpGet('amp_stopvolume')">VOL+</button>
   <button class="button" onclick="httpGet('amp_mute')">(un)MUTE</button>
  </center>
  <script>
   function httpGet ( theReq )
   {
    var theUrl = "/?" + theReq + "&version=" + Math.random() ;
    var xhr = new XMLHttpRequest() ;
    xhr.onreadystatechange = function() {
      if ( xhr.readyState == XMLHttpRequest.DONE )
      {
        resultstr.value = xhr.responseText ;
      }
    }
    xhr.open ( "GET", theUrl ) ;
    xhr.send() ;
   }
 
 </script>
 </body>
</html>
)=====" ;
