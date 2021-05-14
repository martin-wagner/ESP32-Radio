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
   <button class="button" onclick="httpGet('dial_moveleft=30000')">&lt&lt</button>
   <button class="button" onclick="httpGet('dial_searchleft=100')">&lt</button>
   <button class="button" onclick="httpGet('dial_stop')">STOP</button>
   <button class="button" onclick="httpGet('dial_searchright=100')">&gt</button>
   <button class="button" onclick="httpGet('dial_moveright=30000')">&gt&gt</button>
   <button class="button" onclick="httpGet('amp_downvolume=500')">VOL-</button>
   <button class="button" onclick="httpGet('amp_upvolume=500')">VOL+</button>
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
