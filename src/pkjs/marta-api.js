const { MARTA_API_KEY } = require("./generated-config");
const { requestJson } = require("./http");

const endpoint =
  "https://developerservices.itsmarta.com:18096/itsmarta/railrealtimearrivals/developerservices/traindata";

function getTrainData(station) {
  if (!MARTA_API_KEY) {
    return Promise.reject(new Error("MARTA_API_KEY is not set"));
  }

  return requestJson(
    endpoint + "?apiKey=" + encodeURIComponent(MARTA_API_KEY)
  ).then(normalizeRows);
}

function normalizeRows(payload) {
  if (!Array.isArray(payload)) return [];

  return payload.map(function (train) {
    return {
      station: train.STATION || "",
      line: train.LINE || "",
      direction: train.DIRECTION || "",
      destination: train.DESTINATION || "",
      wait: train.WAITING_TIME || "",
      waitingSeconds: Number(train.WAITING_SECONDS) || 2147483647,
    };
  });
}

module.exports = { getTrainData };
