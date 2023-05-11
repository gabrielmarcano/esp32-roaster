const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const faker = require("faker");

const app = express();

app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

const PORT = 3000;

let clients = [];

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
 * Switches States type config
 * @typedef {Object} States
 * @property {boolean} motor1 The state of the first switch (motor 1)
 * @property {boolean} motor2 The state of the second switch (motor 2)
 * @property {boolean} motor3 The state of the third switch (motor 3)
 */

/**
 * SSE message serializer
 * @param {String} event Event name
 * @param {Readings | Timer | States | String} data Event data
 */
function serializeEvent(event, data) {
  const jsonString = JSON.stringify(data);
  return `event: ${event}\ndata: ${jsonString}\n\n`;
}

// timer logic
let server_total = 10;
let server_time = server_total;

// states
let m1_state = false;
let m2_state = false;
let m3_state = false;

app.get("/events", (request, response) => {
  const headers = {
    "Content-Type": "text/event-stream",
    Connection: "keep-alive",
    "Cache-Control": "no-cache",
  };
  response.writeHead(200, headers);

  setInterval(() => {
    response.write(`data: ping\n\n`);

    response.write(
      serializeEvent("readings", {
        temperature: faker.datatype.number({ min: 10, max: 25 }),
        humidity: faker.datatype.number({ min: 40, max: 45 }),
      })
    );

    response.write(
      serializeEvent("timer", {
        total: server_total,
        time: server_time,
      })
    );

    // decrease time or reset
    server_time = server_time <= 0 ? server_total : (server_time -= 1);

    // response.write(
    //   serializeEvent("states", {
    //     motor1: faker.datatype.boolean(),
    //     motor2: faker.datatype.boolean(),
    //     motor3: faker.datatype.boolean(),
    //   })
    // );

    response.write(
      serializeEvent("states", {
        motor1: m1_state,
        motor2: m2_state,
        motor3: m3_state,
      })
    );
  }, 1000);

  const clientId = Date.now();

  const newClient = {
    id: clientId,
    response,
  };

  clients.push(newClient);

  request.on("close", () => {
    console.log(`${clientId} Connection closed`);
    clients = clients.filter((client) => client.id !== clientId);
  });
});

app.get("/status", (request, response) =>
  response.json({ clients: clients.length })
);

app.post("/motors", (request, response) => {
  let data = request.body;

  m1_state = data.motor1;
  m2_state = data.motor2;
  m3_state = data.motor3;

  response.status(200);
});

app.get("/data", (request, response) => {
  response.json({
    timer: { total: 60, time: 30 },
    readings: { temperature: 25, humidity: 66 },
    states: { motor1: m1_state, motor2: m2_state, motor3: m3_state },
  });
});

app.listen(PORT, () => {
  console.log(`SSE service listening at http://localhost:${PORT}`);
});
