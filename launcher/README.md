# KStars extensions
## Launcher
Extension summary
-----------------
The purpose of this extension is to ease the workflow of using KStars/Ekos
with other installed software

Extension actions
-----------------
When launched this extension will:

1. Check that this configuration file exists and is valid.
2. Check that both the Ekos Scheduler and Capture Module are idle.
3. Optionsally - disconnect the INDI driver for the currently selected camera in the Capture Module.
4. Call the selected program executable.

It will then monitor for either the launched program to be closed by the user, or the Stop Plugin
button in the Ekos Manager to be clicked. It will then:

5. Close the launched program.
6. Reconnect the INDI camera driver that was previously disconnected.
