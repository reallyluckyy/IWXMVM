<div align="center">
  <br><br>
  <a href="https://github.com/reallyluckyy/IWXMVM">
    <img src="https://github.com/reallyluckyy/IWXMVM/assets/7430330/9463f12e-f919-49b9-b6ce-94037df9a181" width="50%">
  </a>
  <br><br>
</div>

## About
An in-development editing mod for multiple Call of Duty games.

![iwxmvmscreenshot](https://github.com/reallyluckyy/IWXMVM/assets/7430330/9680c479-1ed3-4690-9c29-b99f4b1b392b)

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
- [`iw3`](iw3/) contains game-specific bindings for creating the IW3 version of the mod
