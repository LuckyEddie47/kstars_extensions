// Provides status' for KStars DBus calls
// enums copied from ekos.h in kstars 3.6.9

#ifndef EKOSSTATUS_H
#define EKOSSTATUS_H

typedef enum
{
    GUIDE_IDLE,
    GUIDE_ABORTED,
    GUIDE_CONNECTED,
    GUIDE_DISCONNECTED,
    GUIDE_CAPTURE,
    GUIDE_LOOPING,
    GUIDE_DARK,
    GUIDE_SUBFRAME,
    GUIDE_STAR_SELECT,
    GUIDE_CALIBRATING,
    GUIDE_CALIBRATION_ERROR,
    GUIDE_CALIBRATION_SUCCESS,
    GUIDE_GUIDING,
    GUIDE_SUSPENDED,
    GUIDE_REACQUIRE,
    GUIDE_DITHERING,
    GUIDE_MANUAL_DITHERING,
    GUIDE_DITHERING_ERROR,
    GUIDE_DITHERING_SUCCESS,
    GUIDE_DITHERING_SETTLE,
    GUIDE_UNKOWN
} GuideState;

typedef enum
{
    CAPTURE_IDLE,                /*!< no capture job active */
    CAPTURE_PROGRESS,            /*!< capture job sequence in preparation (temperature, filter, rotator) */
    CAPTURE_CAPTURING,           /*!< CCD capture running */
    CAPTURE_PAUSE_PLANNED,       /*!< user has requested to pause the capture sequence */
    CAPTURE_PAUSED,              /*!< paused capture sequence due to a user request */
    CAPTURE_SUSPENDED,           /*!< capture stopped since some limits are not met, but may be continued if all limits are met again */
    CAPTURE_ABORTED,             /*!< capture stopped by the user or aborted due to guiding problems etc. */
    CAPTURE_WAITING,             /*!< waiting for settling of the mount before start of capturing */
    CAPTURE_IMAGE_RECEIVED,      /*!< image received from the CDD device */
    CAPTURE_DITHERING,           /*!< dithering before starting to capture */
    CAPTURE_FOCUSING,            /*!< focusing before starting to capture */
    CAPTURE_FILTER_FOCUS,        /*!< not used */
    CAPTURE_CHANGING_FILTER,     /*!< preparation event changing the filter */
    CAPTURE_GUIDER_DRIFT,        /*!< preparation event waiting for the guider to settle */
    CAPTURE_SETTING_TEMPERATURE, /*!< preparation event setting the camera temperature */
    CAPTURE_SETTING_ROTATOR,     /*!< preparation event setting the camera rotator */
    CAPTURE_ALIGNING,            /*!< aligning before starting to capture */
    CAPTURE_CALIBRATING,         /*!< startup of guiding running before starting to capture */
    CAPTURE_MERIDIAN_FLIP,       /*!< only used as signal that a meridian flip is ongoing */
    CAPTURE_COMPLETE,            /*!< capture job sequence completed successfully */
    CAPTURE_UNKNOWN
} CaptureState;

typedef enum
{
    FOCUS_IDLE,
    FOCUS_COMPLETE,
    FOCUS_FAILED,
    FOCUS_ABORTED,
    FOCUS_WAITING,
    FOCUS_PROGRESS,
    FOCUS_FRAMING,
    FOCUS_CHANGING_FILTER,
    FOCUS_UNKNOWN
} FocusState;

typedef enum
{
    ALIGN_IDLE,                 /**< No ongoing operations */
    ALIGN_COMPLETE,             /**< Alignment successfully completed. No operations pending. */
    ALIGN_FAILED,               /**< Alignment failed. No operations pending. */
    ALIGN_ABORTED,              /**< Alignment aborted by user or agent. */
    ALIGN_PROGRESS,             /**< Alignment operation in progress. This include capture and sovling. */
    ALIGN_SUCCESSFUL,           /**< Alignment Astrometry solver successfully solved the image. */
    ALIGN_SYNCING,              /**< Syncing mount to solution coordinates. */
    ALIGN_SLEWING,              /**< Slewing mount to target coordinates.  */
    ALIGN_ROTATING,             /**< Rotating (Automatic or Manual) to target position angle. */
    ALIGN_SUSPENDED,            /**< Alignment operations suspended. */
    ALIGN_UNKNOWN
} AlignState;

typedef enum
{
    FILTER_IDLE,
    FILTER_CHANGE,
    FILTER_OFFSET,
    FILTER_AUTOFOCUS,
    FILTER_UNKNOWN
} FilterState;

typedef enum
{
    SCRIPT_PRE_JOB,     /**< Script to run before a sequence job is started */
    SCRIPT_PRE_CAPTURE, /**< Script to run before a sequence capture is started */
    SCRIPT_POST_CAPTURE,/**< Script to run after a sequence capture is completed */
    SCRIPT_POST_JOB,    /**< Script to run after a sequence job is completed */
    SCRIPT_UNKNOWN
} ScriptTypes;

typedef enum
{
    SCHEDULER_IDLE,     /*< Scheduler is stopped. */
    SCHEDULER_STARTUP,  /*< Scheduler is starting the observatory up. */
    SCHEDULER_RUNNING,  /*< Scheduler is running. */
    SCHEDULER_PAUSED,   /*< Scheduler is paused by the end-user. */
    SCHEDULER_SHUTDOWN, /*< Scheduler is shutting the observatory down. */
    SCHEDULER_ABORTED,  /*< Scheduler is stopped in error. */
    SCHEDULER_LOADING,  /*< Scheduler is loading a schedule. */
    SCHEDULER_UNKNOWN
} SchedulerState;

typedef enum
{
    PLUGIN_START_REQUESTED,
    PLUGIN_STARTED,
    PLUGIN_STOP_REQUESTED,
    PLUGIN_STOPPED,
} pluginState;

#endif // EKOSSTATUS_H
