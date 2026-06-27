# Marta Next Train

Marta Next Train displays the arrival times of the upcoming trains in the metro
Atlanta, Georgia area.

This version is a native C Pebble app with a small PebbleKit JS phone side. The
watch renders menus and station arrivals; PKJS handles location and MARTA API
requests.

## Configuration

Create `.env` with your MARTA API key:

```sh
MARTA_API_KEY=your_marta_api_key_here
```

`npm run build` generates `src/pkjs/generated-config.js` from `.env`.

For local testing when trains are not running, add this to `.env` and rebuild:

```sh
MARTA_USE_EXAMPLE_RESPONSE=true
```

That makes PKJS return `example-api-response.json` instead of calling the live
MARTA API.

## Building & running

```sh
npm run build                         # live MARTA API build for a real phone/watch
npm start                             # build and install on the emery emulator with logs
npm run flint                         # build and install on the flint emulator with logs
npm run gabbro                        # build and install on the gabbro emulator with logs
pebble install --phone <ip> build/PebbleMartaTrainArrivals.pbw
```

## Time Format

Switch emulator between 12 and 24 hour format with

```sh
pebble emu-time-format --emulator <platform> --format <12/24>h
```

## Target platforms

`targetPlatforms` in `package.json` controls which watches you build for. The
modern Pebble hardware is **emery** (Pebble Time 2), **gabbro** (Pebble Round
2), and **flint** (Pebble 2 Duo).

## Project layout

```
src/c/           Native watch renderer, menus, station screen, AppMessage
src/pkjs/        Phone-side location, MARTA API polling, AppMessage state
scripts/         Local build helpers, including generated config
package.json     Project metadata, target platforms, message keys
wscript          Pebble SDK build rules
```

The station screen refreshes MARTA rail data every minute. The center button
manually refreshes while viewing a station.

## Documentation

Full SDK docs, tutorials, and API reference: <https://developer.repebble.com>
