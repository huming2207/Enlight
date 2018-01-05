"use strict";

function onTempSliderChange() {

    var element = document.getElementById("temp-slider");
    sendSettings("/temp", "value=" + element.value.toString());

}

function onBrightSliderChange() {

    var element = document.getElementById("bright-slider");
    sendSettings("/bright", "value=" + element.value.toString());

}

function onColorSelectorChange() {

    var element =  document.getElementById("led-color-selector");
    sendSettings("/color", "value=" + encodeURIComponent(element.value.toString()));
}

function sendSettings(endPoint, setting) {

    var httpRequest = new XMLHttpRequest();

    httpRequest.onreadystatechange = function (ev) {

        if(this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            if(!this.responseText.localeCompare("OK")) {
                console.log("Wrong parameters, this is a bug.")
            }
        }

    };

    // ...Fire in the hole!
    httpRequest.open("GET", endPoint + "?" + setting, true);
    httpRequest.send();
}
