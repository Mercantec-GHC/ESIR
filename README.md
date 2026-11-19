# ESIR – Emotional Supportive Interactive Robot

ESIR is a small desk companion for students. You interact with it by touching, tilting, shaking or picking it up, and it answers with an animated face and LED light. A companion website lets you pick how you feel, and ESIR responds on the robot and on the screen, for example with a guided breathing exercise or cheering messages with hearts.

ESIR is a companion, not a therapist or an emergency service. It does not record conversations, and it stores no personal data or emotion history.

> **Status:** in development (student project, November–December 2026). Features below are planned unless marked as done.

---

## Contents

- [Features](#features)
- [Tech stack](#tech-stack)
- [How it works](#how-it-works)
- [Repository structure](#repository-structure)
- [Getting started](#getting-started)
- [Configuration](#configuration)
- [Robot API](#robot-api)
- [Firmware](#firmware)
- [Deployment](#deployment)
- [Way of working](#way-of-working)
- [Team](#team)

---

## Features

**On the robot**

- Animated face on the round display and RGB LED patterns
- Five touch pads as shortcuts: **Pet**, **Breathe**, **Pause / Resume**, **Cheer me up**, **Stop**
- Reactions to tilt (eyes follow), a gentle shake (dizzy) and being picked up (surprised)
- Falls asleep after a few minutes without interaction and wakes on touch or motion
- Keeps working without Wi-Fi; Stop always works

**On the website**

- Pick one of six emotions: happy, sad, stressed, angry, tired, bored
- ESIR responds on both the robot and the website:

| Emotion | Robot | Website |
| --- | --- | --- |
| Happy | Smile and celebration | Cheering message and hearts |
| Sad | Gentle eyes and a heart | Reassuring message and hearts |
| Stressed | Breathing cue with pulsing LEDs | Guided breathing animation in sync with the robot |
| Angry | Calm face and slow lights | Acknowledges the feeling and offers a pause |
| Tired | Sleepy eyes and soft light | Suggests a short break with a timer |
| Bored | Curious eyes and playful lights | Suggests a simple activity |

- Live robot status: online or offline, current activity, last interaction
- Pause, Resume and Stop controls
- Public company pages: about ESIR, team and progress

## Tech stack

| Part | Technology |
| --- | --- |
| Website (frontend) | Blazor Server (Interactive Server render mode) |
| Backend | ASP.NET Core on .NET 10, Minimal API for the robot endpoints |
| Robot hardware | Arduino MKR WiFi 1010 + Arduino Opla (MKR IoT Carrier) |
| Firmware | Arduino C++ with PlatformIO |
| Robot ↔ server | HTTPS + JSON, the robot polls the server |
| Body | 3D-printed shell |
| Hosting | Docker, deployed with Dokploy (planned) |
| Planning | GitHub Projects and Issues |

## How it works

```
 Browser  ──SignalR──>  ASP.NET Core (Blazor Server + Minimal API)  <──HTTPS polling──  Arduino robot
                              │
                     in-memory services:
                     command queue, robot status
```

1. A user picks an emotion on the website. The Blazor page calls a C# service, which queues a command for the robot.
2. The robot asks the server for its next command about once per second.
3. The robot runs the matching reaction and reports its status back (received, running, completed, breathing phase).
4. The website updates live and shows only what the robot has confirmed.

There is no database in the first version: commands and robot status live in memory and nothing personal is stored.

## Repository structure

Planned layout (folders are added as the project grows):

```
src/Esir.Web/            Blazor Server app + robot API (Minimal API)
tests/Esir.Tests/        xUnit tests
tools/RobotSimulator/    console app that behaves like the robot, for testing without hardware
firmware/                PlatformIO project for the Arduino MKR WiFi 1010 + Opla
cad/                     3D models and print files for the robot body
docs/                    pitch, API contract, decisions, test checklist
Dockerfile               container image (planned)
README.md
```

## Getting started

### Prerequisites

- [.NET 10 SDK](https://dotnet.microsoft.com/download)
- Visual Studio 2026 or VS Code with the C# Dev Kit
- For the firmware: VS Code with the [PlatformIO](https://platformio.org/) extension
- Optional: [Docker](https://www.docker.com/) to build and run the container locally

### Run the website locally

```bash
git clone https://github.com/Mercantec-GHC/ESIR.git
cd ESIR
dotnet run --project src/Esir.Web
```

Then open the URL shown in the terminal (for example `https://localhost:5001`).

### Run the tests

```bash
dotnet test
```

### Test without the robot

Start the simulator in a second terminal. It polls the local server and acts like the robot:

```bash
dotnet run --project tools/RobotSimulator
```

## Configuration

Settings come from `appsettings.json`, `appsettings.Development.json` (local) or environment variables (Docker and Dokploy). Never commit real secrets.

| Setting | Environment variable | Example | Purpose |
| --- | --- | --- | --- |
| Device ID | `Devices__0__Id` | `esir-01` | The robot that may connect |
| Device key | `Devices__0__Key` | random 32-character secret | The robot sends this in the `X-Device-Key` header |
| Control password | `Site__ControlPassword` | team password | Protects the control page |
| Command lifetime | `Commands__TtlSec` | `30` | Seconds before an unanswered command expires |
| Offline after | `Robot__OfflineAfterSec` | `5` | Seconds without a poll before the robot shows as offline |

## Robot API

All robot calls need the header `X-Device-Key`. JSON uses camelCase; enum values are lowercase strings.

| Method | Path | Purpose |
| --- | --- | --- |
| `GET` | `/api/v1/devices/{deviceId}/commands/next` | The robot gets its next command (`200`) or nothing (`204`) |
| `POST` | `/api/v1/devices/{deviceId}/status` | The robot reports its state and acknowledges commands |
| `GET` | `/healthz` | Health check for Docker and Dokploy |

Example command:

```json
{ "id": "c-8f2a41", "type": "emotion", "emotion": "stressed" }
```

The full contract is in `docs/api.md` (planned).

## Firmware

The firmware lives in `firmware/` and is built with PlatformIO.

1. Copy `firmware/include/secrets.example.h` to `firmware/include/secrets.h` and fill in Wi-Fi name, password, server address, device ID and device key. `secrets.h` is git-ignored.
2. Connect the MKR WiFi 1010 with USB.
3. Build and upload from VS Code (PlatformIO: Upload) or run:

```bash
cd firmware
pio run --target upload
pio device monitor
```

## Deployment

Deployment with Docker and Dokploy comes later in the project. The plan:

- A multi-stage `Dockerfile` (.NET 10 SDK to build, ASP.NET Core runtime to run), running as a non-root user on port 8080
- A Dokploy application connected to this GitHub repository, deploying automatically on every push to `main`
- Secrets set as environment variables in Dokploy, HTTPS through Dokploy's domain settings
- One replica only, because Blazor Server keeps each user's page state in memory

Build and run the container locally:

```bash
docker build -t esir-web .
docker run -p 8080:8080 -e Devices__0__Id=esir-01 -e Devices__0__Key=dev-key esir-web
```

## Way of working

- Tasks are GitHub Issues, tracked on the GitHub Project board.
- One branch per issue, for example `12-command-service`.
- Pull requests reference their issue (`Closes #12`) and get one review before merging into `main`.
- Day-to-day communication happens in Microsoft Teams.

## Team

- Patricia
- Victoria
- Yuliia

ESIR Robotics – student project at Mercantec, 2026.
