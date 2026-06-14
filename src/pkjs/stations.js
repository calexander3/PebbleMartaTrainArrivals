const stations = [
  {
    name: "Airport",
    apiName: "AIRPORT STATION",
    lat: 33.640758,
    lon: -84.446341,
  },
  {
    name: "Arts Center",
    apiName: "ARTS CENTER STATION",
    lat: 33.789705,
    lon: -84.387789,
  },
  { name: "Ashby", apiName: "ASHBY STATION", lat: 33.756346, lon: -84.417556 },
  {
    name: "Avondale",
    apiName: "AVONDALE STATION",
    lat: 33.775277,
    lon: -84.281903,
  },
  {
    name: "Bankhead",
    apiName: "BANKHEAD STATION",
    lat: 33.77189,
    lon: -84.42884,
  },
  {
    name: "Brookhaven",
    apiName: "BROOKHAVEN STATION",
    lat: 33.860705,
    lon: -84.340003,
  },
  {
    name: "Buckhead",
    apiName: "BUCKHEAD STATION",
    lat: 33.84841,
    lon: -84.367018,
  },
  {
    name: "Chamblee",
    apiName: "CHAMBLEE STATION",
    lat: 33.886191,
    lon: -84.306957,
  },
  {
    name: "Civic Center",
    apiName: "CIVIC CENTER STATION",
    lat: 33.766305,
    lon: -84.387209,
  },
  {
    name: "College Park",
    apiName: "COLLEGE PARK STATION",
    lat: 33.651673,
    lon: -84.448793,
  },
  {
    name: "Decatur",
    apiName: "DECATUR STATION",
    lat: 33.774717,
    lon: -84.295588,
  },
  {
    name: "Dome",
    apiName: "OMNI DOME STATION",
    lat: 33.756293,
    lon: -84.397759,
  },
  {
    name: "Doraville",
    apiName: "DORAVILLE STATION",
    lat: 33.902079,
    lon: -84.280389,
  },
  {
    name: "Dunwoody",
    apiName: "DUNWOODY STATION",
    lat: 33.9212,
    lon: -84.3444,
  },
  {
    name: "East Lake",
    apiName: "EAST LAKE STATION",
    lat: 33.765166,
    lon: -84.312665,
  },
  {
    name: "East Point",
    apiName: "EAST POINT STATION",
    lat: 33.677814,
    lon: -84.440344,
  },
  {
    name: "Edgewood",
    apiName: "EDGEWOOD CANDLER PARK STATION",
    lat: 33.762001,
    lon: -84.339579,
  },
  {
    name: "Five Points",
    apiName: "FIVE POINTS STATION",
    lat: 33.753826,
    lon: -84.391571,
  },
  {
    name: "Garnett",
    apiName: "GARNETT STATION",
    lat: 33.747845,
    lon: -84.396415,
  },
  {
    name: "Georgia State",
    apiName: "GEORGIA STATE STATION",
    lat: 33.750539,
    lon: -84.386464,
  },
  {
    name: "H E Holmes",
    apiName: "HAMILTON E HOLMES STATION",
    lat: 33.754638,
    lon: -84.46794,
  },
  {
    name: "Indian Creek",
    apiName: "INDIAN CREEK STATION",
    lat: 33.769794,
    lon: -84.229656,
  },
  {
    name: "Inman Park",
    apiName: "INMAN PARK STATION",
    lat: 33.757497,
    lon: -84.352797,
  },
  {
    name: "Kensington",
    apiName: "KENSINGTON STATION",
    lat: 33.772664,
    lon: -84.251937,
  },
  {
    name: "King Memorial",
    apiName: "KING MEMORIAL STATION",
    lat: 33.749959,
    lon: -84.37544,
  },
  {
    name: "Lakewood",
    apiName: "LAKEWOOD STATION",
    lat: 33.700457,
    lon: -84.428859,
  },
  { name: "Lenox", apiName: "LENOX STATION", lat: 33.847144, lon: -84.35631 },
  {
    name: "Lindbergh",
    apiName: "LINDBERGH STATION",
    lat: 33.821995,
    lon: -84.367447,
  },
  {
    name: "Medical Center",
    apiName: "MEDICAL CENTER STATION",
    lat: 33.910689,
    lon: -84.352684,
  },
  {
    name: "Midtown",
    apiName: "MIDTOWN STATION",
    lat: 33.781121,
    lon: -84.386345,
  },
  {
    name: "North Avenue",
    apiName: "NORTH AVE STATION",
    lat: 33.771712,
    lon: -84.386699,
  },
  {
    name: "North Springs",
    apiName: "NORTH SPRINGS STATION",
    lat: 33.944552,
    lon: -84.356206,
  },
  {
    name: "Oakland City",
    apiName: "OAKLAND CITY STATION",
    lat: 33.716848,
    lon: -84.4252,
  },
  {
    name: "Peachtree Center",
    apiName: "PEACHTREE CENTER STATION",
    lat: 33.759677,
    lon: -84.387548,
  },
  {
    name: "Sandy Springs",
    apiName: "SANDY SPRINGS STATION",
    lat: 33.933035,
    lon: -84.352019,
  },
  {
    name: "Vine City",
    apiName: "VINE CITY STATION",
    lat: 33.75687,
    lon: -84.40391,
  },
  {
    name: "West End",
    apiName: "WEST END STATION",
    lat: 33.73581,
    lon: -84.41296,
  },
  {
    name: "West Lake",
    apiName: "WEST LAKE STATION",
    lat: 33.75314,
    lon: -84.44658,
  },
];

module.exports = stations.map((station, index) =>
  Object.assign({}, station, {
    index,
    normalizedApiName: normalizeStation(station.apiName),
  })
);

function normalizeStation(value) {
  return String(value || "")
    .toUpperCase()
    .replace(/[^A-Z0-9]/g, "");
}

module.exports.normalizeStation = normalizeStation;
