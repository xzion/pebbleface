
// ACK/NACK placeholder for debugging
function gotACK(e) {
	//console.log("Received ACK from watch!");
}

function gotNACK(e) {
	//console.log("Received NACK from watch!");
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
    	GetUQWeather();
    }

    
  }
);