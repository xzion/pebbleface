// Location Variable
var coords;



// ACK/NACK placeholder for debugging
function gotACK(e) {
	//console.log("Received ACK from watch!");
}

function gotNACK(e) {
	//console.log("Received NACK from watch!");
}

// OpenWeather Request
function GetOpenWeather() {
    window.navigator.geolocation.getCurrentPosition(
    	function(pos) {coords = pos.coords;},
    	function(err) {console.log("Location Error (" + err.code + "): " + err.message); },
    	{ "timeout" : 15000, "maximumAge" : 60000 }
   	);

   	console.log("Got this far");

    var response;
    var req = new XMLHttpRequest();
    var url = "http://api.openweathermap.org/data/2.5/find?" + "lat=" + coords.latitude + "&lon=" + coords.longitude + "&cnt=1";

    req.open('GET', url, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
			if (req.status == 200) {
				var response = JSON.parse(req.responseText);

				if (response && response.list && response.list.length > 0) {
					var new_weather = response.list[0];
					// Convert kelvin to degrees and round to 1 decimal place
					var ow_temp = Math.round(new_weather.main.temp*10-2731.5)/10;

					console.log("OW result: temp - " + String(ow_temp) + ", cond - " + new_weather.weather[0].id);

					Pebble.sendAppMessage({
			        	"returnOW" : new_weather.weather[0].id,
			        	"returnOWTemp" : String(ow_temp),
					}, gotACK, gotNACK);
					console.log("New OpenWeather results sent!");
			    }
			} else { 
				console.log("Error"); 
			}
        }
    }
    req.send(null);
}


// UQ Weather Request
function GetUQWeather() {
    var response;
    var req = new XMLHttpRequest();
    var url = "http://ww2.gpem.uq.edu.au/UQweather/";

    req.open('GET', url, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
			if (req.status == 200) {
				var temp_pattern = />Temperature<\/a>[\s\S]*?(\d+\.\d).C/gm;
				var temp_string = temp_pattern.exec(req.responseText);

				if (temp_string != null) {
					Pebble.sendAppMessage({
			        	"returnTemp" : temp_string[1],
					}, gotACK, gotNACK);
					console.log("New UQ Temp sent!");
				} else {
					console.log("UQ Temp regex failed");
				}
			} else { 
				console.log("Error"); 
			}
        }
    }
    req.send(null);
}


// BTC Request
function GetBTCPrice() {
    var response;
    var req = new XMLHttpRequest();
    var url = "http://data.mtgox.com/api/2/BTCUSD/money/ticker_fast";

    req.open('GET', url, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
			if (req.status == 200) {
				var response = JSON.parse(req.responseText);

				if (response) {
					Pebble.sendAppMessage({
			        	"returnBTC" : parseInt(response.data.last.value_int),
					}, gotACK, gotNACK);
					console.log("New BTC price sent!");
			    }
			} else { 
				console.log("Error"); 
			}
        }
    }
    req.send(null);
}


Pebble.addEventListener("ready", function(e) {
	// Get Location
	window.navigator.geolocation.getCurrentPosition(
    	function(pos) {coords = pos.coords;},
    	function(err) {console.log("Location Error (" + err.code + "): " + err.message); },
    	{ "timeout" : 15000, "maximumAge" : 60000 }
   	);

	// Send an initial message to the Pebble
	Pebble.sendAppMessage({
		"jsLoaded" : 1,
	}, gotACK, gotNACK);

    console.log("JavaScript app ready and running!");
  	}
);


Pebble.addEventListener("appmessage", function(e) {
    //console.log("Received message: " + e.payload["requestBTC"]);

    if (e.payload["requestBTC"]) {
    	// AppMessage requested new BTC price
    	console.log("Recieved AppMessage request for new BTC price");
    	GetBTCPrice();
    	//GetUQWeather();
    }

    if (e.payload["requestTemp"]) {
    	// AppMessage requested new Temperature
    	console.log("Received AppMessage request for new Temp");
    	//GetUQWeather();
    	GetOpenWeather();
    }

    
  }
);