# KStars extensions
## Launcher
Extension summary
-----------------
The purpose of this extension is to ease the workflow of using KStars/Ekos
with other installed software

The program_path string in the .conf file must be set to call the program you require.
This extension can be renamed as you wish. Make sure you rename the executable, the .conf file, and optionally, the icon file identically. The naming is case sensitive.

Extension actions
-----------------
When launched this extension will:

1. Check that this configuration file exists and is valid.
2. Check that both the Ekos Scheduler and Capture Module are idle.
3. Call the selected program executable.

It will then monitor for either the launched program to be closed by the user, or the Stop Plugin
button in the Ekos Manager to be clicked. It will then:

4. Close the launched program.
