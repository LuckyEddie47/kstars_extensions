# KStars extensions
## firecapture_launcher
Extension summary
-----------------
The purpose of this extension is to ease the workflow of using KStars/Ekos
for setup, followed by FireCapture for planetary imaging.

Extension actions
-----------------
When launched this extension will:

1. Check that this configuration file exists and is valid.
2. Check that the FireCapture startup script supports the --no-confirm switch, and if not backup the as startup.sh.bak and patch the startup.sh.
3. Check that both the Ekos Scheduler and Capture Module are idle.
4. Disconnect the INDI driver for the currently selected camera in the Capture Module.
5. Call the FireCapture start script.

It will then monitor for either FireCapture to be closed by the user, or the Stop Plugin
button in the Ekos Manager to be clicked. It will then:

6. Close FireCapture
7. Reconnect the INDI camera driver that was previously disconnected.
