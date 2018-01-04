"use strict";

var tempSlider = document.getElementById("temp-slider");
var brightSlider = document.getElementById("bright-slider");
var colorSelector = document.getElementById("color-selector");

function pageInit() {

    tempSlider.addEventListener("change", onTempSliderChange);
    brightSlider.addEventListener("change", onBrightSliderChange);
    colorSelector.addEventListener("change", onColorSelectorChange);

}

function onTempSliderChange() {


}

function onBrightSliderChange() {

    sendSettings("/bright", "value=" + brightSlider.value().toString())

}

function onColorSelectorChange() {



}

function sendSettings(endPoint, setting) {

    var httpRequest = new XMLHttpRequest();

    httpRequest.onreadystatechange = function (ev) {

        if(this.readyState === XMLHttpRequest.DONE && this.status === 200) {

            if(!this.responseText.localeCompare("OK")) {
                alert("Wrong parameters, this is a bug.")
            }

        } else {
            alert("Request failed!")
        }

    };

    // ...Fire in the hole!
    httpRequest.open("GET", endPoint + "?" + setting, true);
    httpRequest.send();
}
