# yt-dlp-helper
a more intuitive way to download using yt-dlp

## Requirements
- Linux
- gcc compiler
- yt-dlp
- ffmpeg _<span style="color:gray;">(for .opus converting)</span>_
- Write permission to the destination directory

## How to run the program
1. Clone or download the repository

2. Compile the .c file by running:

    ```bash
    gcc yt-dlp-helper.c -o yt-dlp-helper
    ```
    > I recommend copying the executable program to `/usr/local/bin/` <br>
    > so you can run it from any directory

    <br>

3. Run the executable following the format:

    ```bash
    ./yt-dlp-helper [video|audio] "URL" "path/to/save/" [optional_name]
    ```

## Example Usage

- Help

    ```bash
    ./yt-dlp-helper --help
    ```
    or
    ```bash
    ./yt-dlp-helper -h
    ```
    <br>

- Version

    ```bash
    ./yt-dlp-helper --version
    ```
    or
    ```bash
    ./yt-dlp-helper -v
    ```
    <br>

- Download audio only

    ```bash
    ./yt-dlp-helper audio "https://www.youtube.com/watch?v=example" "~/Music"
    ```
    <br>

- Download video with custom file name

    ```bash
    ./yt-dlp-helper video "https://www.youtube.com/watch?v=example" "~/Videos" "my_video"
    ```
    <br>

- Download playlist

    ```bash
    ./yt-dlp-helper audio "https://www.youtube.com/playlist?list=PLxxxxxx" "~/Music"
    ```

    > When downloading a playlist, do **not** put a custom name, the program ignores it; <br>
    > otherwise, it would be overwritten in every download and only the last one would <br>
    > be available for you

    > The program will create a folder `/path/to/save/[playlist name]/` and save <br>
    > all playlist content inside it, with filenames starting with the playlist index.

## Notes
- Ensure you have write permission for the specified save directory

- This program creates the destination directory if it doesn't exist

- Audio files are downloaded in .opus format

- Videos are downloaded in the format yt-dlp determines to be best