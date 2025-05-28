# yt-dlp-helper
A lightweight open-source C wrapper for yt-dlp that simplifies YouTube media downloads. <br>
Custom filenames | Playlist support | Auto-directory creation.

## Requirements
- Linux
- Build tools: `gcc`
- Depedencies:
    - yt-dlp (latest)

    - ffmpeg _(for audio conversion to .opus)_
    
    > Tip: install dependencies with:
    > ```bash
    > sudo apt install yt-dlp ffmpeg  # Debian/Ubuntu
    > ```

## Installation
1. Clone or download the repository

    ```bash
    git clone https://github.com/samuelleonildo/yt-dlp-helper.git
    cd yt-dlp-helper
    ```

2. Compile the .c file by running:

    ```bash
    gcc yt-dlp-helper.c -o yt-dlp-helper
    ```
    > I recommend moving the executable program to `/usr/local/bin/` <br>
    > using `sudo mv yt-dlp-helper /usr/local/bin/` to run it from any directory

## Usage

- ### Basic Syntax

    ``` bash
    yt-dlp-helper [video|audio] "URL" "path/to/save/" [optional_name]
    ```

- ### Examples

    <table style="border:none; border-collapse: collapse; width: 100%;">
        <tr>
            <td style="border:none; padding-right: 40px;"><strong>Command</strong></td>
            <td style="border:none;"><strong>Description</strong></td>
        </tr>
        <tr>
            <td style="border:none; padding-right: 40px;"><code>yt-dlp-helper --help</code></td>
            <td style="border:none;">Show help message</td>
        </tr>
        <tr>
            <td style="border:none; padding-right: 40px;"><code>yt-dlp-helper audio "https://youtu.be/..." "~/Music"</code></td>
            <td style="border:none;">Downloads audio (.opus)</td>
        </tr>
        <tr>
            <td style="border:none; padding-right: 30px;"><code>yt-dlp-helper video "https://youtu.be/..." "~/Videos" "my_video"</code></td>
            <td style="border:none;">Saves video with custom name</td>
        </tr>
    </table>  

- ### Download playlist

    ```bash
    ./yt-dlp-helper audio "https://www.youtube.com/playlist?list=PLxxxxxx" "~/Music"
    ```

    > When downloading a playlist, do **not** put a custom name, the program ignores it; <br>
    > otherwise, it would be overwritten in every download and only the last one would <br>
    > be available for you

    > The program will create a folder `/path/to/save/[playlist name]/` and save <br>
    > all playlist content inside it, with filenames starting with the playlist index.

## Notes
- Auto-creates missing directories
- Audio defaults to .opus
- Videos are downloaded in the format yt-dlp determines to be best
- Ensure write permission for target directories

## License

This project is licensed under the **MIT License**

## Author

samuelleonildo
<br>
[github.com/samuelleonildo](https://github.com/samuelleonildo/)