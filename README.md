# OBS Lightgun Flash Filter

OBS plugin that provides a flashing filter for lightgun games. Forked from the original implementation (https://github.com/puri-puri/OBS-Studio-Flash-Filter)
to provide a regular OBS plugin instead of requiring a modified OBS build.

Note that the filter requires tuning for each game to recognize the flashes correctly. It is very much recommended to try out the game ahead of time,
and adjust the filter color and threshold to make sure that it is effective in reducing flashing.

### Sample video (click to view on Youtube)

[![Youtube video: OBS Lightgun Flash Filter sample - Time Crisis (PSX)](https://img.youtube.com/vi/dI5os4Ajf4o/0.jpg)](https://www.youtube.com/watch?v=dI5os4Ajf4o)

# Installation

Download from the [Releases page](https://github.com/mooware/OBS-Studio-Flash-Filter/releases).

Extract into the root folder of your OBS installation.

# Usage

In OBS, select a video source, go to Filters, and click the plus sign in the lower left corner. The flash filter should be in the list.
Adjust the flash color and threshold, and make sure that it filters flashes to your satisfaction.

## Presets

A few presets for popular games:

| Game                     | Color                               |
| ------------------------ | ----------------------------------- |
| Point Blank (PSX)        | #DEDEDE (White/Grey, 222 222 222) |
| Project Horned Owl (PSX) | #000CC7 (Blue,	0 12 199)          |
| Time Crisis (PSX)        | #F8F8F8 (White, 248 248 248)      |
| Time Crisis 2 (PS2)      | #838383 (Grey, 131 131 131)       |
