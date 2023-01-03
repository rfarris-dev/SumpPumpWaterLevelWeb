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

#healthtable tbody>tr>:nth-child(3){
 width:100px;
 text-align:left;
}

#healthtable tbody>tr>:nth-child(4){
 width:450px;
 text-align:left;
}

#configtable {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  table-layout: fixed;
  width: 500px;
}

#configtable td, #healthtable th {
  border: 1px solid #ddd;
  padding: 8px;
}

#configtable tr:nth-child(even){background-color: #f2f2f2;}

#configtable tr:hover {background-color: #ddd;}

#configtable th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #00ACFC;
  color: white;
}

#configtable tbody>tr>:nth-child(1){
 width:350px;
 text-align:left;
}

#configtable tbody>tr>:nth-child(2){
 width:150px;
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
      <tr><td><span id="percentFilled">-</span>%<br></td></tr>
      </table>
    </div>
    </td>
  </tr>
  
  
  <tr>
    <td>Water Level: </td>
    <td>
    <div>
      Distance (cm): <span id="distanceCm">-</span><br>
      Distance (inch): <span id="distanceInch">-</span><br>
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
      Wifi: <span id="ssid">-</span><br>
      IP Address: <span id="ipAddress">-</span><br>
      Mac Address: <span id="macAddress">-</span><br>
    </div>
    </td>
  </tr>
  
  <tr>
    <td>LED State:</td>
    <td>
    <div>
      <button class="button" onclick="send(1)">LED OFF</button>
      <button class="button" onclick="send(0)">LED ON</button> <b><span id="state"></span></b>
    </div>
    </td>
  </tr>
</table>

<br>

<h2>Configuration</h2>

<table id="configtable">
  <tr>
      <th>Board ID: <span id="boardId">-</span></th>
	  <th>Update</th>
  </tr>
  <tr>
    <td>
    <div>
	  <form  name="eepromForm" action="/action_page">
	   Sensor to min water level distance (in): <b><span id="emptyWaterLevelDistanceInches">-</span></b>
    </div>
    </td>
	<td>
	<div>
	  <input type="text" size="8" name="emptywaterleveldistanceinches" value="">
	</div>
	</td>
  </tr>
  <tr>
	<td>
	<div>
	  Sensor to max water level distance (in): <b><span id="warningWaterLevelDistanceInches">-</span></b>
	</div>
	</td>
	<td>
	<div>
	  <input type="text" size="8" name="warningwaterleveldistanceinches" value="">
	</div>
	</td>
  </tr>
  <tr>
	<td>
	<div>
	  LED state after power up: <b><span id="boardLedOn">-</span></b>
	</div>
	</td>
    <td>
	<div>
	  <select style="width: 90px;" name="boardledon">
	  <option value="0">LED On</option>
      <option value="1">LED Off</option>
	</div>
	</td>
  </tr>
  
  <tr>
	<td>
	</td>
	<td>
	  <div align="left">
	  <input class="button" type="submit" value="Write">
	  </form>
	  <button class="button" id="readsettingsbutton" onclick="readButton()">Read</button>
	</div>
	</td>
  </tr>
  </table>

<script> 

function readButton() {
	document.forms['eepromForm']['emptywaterleveldistanceinches'].value = document.getElementById("emptyWaterLevelDistanceInches").innerHTML;
	document.forms['eepromForm']['warningwaterleveldistanceinches'].value = document.getElementById("warningWaterLevelDistanceInches").innerHTML;
	document.forms['eepromForm']['boardledon'].value = document.getElementById("boardLedOn").innerHTML;
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
  getBoardLedOn();
}, 500); 

setTimeout(function() 
{
  readButton();
}, 1300); 


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

function getBoardLedOn() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("boardLedOn").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "boardledon", true);
  xhttp.send();
}


</script>
</left>
</body>
</html>
)=====";
