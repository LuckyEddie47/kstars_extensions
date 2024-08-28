# kstars_extensions
This is a multi-project repository for extensions for the KStars/Ekos/INDI application family

From the KStars handbook...

# Extensions
## Introduction

Extensions are small programs that can be added to interact with Kstars/Ekos/INDI in order to provide extra functions and features.
Note

Extensions are separate from Kstars/Ekos/INDI. They are not provided as part of this software. Only a means to call them is provided for convenience. Make sure that you understand the requirements and risks of using an extension.
## Requirements

Each extension must consist of at least 2 files, with an optional third icon file.
- The program file. This is any executable file that will run on the users system. The user under which KStars is running must have execute permission on this file.
- The configuration file. This is a plain text file that defines the required and optional inputs for the extension. It must be named the same as the extension executable with the addition of a .conf file name extension.

   It is mandatory that the configuration file contains a line starting with minimum_kstars_version=n.n.n where n.n.n is the lowest version of KStars that supports the extension e.g. 3.7.3

   Optionally (and non-preferred) the configuration file may contain a line starting with runDetached=true. If present this line makes the extension run independently from KStars. Once it has been started the extension can not pass status information back to Ekos. This should only be used by extensions that are required to continue to run after KStars has closed. The extension must also provide its own user interface.

   Additionally the configuration file can contain any other parameters that the extension author decides and free text.
- The icon file, if present, must be named the same as the extension executable with the addition of the appropriate file name extension for the image format, .jpg, .bmp, .gif, .png and .svg are supported. If provided this icon will be used in the extension selection ComboBox. If an icon is not provided a default icon will be used instead. 

All files of the extension (executable, configuration, and optional icon) must be copied into the KStars writable data location /extensions e.g. ~/.local/share/kstars/extensions

Extensions that are present, have a valid configuration file, and have a minimum KStars version no higher than the current installation will be detected when Ekos is started. If there are no valid extensions detected the extension UI element will not be displayed. The extension UI elements are a ComboBox showing the name and icon of each detected extension, and a start/stop button. If the extension fails to close within 10 seconds of the stop button being clicked, it becomes re-enabled as an abort button that will force close the extension. Only one extension can be used at a time.
## Development

The following describes additional points for developers of extensions.

- Some extension sources including an example skeleton extension are available in the KStars Extension Github

- Each extension must check for the existence and validity of its own configuration file. The minimum_kstars_version configuration file entry must be checked against an internal reference to confirm that the configuration file matches the extension requirements. See the skeleton example.

- The calling KStars version is passed to the extension as arg(1) in the launching QProcess call.

- All interaction with KStars/Ekos/INDI should be via the DBus interface. See the skeleton example. Useful tools for interrogating, monitoring and understanding DBus include D-Feet and Bustle

   It may appear on initial investigation that using the Qt DBus Adaptors system would be much easier than direct use of the Qt DBus Interfaces/Messages and KStars does provide the required xml definitions. However currently there is heavy use of custom types, the definitions of which are combined with other information in the KStars sources. This results in a large set of files from KStars that require inclusion within an extension in order to make use of the Qt DBus Adaptors. Hopefully this will be addressed in the future. 
