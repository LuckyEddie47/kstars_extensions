# KStars extensions
## Siril_EAA

Extension summary
-----------------
The purpose of this extension is to use the EAA live stacking abilities of Siril as a provider to  KStars. Capture will provide the individual subs, Siril in headless mode will stack them on the fly, and return the result to KStars for display in the Ekos Setup tab preview window.

Configuration
-------------
The Siril_EAA.conf file is used to configure the extension for your system and requirements. The configuratin file contains the following lines for setting up your session:

- siril_path=

This line should contain the full file path to your Siril executable eg: siril_path=/home/pi/Applications/Siril-1.3.0-beta.AppImage
- dark_path=

This line is optional. If uses it should contain the full file path to a master dark calibration file that you wish to use with the live stacking. This could be a Siril or KStars generated dark master. As usual with stacking the dark master must be the same dimensions as the light image being captured.
- flat_path=

This line is optional. If uses it should contain the full file path to a master flat calibration file that you wish to use with the live stacking. This could be a Siril generated flat master. As usual with stacking the flat master must be the same dimensions as the light image being captured. **Do not** use without also using a dark master or it will have a negative effect.
- registration_mode="rotate"

This line is optional. It defaults to setting the stacking registration mode to allow for rotation as well as shift. This takes slightly longer to calculate so if performance is an issue this can be set to an empty string eg: registration_mode="" or deleted from the configuration file altogether to disable rotational alignment.

Use
---
Use KStars/Ekos as normal to set up your observation. 
Live stacking is performed using the settings made in the Capture Module. Configure the settings as if you were going to create a Capture job but **do not** add a job to the sequence queue.
Once Capture is configured, switch to the Setup Tab, select and start the Siril_EAA extension.
The extension will start Siril in headless mode and then start Capturing. Each captured frame will be passed to Siril, optionally calibrated, and stacked. The resulting stacked image will be passed to the Preview window on the Setup Tab.
When finished, click the extension stop button to resume normal Capture mode.

Issues / Limitations
--------------------
Siril must be installed and functional on the same computer that KStars/Ekos is running on. Siril must be version 1.3.0 or later. Remote saving is not supported.

Siril's live stacking mode currently only supports alignment and rotational registration, not scaling.

Capture is repeated started in preview mode to generate the individual sub frames. As such the sequence queue must be empty before starting. The extension **will not** empty the queue for you as this is deemed to be a potential for data loss.

The Directory and Format fields of Capture are read to locate the sub-frames. The Format **must not** contain any tags in the directory path portion that are set at save time (ie: %D, %C or %P). These are usable in the file name portion of the Format.

Siril will create temporary files in the specified storage directory. These will be removed when the extension closes. There will also be a live_stack_00001.fit file created that contains your live stacked image.
