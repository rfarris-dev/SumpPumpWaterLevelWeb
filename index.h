const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style type="text/css">
.button {
  background-color: #00ACFC;
  border: none;
  color: white;
  padding: 3.75px 8px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
}



#healthtable {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  table-layout: fixed;
  width: 500px;
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
  background-color: #04AA6D;
  color: white;
}

tbody>tr>:nth-child(1){
 width:100px;
 text-align:left;
}

tbody>tr>:nth-child(2){
 width:400px;
 text-align:left;
}

</style>

<body style="background-color: #ffffff">


<left>
<h2>Sump Pump Water Level Monitor</h2>

<table id="healthtable">
<tr>
  <td>Pump Health: </td>
  <td>
  <div>
    Distance (cm): <span id="distanceCm">0</span><br>
    Distance (inch): <span id="distanceInch">0</span><br>
  </div>
  <div>
    <b><span id="pumpHealth"></b></span><br>
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

<script>

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
  getData();
  getDataInch();
  getDataPumpHealth();
}, 500); 

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

</script>
</left>
</body>
</html>
)=====";