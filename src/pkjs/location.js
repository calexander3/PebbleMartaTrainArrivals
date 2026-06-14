const locationOptions = {
  enableHighAccuracy: true,
  maximumAge: 10000,
  timeout: 4000,
};

function getCurrentPosition() {
  return new Promise((resolve, reject) => {
    navigator.geolocation.getCurrentPosition(resolve, reject, locationOptions);
  });
}

function distanceMiles(lat1, lon1, lat2, lon2) {
  if (!lat1 || !lat2 || !lon1 || !lon2) {
    return -1;
  }

  const phi1 = radians(lat1);
  const phi2 = radians(lat2);
  const deltaLambda = radians(lon2 - lon1);
  const meters =
    Math.acos(
      Math.sin(phi1) * Math.sin(phi2) +
        Math.cos(phi1) * Math.cos(phi2) * Math.cos(deltaLambda)
    ) * 6371000;
  return Math.abs(meters / 1000) * 0.62137;
}

function radians(number) {
  return (number * Math.PI) / 180;
}

module.exports = { distanceMiles, getCurrentPosition };
