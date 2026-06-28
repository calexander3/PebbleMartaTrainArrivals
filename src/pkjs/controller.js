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
  const requestId =
    payload.REQUEST_ID !== undefined && payload.REQUEST_ID !== null
      ? Number(payload.REQUEST_ID)
      : null;
  console.log("watch command: " + JSON.stringify(payload));

  if (command === "INIT") {
    sendCurrentMenu(requestId);
    if (selectedStationIndex >= 0) {
      sendSelectedStation(requestId);
    }
  } else if (command === "SELECT_STATION") {
    selectedStationIndex = index;
    if (lastUpdated) {
      sendSelectedStation(requestId);
    }
    refreshTrainData(requestId);
  } else if (command === "REFRESH") {
    if (!Number.isNaN(index)) {
      selectedStationIndex = index;
    }
    refreshTrainData(requestId);
  }
}

function sendCurrentMenu(requestId) {
  if (closestStations.length > 0) {
    watch.sendClosestStationsMenu(closestStations, requestId);
  } else {
    watch.sendLoadingMenu(requestId);
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

function refreshTrainData(requestId) {
  const selectedStation = stations[selectedStationIndex];
  if (refreshInFlight) {
    if (selectedStationIndex >= 0 && lastUpdated) {
      sendSelectedStation(requestId);
    }
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
        sendSelectedStation(requestId);
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
          "error",
          requestId
        );
      }
    });
}

function sendSelectedStation(requestId) {
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
  watch.sendStation(selectedStationIndex, rows, lastUpdated, "", requestId);
}

function stationRows(stationIndex) {
  const station = stations[stationIndex];
  if (!station) return [];

  const rows = takeNextTwoPerDestination(
    trainRows
      .filter(
        (train) => normalizeStation(train.station) === station.normalizedApiName
      )
      .sort((a, b) => a.waitingSeconds - b.waitingSeconds)
  ).map((train) => ({
    line: train.line,
    direction: train.direction,
    destination: shortenString(train.destination),
    wait: shortenString(train.wait),
  }));

  return rows;
}

function takeNextTwoPerDestination(trains) {
  const destinationCounts = {};

  return trains.filter((train) => {
    const destination = normalizeStation(train.destination);
    destinationCounts[destination] = (destinationCounts[destination] || 0) + 1;

    return destinationCounts[destination] <= 2;
  });
}

function shortenString(str) {
  return str
    .replace("Hamilton E.", "H E")
    .replace("North", "N")
    .replace("Creek", "Crk")
    .replace("Lindbergh Center", "Lindbergh");
}

module.exports = { handleCommand, start };
