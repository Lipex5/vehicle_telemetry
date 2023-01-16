// Create the script tag, set the appropriate attributes
var script = document.createElement('script');
script.src = 'https://maps.googleapis.com/maps/api/js?key=KEY&callback=initMap';
script.async = true;

var i = 1;

// Attach your callback function to the `window` object
window.initMap = function () {
    // JS API is loaded and available

    // Get the current position of the device
        var pos = {
        lat: global_lat,
        lng: global_lng
    };

    // Create a new map and center it on the device's current position
    var map = new google.maps.Map(document.getElementById('map'), {
        zoom: 20,
        center: pos
    });

    // Add a marker to the map at the device's current position
    var marker = new google.maps.Marker({
        position: pos,
        map: map
    });
};

//window.initMap();
setInterval(function () {
    i = i + 1;
    window.initMap();
}, 5000);

// Append the 'script' element to 'head'
document.head.appendChild(script);