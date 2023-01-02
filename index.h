const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style type="text/css">
<!-- HTML !-->

#waterleveltable {
  table-layout: fixed;
  width: 50px;
}

#waterleveltable td, #waterleveltable th {
  font-size: 45px;
  text-align: center;
  border: 1px solid black;
  border-radius: 7px;
}

#healthtable {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  table-layout: fixed;
  width: 550px;
}

#healthtable td, #healthtable th {
  border: 1px solid #ddd;
  padding: 8px;
}

#healthtable tr:nth-child(even){background-color: #f2f2f2;}

#healthtable tr:hover {background-color: #ddd;}

#healthtable th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #00ACFC;
  color: white;
}

tbody>tr>:nth-child(1){
 width:100px;
 text-align:left;
}

tbody>tr>:nth-child(2){
 width:450px;
 text-align:left;
}

</style>

<body style="background-color: #ffffff">

<h2>Sump Pump Water Level Monitor</h2>

<table id="healthtable">

  <tr>
      <th>Function</th>
      <th>Realtime Output</th>
  </tr>
  
  <tr>
    <td>Sump Pit<br>% Filled: </td>
    <td>
    <div>
      <table id="waterleveltable">
      <tr><td><span id="percentFilled">0</span>%<br></td></tr>
      </table>
    </div>
    </td>
  </tr>
  
  
  
  <tr>
    <td>Water Level: </td>
    <td>
    <div>
      Distance (cm): <span id="distanceCm">0</span><br>
      Distance (inch): <span id="distanceInch">0</span><br>
    </div>
    </td>
  </tr>
  
  <tr>
    <td>Pump Health: </td>
    <td>
    <div>
      <b><span id="pumpHealth"></b></span><br>
    </div>
    </td>
  </tr>
  
  <tr>
    <td>Network: </td>
    <td>
    <div>
      Wifi: <span id="ssid">0</span><br>
      IP Address: <span id="ipAddress">0</span><br>
      Mac Address: <span id="macAddress">0</span><br>
    </div>
    </td>
  </tr>
  
  <tr>
    <td>LED State:</td>
    <td>
    <div>
      Currently <b><span id="state">NA</span></b><br>
      <button class="button" onclick="send(1)">LED OFF</button>
      <button class="button" onclick="send(0)">LED ON</button><BR>
    </div>
    </td>
  </tr>
</table>

<h2>Configuration</h2>

<table id="healthtable">
  <tr>
      <th>Settings in EEPROM for Board ID: <span id="boardId">0</span></th>
  </tr>
  <tr>
    <td>
    <div>
	 <form  name="eepromForm" action="/action_page">
	 Sensor to min water level distance is set to <span id="emptyWaterLevelDistanceInches">0</span> inches<br>
	 <input type="text" name="emptywaterleveldistanceinches" value="">
    </div>
    </td>
  </tr>
  <tr>
	<td>
	<div>
	  Sensor to max water level distance is set to <span id="warningWaterLevelDistanceInches">0</span> inches<br>
	  <input type="text" name="warningwaterleveldistanceinches" value="">
	</div>
	</td>
  </tr>
  
    <tr>
	<td>
	<div align="right">
	  <input class="button" type="submit" value="Write Settings">
	  </form>
	  <button class="button" id="readsettingsbutton" onclick="readButton()">Read Settings</button>
	</div>
	</td>
  </tr>
  </table>

<script> 

function readButton() {
	document.forms['eepromForm']['emptywaterleveldistanceinches'].value = document.getElementById("emptyWaterLevelDistanceInches").innerHTML;
	document.forms['eepromForm']['warningwaterleveldistanceinches'].value = document.getElementById("warningWaterLevelDistanceInches").innerHTML;
}

function send(led_sts) 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "led_set?state="+led_sts, true);
  xhttp.send();
}

setInterval(function() 
{
  getSsid()
  getDataIPAddress();
  getDataMACAddress();
  getData();
  getDataInch();  
  getPercentFilled();
  getDataPumpHealth();
  getBoardId();
  getSensorToMinWaterLevelInches();
  getSensorToMaxWaterLevelInches();
}, 1000); 

setTimeout(function() 
{
  readButton();
}, 1200); 


function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("distanceCm").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readcm", true);
  xhttp.send();
}

function getDataInch() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("distanceInch").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readin", true);
  xhttp.send();
}



function getPercentFilled() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percentFilled").innerHTML =
      this.responseText;

      if (parseInt(this.responseText) >= 100) {
        document.getElementById('waterleveltable').bgColor = '#FF0000';
      }  
	  
	  else if ((parseInt(this.responseText) < 100) && (parseInt(this.responseText) > 80)) {
        document.getElementById('waterleveltable').bgColor = '#FFFF00';
      }

      else if ((parseInt(this.responseText) < 79) && (parseInt(this.responseText) > 5)) {
        document.getElementById('waterleveltable').bgColor = '#00ACFC';
      }

      else if (parseInt(this.responseText) <= 5) {
        document.getElementById('waterleveltable').bgColor = '#00FF00';
      }
    }
  };
  xhttp.open("GET", "percentfilled", true);
  xhttp.send();
}

function getDataPumpHealth() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("pumpHealth").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "pumphealth", true);
  xhttp.send();
}


function getSsid() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ssid").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "ssid", true);
  xhttp.send();
}

function getDataIPAddress() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ipAddress").innerHTML =
      this.responseText;
    } 
  };
  xhttp.open("GET", "ipaddress", true);
  xhttp.send();
}

function getDataMACAddress() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("macAddress").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "macaddress", true);
  xhttp.send();
}

function getBoardId() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("boardId").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "boardid", true);
  xhttp.send();
}

function getSensorToMinWaterLevelInches() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("emptyWaterLevelDistanceInches").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "emptywaterleveldistanceinches", true);
  xhttp.send();
}

function getSensorToMaxWaterLevelInches() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("warningWaterLevelDistanceInches").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "warningwaterleveldistanceinches", true);
  xhttp.send();
}


</script>
</left>
</body>
</html>
)=====";
