# AuraDynamicsEngine
(RELEASE NAME: Kinetic Aura Dynamics)

![AuraDynamicsEngine](docs/images/hero.png)

AuraDynamicsEngine is a custom C++/DirectX 11 engine I've been building to get deeper into graphics programming and engine architecture. I use Unreal as well, but I wanted a project where I had to work directly with the rendering pipeline, input, cameras, resources, and the systems around them instead of having those parts handled for me.

This repo is a working checkpoint of the engine, not the finished architecture. Some systems are staying, some are being replaced, and there's newer foundation work that I'm keeping separate until it's far enough along to actually belong here.

## Current Features

- DirectX 11 rendering
- Procedurally generated sphere geometry
- Custom HLSL shaders
- Earth lighting driven by the Sun's world position
- Normal mapping, ocean specular, night lights, and animated clouds
- Textured Sun rendering
- Star and Milky Way space background
- Free-flight camera
- Earth focus and moving-target tracking
- Custom keyboard and mouse input
- Custom HUD panels and controls
- Early runtime editor/launcher

![Earth and Sun lighting](docs/images/earth-sun-lighting.png)

## Rendering

`SphereMesh` handles the reusable sphere geometry and common rendering work. Earth, Sun, and Space build their own behavior around it instead of putting planet-specific logic into the mesh itself.

Earth currently uses five texture inputs: day color, ocean/specular, night lights, normal mapping, and clouds. The shader uses the Sun's world-space position to work out which side of Earth is actually facing the light. The geometric normal controls the large-scale day/night split, while the normal map is used for smaller surface detail.

Earth has its own rotation, and `PlanetOrbit` handles its movement around the Sun.

## Camera

The camera currently has three modes: free flight, returning to focus, and tracking focus.

Free flight is the normal keyboard and mouse camera. Focus mode is built around Earth being a moving target. While the camera is returning, it first follows Earth's frame-to-frame movement and then closes the remaining distance relative to Earth. Once it arrives, it stays locked to the moving target.

### Controls

| Input | Action |
|---|---|
| `W A S D` | Camera movement |
| `Q / E` | Vertical movement |
| `Left Mouse + Move` | Camera look |
| `1` | Free-flight mode |
| `2` | Focus on Earth |
| `Tab` | Camera speed controls |

## HUD / Editor

The current HUD is my custom panel system. It handles reusable panel windows, buttons, text, images, mouse interaction, minimize/restore behavior, and the specialized panels built on top of it.

The current build has Camera Speed, CPU Data, System Status, and an Aura Editor entry panel. Aura Editor can open the larger Editor Launcher, which is where I've started experimenting with runtime authoring tools.

This HUD is working, but it isn't the UI architecture I want to keep building on forever. I'm keeping the current version stable while I work on the newer runtime UI system separately.

## Known Limitation

The current HUD can get panel overlap during aggressive window resizing. I started experimenting with centralized collision handling for it, but the extra behavior made the legacy system worse in other ways.

I rolled that attempt back and kept the stable layout instead. Since this UI architecture is already being replaced, I'd rather solve cross-panel layout in the new system than keep adding temporary systems to this one.

Normal operation at the intended window sizes is unaffected.

## Where It's Going

AuraDynamicsEngine started as an engine project, but the longer-term goal is bigger than graphics or game development. I want AuraDynamics to eventually grow into a broader programming and technology platform that can support different kinds of software and technical systems without every area becoming its own disconnected architecture.

One of the foundations I'm building toward that goal is a shared identity system based around `AuraID`. Instead of letting every future subsystem invent its own unrelated way to identify resources, commands, diagnostics, UI elements, services, and other objects, the idea is to give Aura systems a common identity contract.

That identity layer is also part of how I'm thinking about compatibility between systems. If separate Aura components can refer to things through stable shared identities and well-defined contracts, they have a better foundation for communicating without needing to know each other's internal implementation.

I'm not claiming that cross-system architecture is finished today. `AuraID` is an early foundation, and the surrounding error/result, diagnostics, command, runtime UI, and other platform systems are still being designed and integrated.

Most of that work isn't in this portfolio checkpoint yet. I would rather keep this repo centered on the engine that actually runs and show the newer architecture as it becomes real instead of filling the project with unfinished systems just to make it look bigger.

## Building

AuraDynamicsEngine is currently developed on Windows using C++, DirectX 11, HLSL, Win32, DirectXMath, and DirectX Tool Kit components.

I'm in the process of preparing the repo for a clean build outside of my normal development environment. Exact fresh-clone build instructions will be added once I've finished checking the dependencies and the assets that are allowed to ship with the public version.

## Third-Party Work

The engine uses third-party libraries and some external visual assets. I don't present those as my own work.

I'm reviewing those dependencies and assets before the public release so their original licenses, ownership, and redistribution requirements are clear.

## Development Process

I do use AI-assisted tools, including Microsoft Copilot, while working on AuraDynamicsEngine. I use them for things like debugging, reviewing code, researching unfamiliar areas, and pressure-testing/refactoring ideas.

I don't treat generated code as automatically correct. I still decide how the systems fit together, integrate the changes, build and run them, trace problems when something breaks, and roll changes back when they make the engine worse. I only want code in the project that I can follow and explain myself.

A big reason I'm building this engine in the first place is to get better at reasoning through C++, graphics code, ownership, data flow, and architecture instead of just getting something working.

## About the Project

AuraDynamicsEngine has changed a lot as I've gotten better at C++ and graphics programming. I've rewritten parts after understanding the responsibility better, moved things when they were living in the wrong layer, and learned that adding another abstraction isn't automatically an improvement.

The long-term idea behind AuraDynamics is to take what I learn building the engine and use those foundations beyond the engine itself. I want to build a technology company capable of working across different areas of software and technical development while keeping the underlying systems structured enough to work together.

That's why things like identity, dependency direction, shared contracts, and system boundaries matter to me even when the current engine doesn't need every part of that architecture yet.

I'm not trying to present AuraDynamicsEngine as a finished commercial engine or the larger Aura vision as something that's already built. This is the working system I have today, and I want the repo to show both what works now and how I'm approaching the engineering needed to take it further.

## License

AuraDynamicsEngine is currently published as a portfolio and development project. Unless a file or third-party component states otherwise, I am not currently granting a general license to copy, redistribute, modify, or use the original AuraDynamicsEngine source in another project.

Third-party libraries and assets remain covered by their own licenses and usage terms.

I may choose a formal software license for AuraDynamicsEngine in the future as the project and the larger AuraDynamics platform develop.