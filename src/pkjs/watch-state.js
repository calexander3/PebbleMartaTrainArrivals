const MAX_CLOSEST = 5;
const MAX_TRAINS = 20;

function sendReady() {
  send({ READY: 1 });
}

function sendLoadingMenu() {
  send({
    SCREEN: "menu",
    MENU_KIND: "loading",
    MENU_COUNT: 0,
  });
}

function sendAllStationsMenu() {
  send({
    SCREEN: "menu",
    MENU_KIND: "all",
    MENU_COUNT: 0,
  });
}

function sendClosestStationsMenu(stations) {
  const message = {
    SCREEN: "menu",
    MENU_KIND: "closest",
    MENU_COUNT: Math.min(stations.length, MAX_CLOSEST),
  };

  stations.slice(0, MAX_CLOSEST).forEach((station, index) => {
    message["CLOSEST_" + index + "_INDEX"] = station.index;
    message["CLOSEST_" + index + "_DISTANCE"] = station.distanceText || "";
  });

  send(message);
}

function sendStation(stationIndex, trains, lastUpdated, error) {
  const rows = trains.slice(0, MAX_TRAINS);
  const message = {
    SCREEN: "station",
    SELECTED_STATION_INDEX: stationIndex,
    TRAIN_COUNT: rows.length,
    LAST_UPDATED_TS: lastUpdated || 0,
    ERROR: error || "",
  };

  rows.forEach((train, index) => {
    message["TRAIN_" + index + "_LINE"] = train.line;
    message["TRAIN_" + index + "_DIRECTION"] = train.direction;
    message["TRAIN_" + index + "_DESTINATION"] = train.destination;
    message["TRAIN_" + index + "_WAIT"] = train.wait;
  });

  send(message, "station rows=" + rows.length);
}

function send(message, label) {
  Pebble.sendAppMessage(
    message,
    function () {},
    function (_error) {
      console.log("send failed: " + (label || JSON.stringify(message)));
    }
  );
}

module.exports = {
  sendAllStationsMenu,
  sendClosestStationsMenu,
  sendLoadingMenu,
  sendReady,
  sendStation,
};
