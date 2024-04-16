# KStars extensions
## kstars_backup

Extension summary
-----------------
The purpose of this extension is to provide a simple interface to allow the backup and restoration of KStars configuration files.

Configuration
-------------
The KS_backup.conf file is used to configure the extension for your system and requirements. The configuration file contains the following lines for setting up the extension:

- path=""

Each occurance of this line specifys a directory that is included in the backup eg. ~/.local/share/kstars

The default set of entries suffice for backing up KStars/Ekos/INDI but can be supplimented by additional entries to include other software as the user wishes.

Use
---
This extension **must not** be run when KStars/Ekos/INDI is in use. In order to prevent access conflicts this extension runs independently and **will close KStars** when launched. The user will be warned of this and given an opportunity to cancel. Additionally the status of the Scheduler and Capture modules will be confirmed first. The extension will restart KStars when exiting.

Additional directories may be added to the archive set in the GUI and pre-set directories (from the configuration file) can also be removed.

Backups are created as standard .tar.gz archives and as such can be opened / modified / extracted by normal archive tools. File paths are written in absolute mode so that when using the restore function of the extension all files can be correctly located regardless of where the archive is stored.

Archives are automatically named with: the KStars version, the computer name, the OS version, the CPU architecture, and a date/time stamp.

When requesting a restore process the user is prompted to create a backup of the same directories first to allow roll back in the event of an error.

Issues / Limitations
--------------------
It is not possible to calculate the compressed size of a backup before it is created. The uncompressed size of the source files and the available space of the file system are shown but it is the users responsibility to ensure that there is sufficient space available.

This extension makes use of the following common system utilities:
- tar
- df
- du

These **should** be available on every system which can run Ekos but if, for instance, a custom distribution is used and any of these are not available, then this extension will fail.

The default KStars configuration locations include the extensions directory holding this extension. Make sure that you do not restore a backup from a different CPU architechture eg. a Raspberry Pi system backup onto a X86_64 system. If this is done the extensions will not work. As a reminder of this the architecture is included in the archive filename.
