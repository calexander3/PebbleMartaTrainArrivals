const controller = require("./controller");

Pebble.addEventListener("ready", function () {
  console.log("Marta PKJS ready");
  controller.start();
});

Pebble.addEventListener("appmessage", function (event) {
  controller.handleCommand(event.payload || {});
});
