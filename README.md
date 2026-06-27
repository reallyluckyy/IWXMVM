<div align="center">
  <br><br>
  <a href="https://github.com/reallyluckyy/IWXMVM">
    <img src="https://github.com/reallyluckyy/IWXMVM/assets/7430330/9463f12e-f919-49b9-b6ce-94037df9a181" width="50%">
  </a>
  <br><br>
</div>

## About
An eternally in-development recording mod for multiple Call of Duty games, featuring keyframeable campaths, a fully rewindable video editor-like timeline and built-in ProRes video capturing support.

![iwxmvmscreenshot](https://github.com/reallyluckyy/IWXMVM/assets/7430330/9680c479-1ed3-4690-9c29-b99f4b1b392b)

## Supported Games

All game implementations use signature scanning to support as many game individual game versions as possible. Usually, the latest Steam version should work, if nothing else is specified in the table below.

| Game                            | Status         | Notes          |
| ------------------------------- | -------------- | -------------- |
| Call of Duty 4: Modern Warfare  | Well supported     | Reference Implementation. Should also mostly work with cod4x and [iw3xo](https://github.com/xoxor4d/iw3xo-dev). |
| Call of Duty: Modern Warfare 3  | Mostly functional  | Partial support for [Plutonium IW5](https://plutonium.pw/). |

The mod was built with [ReShade](https://reshade.me/) compatibility in mind, but some issues may remain.

## Requirements
- Visual Studio 2022
- DirectX SDK Jun10

## Building

First clone the repository:
```
git clone --recursive https://github.com/reallyluckyy/IWXMVM.git
```
Then build the included solution file using Visual Studio.

## Contributing

If you like the project and want to help out, feel free to submit a pull request!
You can either look at the [issues page](https://github.com/reallyluckyy/IWXMVM/issues) for what still needs to be done from our perspective or just bring your own ideas.
Theres also a bunch of TODOs littered throughout the codebase, which need attention at some point.

There are no style or structure guidelines yet so please just refer to the already existing code.

## Project Structure

The project is structured into the following sub-projects:
- [`core`](core/) contains the core mod logic
- [`iw3`](iw3/) contains game-specific bindings for creating the Call of Duty 4 version of the mod
- [`iw5`](iw5/) contains game-specific bindings for creating the Modern Warfare 3 version of the mod
