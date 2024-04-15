# KStars extensions
## siril_eaa

Extension summary
-----------------
The purpose of this extension is to use the EAA live stacking abilities of Siril as a provider to  KStars. All setup and control will be within KStars. Capture will provide the individual subs, Siril in headless mode will stack them on the fly, and return the result to KStars for display in the Ekos Setup tab preview window.

Usage
-----------------
Use KStars/Ekos as normal to set up your observation. 
Live stacking is performed using the settings made in the Capture Module. Configure the settings as if you were going to create a Capture job but **do not** add a job to the list.
Once Capture is configured, switch to the Setup Tab, select and start the Siril_EAA extension.
The extension will start Siril and then start Capturing. Each captured frame will be passed to Siril and stacked. The resulting stacked image will be passed to the Preview window on the Setup Tab.
When finished, click the extension stop button to resume normal Capture mode.
