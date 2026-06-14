const { getCurrentPosition, distanceMiles } = require("./location");
const martaApi = require("./marta-api");
const stations = require("./stations");
const { normalizeStation } = require("./stations");
const watch = require("./watch-state");

let started = false;
let trainRows = [];
let lastUpdated = 0;
let closestStations = [];
let selectedStationIndex = -1;
let refreshInFlight = false;

function start() {
  if (started) return;
  started = true;

  watch.sendReady();
  watch.sendLoadingMenu();
  refreshLocation();
  refreshTrainData();
}

function handleCommand(payload) {
  const command = payload.COMMAND;
  const index = Number(payload.INDEX);
  console.log("watch command: " + JSON.stringify(payload));

  if (command === "INIT") {
    sendCurrentMenu();
    if (selectedStationIndex >= 0) {
      sendSelectedStation();
    }
  } else if (command === "SELECT_STATION") {
    selectedStationIndex = index;
    if (lastUpdated) {
      sendSelectedStation();
    }
    refreshTrainData();
  } else if (command === "REFRESH") {
    if (!Number.isNaN(index)) {
      selectedStationIndex = index;
    }
    refreshTrainData();
  }
}

function sendCurrentMenu() {
  if (closestStations.length > 0) {
    watch.sendClosestStationsMenu(closestStations);
  } else {
    watch.sendLoadingMenu();
  }
}

function refreshLocation() {
  getCurrentPosition()
    .then((position) => {
      closestStations = stations
        .map((station) => {
          const distance = distanceMiles(
            position.coords.latitude,
            position.coords.longitude,
            station.lat,
            station.lon
          );
          return Object.assign({}, station, {
            distance,
            distanceText:
              distance > -1
                ? distance.toFixed(distance < 10 ? 1 : 0) + " Miles"
                : "",
          });
        })
        .sort((a, b) => a.distance - b.distance)
        .slice(0, 5);

      watch.sendClosestStationsMenu(closestStations);
    })
    .catch((error) => {
      console.log("location error: " + error.message);
      watch.sendAllStationsMenu();
    });
}

function refreshTrainData() {
  const selectedStation = stations[selectedStationIndex];
  if (refreshInFlight) {
    return;
  }
  refreshInFlight = true;

  martaApi
    .getTrainData(selectedStation)
    .then((rows) => {
      refreshInFlight = false;
      trainRows = rows || [];
      lastUpdated = Math.floor(Date.now() / 1000);
      console.log("train refresh rows=" + trainRows.length);
      if (selectedStationIndex >= 0) {
        sendSelectedStation();
      }
    })
    .catch((error) => {
      refreshInFlight = false;
      console.log("train refresh failed: " + error.message);
      if (selectedStationIndex >= 0) {
        const cachedRows = stationRows(selectedStationIndex);
        watch.sendStation(
          selectedStationIndex,
          cachedRows,
          lastUpdated,
          "error"
        );
      }
    });
}

function sendSelectedStation() {
  if (selectedStationIndex < 0 || selectedStationIndex >= stations.length) {
    return;
  }

  const rows = stationRows(selectedStationIndex);
  console.log(
    "sending station " +
      stations[selectedStationIndex].name +
      " rows=" +
      rows.length +
      " updated=" +
      lastUpdated
  );
  watch.sendStation(selectedStationIndex, rows, lastUpdated, "");
}

function stationRows(stationIndex) {
  const station = stations[stationIndex];
  if (!station) return [];

  return trainRows
    .filter(
      (train) => normalizeStation(train.station) === station.normalizedApiName
    )
    .sort((a, b) => a.waitingSeconds - b.waitingSeconds)
    .map((train) => ({
      line: train.line,
      direction: train.direction,
      destination: shortenString(train.destination),
      wait: shortenString(train.wait),
    }));
}

function shortenString(str) {
  return str
    .replace("Hamilton E.", "H E")
    .replace("North", "N")
    .replace("Lindbergh Center", "Lindbergh");
}

module.exports = { handleCommand, start };
