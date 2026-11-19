# ESIR Starter

A small starting point for ESIR: a Blazor website (.NET 10) with a minimal API, and Arduino firmware that polls it over Wi-Fi.

```
ESIR.slnx
src/ESIR.Web/            Blazor Interactive Server site + Minimal APIs
  Services/RobotStore.cs   in-memory commands and robot status
  Components/Pages/Home.razor
firmware/ESIR_NetworkStarter/   Arduino MKR WiFi 1010 sketch
Dockerfile, compose.yaml
```

## Run the website

```bash
dotnet run --project src/ESIR.Web
```

Open http://localhost:5080. With Docker: `docker compose up --build` (same port).

## How the robot link works

The Arduino asks the website for work. The website never connects to the Arduino.

| Call | Who | Purpose |
|---|---|---|
| `POST /api/robot/emotion` `{"emotion":"happy"}` | website | queue a command |
| `GET /api/robot/state` | website | connection status and last command |
| `GET /api/device/commands/next` | Arduino | 200 with a command, or 204 if none |
| `POST /api/device/status` `{"commandId":"...","status":"accepted"}` | Arduino | confirm `accepted`, `running`, `completed` |

The robot counts as **Connected** if it contacted the site in the last 15 seconds. A command waits as **Pending** (and expires after 60 seconds) until the robot confirms it. The page says so when the robot is offline.

## Try it without hardware

Click a feeling on the home page, then pretend to be the robot:

```bash
curl http://localhost:5080/api/device/commands/next
curl -X POST http://localhost:5080/api/device/status -H "Content-Type: application/json" -d '{"commandId":"<id from above>","status":"completed"}'
```

## Firmware

Open `firmware/ESIR_NetworkStarter` in the Arduino IDE, install WiFiNINA, ArduinoHttpClient and ArduinoJson, set the Wi-Fi name, password and the computer's LAN IP, then upload. Drawing the face is still a TODO. The sketch has not been tested on hardware.

## Limits

State is in memory (lost on restart), there is no sign-in, and traffic is plain HTTP. Use this on a trusted school network only. ESIR is not a medical device.


