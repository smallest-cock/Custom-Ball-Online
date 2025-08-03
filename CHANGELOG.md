# Changelog

## v2.2.4
- Added potential fix for crashes

## v2.2.3
- Updated internal SDK to match latest RL update (v2.54)

## v2.2.2
- Added a basic JSON creator tool in settings
  - Should help beginners who don't know how to create JSON files for their skins
  - Thanks to @GsRClans for the idea & initial implemention 👐🦒

## v2.2.1
- Automated the release process 👷
  - Will probably lead to faster updates in the future 🥳
### AlphaConsole version
- Fixed bug in finding textures when `.json` filepath or texture name contains a dash `-`
- General improvements and fixes

## v2.2.0
- Added a better version for people who don't use AlphaConsole

## v2.1.5
- Updated internal SDK to match latest RL update (v2.53)

## v2.1.4
- Improved plugin initialization
- Updated internal SDK to match latest RL update (v2.51)

## v2.1.3
- Updated internal SDK to match latest RL update (v2.50)
- Improved method of applying textures
- Added a notification in settings for when the plugin needs an update

## v2.1.2
- Added an experimental fix for mid-game crashes due to the latest bakkesmod / RL update
    - It's possible the crashes were intentionally caused by bakkesmod

## v2.1.1
- Updated internal SDK to match latest RL update (v2.48)
- Fixed minor bug in settings footer

## v2.1.0
- Added JPEG image support
    - AlphaConsole doesn't support JPEG images on its own 😔
- Updated settings UI
- Improved efficiency for getting ball texture data
- Updated internal SDK to match latest RL update (v2.47)

## v2.0.9
- updated internal SDK to match latest RL update (v2.46)

## v2.0.8
- fixed bug preventing JSON files from being found if they're in a subfolder
- fixed issue with saving textures to make applying textures more efficient

## v2.0.7
- updated to support latest AlphaConsole 2.0 update

## v2.0.6
- updated SDK to match latest RL update
- turned off unnecessary logging

## v2.0.5
- fixed bug where texture isn't applied in casual intermission maps (the map you join when you press "Ready" after a casual match)
- updated SDK

## v2.0.4
- Made applying ball textures more efficient in certain situations
    - Which made some function hooks unnecessary... so those were removed
- Removed the need to "hook" AlphaConsole cvar

## v2.0.3
- Fixed bug causing custom ball texture to be removed when skipping around in a replay
- Fixed bug preventing custom ball texture from being applied to ball dissolve effect when goal is scored (hopefully)
- Cleaned up code a bit

## v2.0.1
- Fixed bug causing plugin to remain active even when it's disabled in settings
- Improved settings UI a bit
- Refactored code to be more organized

## v2.0.0
v2 released! 🙌🥳🎉

- Now supports latest version of AlphaConsole
- Now enables ball texture directly instead of attempting to manipulate the AlphaConsole menu (which was hacky and unreliable)