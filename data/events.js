// TYPES

/**
 * Readings type config
 * @typedef {Object} Readings
 * @property {number} temperature Temperature value as an integer
 * @property {number} humidity Humidity value as a percentage (0 - 100)
 */

/**
 * Timer type config
 * @typedef {Object} Timer
 * @property {number} total Number of the clock total time in seconds
 * @property {number} time Number of seconds remaining in the clock
 */

/**
 * Motors States type config
 * @typedef {Object} States
 * @property {boolean} motor1 The state of the first switch (motor 1)
 * @property {boolean} motor2 The state of the second switch (motor 2)
 * @property {boolean} motor3 The state of the third switch (motor 3)
 */

// ELEMENTS & OBJECTS

/**
 * Used to only update the reading values once every 2 reading events,
 * due to gauges bugging because of a fast render update.
 */
let delay = true;

// Create Temperature Gauge Object
const temperatureGauge = new LinearGauge({
  renderTo: "gauge-temperature",
  width: 120,
  height: 400,
  units: "Temperatura (°C)",
  minValue: 0,
  maxValue: 250,
  startAngle: 90,
  ticksAngle: 180,
  colorValueBoxRect: "#049faa",
  colorValueBoxRectEnd: "#049faa",
  colorValueBoxBackground: "#f1fbfc",
  valueDec: 0,
  valueInt: 3,
  majorTicks: ["0", "50", "100", "150", "200", "250"],
  minorTicks: 5,
  strokeTicks: true,
  highlights: [
    {
      from: 0,
      to: 185,
      color: "rgba(225, 250, 250, .75)",
    },
    {
      from: 185,
      to: 215,
      color: "rgba(255, 234, 0, .75)",
    },
    {
      from: 195,
      to: 205,
      color: "rgba(2, 167, 36, .75)",
    },
    {
      from: 215,
      to: 250,
      color: "rgba(255, 0, 0, .75)",
    },
  ],
  colorPlate: "#fff",
  colorBarProgress: "#0047AB",
  colorBarProgressEnd: "#049faa",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  needleCircleSize: 7,
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 1500,
  animationRule: "linear",
  barWidth: 12,
}).draw();

// Create Humidity Gauge Object
const humidityGauge = new RadialGauge({
  renderTo: "gauge-humidity",
  width: 300,
  height: 300,
  units: "Humedad (%)",
  minValue: 0,
  maxValue: 100,
  colorValueBoxRect: "#049faa",
  colorValueBoxRectEnd: "#049faa",
  colorValueBoxBackground: "#f1fbfc",
  valueDec: 0,
  valueInt: 2,
  majorTicks: ["0", "20", "40", "60", "80", "100"],
  minorTicks: 4,
  strokeTicks: true,
  highlights: [
    {
      from: 80,
      to: 100,
      color: "#03C0C1",
    },
  ],
  colorPlate: "#fff",
  borderShadowWidth: 0,
  borders: false,
  needleType: "line",
  colorNeedle: "#007F80",
  colorNeedleEnd: "#007F80",
  needleWidth: 2,
  needleCircleSize: 3,
  colorNeedleCircleOuter: "#007F80",
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 1500,
  animationRule: "linear",
}).draw();

// Get clock elements
const clock = document.querySelector(".clock");
const count = document.querySelector(".count");

// Set time-related variables
let total;
let time;

// Get switches elements
const switch1 = document.querySelector("#switch1");
const switch2 = document.querySelector("#switch2");
const switch3 = document.querySelector("#switch3");

// EVENTS

// Get current sensor readings when the page loads
window.addEventListener("load", () => {
  fetch("/data")
    .then((res) => res.json())
    .then(({ readings, timer, states }) => {
      readings = JSON.parse(readings);
      timer = JSON.parse(timer);
      states = JSON.parse(states);

      temperatureGauge.value = readings.temperature;
      humidityGauge.value = readings.humidity;

      total = timer.total;
      time = timer.time <= 0 ? 0 : timer.time;

      switch1.checked = states.motor1;
      switch2.checked = states.motor2;
      switch3.checked = states.motor3;
    });
});

if (!!window.EventSource) {
  // let source = new EventSource("http://localhost:3000/events"); // use this when sse-local-server is on
  // let source = new EventSource("http://192.168.0.177:3000/events"); // use this when sse-local-server is on
  let source = new EventSource("/events");

  // Start event listener
  source.addEventListener("open", function (e) {
    console.log("Events Connected");
  });

  // Stop event listener
  source.addEventListener("error", function (e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  });

  // Message handler
  source.addEventListener("message", function (e) {
    console.log("\nmessage\n", e.data);
  });

  // Readings / Gauges event handler
  source.addEventListener("readings", function (e) {
    if (!delay) {
      /**
       * An object holding the temperature & humidity values
       * @type {Readings}
       */
      let readings = JSON.parse(e.data);
      console.log("readings", readings);

      temperatureGauge.value = readings.temperature;
      humidityGauge.value = readings.humidity;

      delay = !delay;
    } else {
      delay = !delay;
    }
  });

  // Timer event handler
  source.addEventListener("timer", function (e) {
    /**
     * An object holding all time-related data
     * @type {Timer}
     */
    let timer = JSON.parse(e.data);
    console.log("timer", timer);

    // update time values
    total = timer.total;
    time = timer.time <= 0 ? 0 : timer.time;

    // update circular progress bar
    clock.style.background = `conic-gradient(#db3e6c, ${
      (time * 360) / total
    }deg, #feeff4 0deg)`;

    // format clock
    let minutes = Math.floor(time / 60);
    let seconds = time - minutes * 60;

    count.textContent = `${minutes < 10 ? "0" : ""}${minutes}:${
      seconds < 10 ? "0" : ""
    }${seconds}`;
  });

  // Switches states event handler
  source.addEventListener("states", function (e) {
    /**
     * An object holding all motor states
     * @type {States}
     */
    let states = JSON.parse(e.data);
    console.log("states", states);

    switch1.checked = states.motor1;
    switch2.checked = states.motor2;
    switch3.checked = states.motor3;
  });
}

// handle update of motor status to the esp32
document.querySelectorAll('[type="checkbox"]').forEach((sw) => {
  sw.addEventListener("click", () => {
    fetch("/motors", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        [`motor${sw.id.match(/[123]/g)[0]}`]: sw.checked,
      }),
    });
  });
});
