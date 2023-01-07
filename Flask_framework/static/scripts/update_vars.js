// Update function
var global_lat; //global variable 
var global_lng; //global variable   

function update() {
    let led = document.getElementById("led_status");
    let tempC = document.getElementById("temp_celsius");
    let tempF = document.getElementById("temp_fahren");
    let tempK = document.getElementById("temp_kelv");
    let lat = document.getElementById("lat_status");
    let lng = document.getElementById("lng_status");
    let vel = document.getElementById("vel_status");

    fetch("/mqtt/data/").then(
        response => response.json()
    ).then(
        function (data) {
            console.log(data["led"]);
            led.innerText = data["led"];

            tempC.innerText = data["tempC"];

            tempF.innerText = data["tempF"];

            tempK.innerText = data["tempK"];

            console.log(data["lat"]);
            global_lat = parseFloat(data["lat"]);

            console.log(data["lng"]);
            global_lng = parseFloat(data["lng"]);

            console.log(data["vel"]);
            vel.innerText = data["vel"];
        });
}

(function () {
    update();
    setInterval(function () {
        update();
    }, 500);
})();
