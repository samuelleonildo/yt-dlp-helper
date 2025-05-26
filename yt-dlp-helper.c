#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h> // for mkdir
#include <sys/types.h> // for mkdir
#include <errno.h> // for mkdir errors
#include <sys/wait.h> // for WIFEXITED and WEXITSTATUS

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif


void print_help(const char* program_name)
{
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("       %s [video|audio] URL DOWNLOAD_PATH [CUSTOM_NAME]\n\n", program_name);
    printf("A helper tool for yt-dlp to download media content from youtube.\n\n");
    
    printf("Options:\n");
    printf("  --help, -h       Show this help message and exit\n");
    printf("  --version, -v    Show version information\n\n");
    
    printf("Arguments:\n");
    printf("  mode             Download mode: 'video' or 'audio'\n");
    printf("  URL              URL of the video/audio to download (must start with http:// or https://)\n");
    printf("  DOWNLOAD_PATH    Path where the file(s) will be saved\n");
    printf("  CUSTOM_NAME      Optional custom filename (without extension and special characters)\n\n");
    
    printf("Examples:\n");
    printf("  %s video \"https://youtube.com/watch?v=dQw4w9WgXcQ\" \"/home/user/Videos\"\n", program_name);
    printf("  %s audio \"https://youtube.com/watch?v=dQw4w9WgXcQ\" \"/home/user/Music\" \"my_song\"\n\n", program_name);
    
    printf("Note:\n");
    printf("  - Creates destination directory if needed\n");
    printf("  - Automatically handles playlists\n");
    printf("  - Requires yt-dlp and ffmpeg to be installed\n");
}


void print_version(void)
{
    printf("yt-dlp helper v1.1\n");
    printf("Copyright (c) 2025 samuelleonildo\n");
    printf("License: MIT\n\n");
    printf("Wrapper for yt-dlp to simplify media downloads.\n");
}


int is_valid_mode(const char* mode)
{
    return (strcmp(mode, "audio") == 0 || strcmp(mode, "video") == 0);
}


int is_valid_url(const char* url)
{
    int http = strncmp(url, "http://", 7);
    int https = strncmp(url, "https://", 8);

    return (http == 0 || https == 0);
}


int is_playlist_url(const char* url)
{
    return ((strstr(url, "list=") != NULL) || (strstr(url, "playlist?") != NULL));
}


void sanitize_filename(char* filename)
{
    const char* invalid = "\\/:*?\"<>|";
    while (*filename)
    {
        if (strchr(invalid, *filename))
        {
            *filename = '_';
        }
        filename++;
    }
}


int main(int argc, char* argv[])
{
    // Check for help or version arguments
    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            print_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
        {
            print_version();
            return 0;
        }
    }

    if (argc != 4 && argc != 5)
    {
        fprintf(stderr, "Invalid number of arguments.\n\n");
        print_help(argv[0]);
        return 1;
    }

    // checking if yt-dlp is installed
    if (system("command -v yt-dlp > /dev/null 2>&1") != 0)
    {
        fprintf(stderr, "error: yt-dlp not found. Please install yt-dlp first.\n");
        return 1;
    }
    // checking if ffmpeg is installed
    if (system("command -v ffmpeg > /dev/null 2>&1") != 0)
    {
        fprintf(stderr, "error: ffmpeg not found. Please install ffmpeg first.\n");
        return 1;
    }

    // instanciating arguments
    char* mode = argv[1];
    char* url = argv[2];

    char dir[PATH_MAX];
    snprintf(dir, sizeof(dir), "%s", argv[3]);

    char* custom_name = (argc == 5) ? argv[4] : NULL;


    // validation
    if (!is_valid_mode(mode))
    {
        fprintf(stderr, "invalid mode: \"%s\". Allowed values are \"video\" or \"audio\"\n", mode);
        return 1;
    }
    if (!is_valid_url(url))
    {
        fprintf(stderr, "invalid URL: \"%s\". URL must start with http:// or https://\n", url);
        return 1;
    }
    if (is_playlist_url(url))
    {
        fprintf(stderr, "warning: playlist detected - using automatic naming\n");
        custom_name = NULL;
    }


    // removing '/' at the end of the dir path, if present
    size_t dir_len = strlen(dir);
    if (dir_len > 0 && dir[dir_len - 1] == '/')
    {
        dir[dir_len - 1] = '\0';
    }

    // creating dir if necessary
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        if (mkdir(dir, 0755) == 0) // 'owner' can read, write and execute | 'groups' and 'others' can read and execute
        {
            printf("created dir: %s\n", dir);
        }
        else
        {
            perror("error creating dir");
            return 1;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "error: %s exists but is not a directory.\n", dir);
        return 1;
    }


    // all about command below
    char* command_format = NULL;
    size_t size_needed = 0;

    if (custom_name)
    {
        sanitize_filename(custom_name); // sanitizing invalid characters

        // truncating custom_name to 100 char
        if (custom_name && strlen(custom_name) > 100)
        {
            fprintf(stderr, "warning: custom name truncated to 100 characters\n");
            custom_name[100] = '\0';
        }

        if (strcmp(mode, "audio") == 0)
        {
            command_format = "yt-dlp -x --audio-format opus \"%s\" -o \"%s/%s.%%(ext)s\"";
        }
        else // video
        {
            command_format = "yt-dlp \"%s\" -o \"%s/%s.%%(ext)s\"";
        }

        size_needed = snprintf(NULL, 0, command_format, url, dir, custom_name) + 1;
    }
    else
    {
        if (is_playlist_url(url))
        {
            if (strcmp(mode, "audio") == 0)
            {
                command_format = "yt-dlp -x --audio-format opus \"%s\" -o \"%s/%%(playlist)s/%%(playlist_index)03d - %%(title)s.%%(ext)s\"";
            }
            else // video
            {
                command_format = "yt-dlp \"%s\" -o \"%s/%%(playlist)s/%%(playlist_index)03d - %%(title)s.%%(ext)s\"";
            }
        }
        else
        {
            if (strcmp(mode, "audio") == 0)
            {
                command_format = "yt-dlp -x --audio-format opus \"%s\" -o \"%s/%%(title)s.%%(ext)s\"";
            }
            else // video
            {
                command_format = "yt-dlp \"%s\" -o \"%s/%%(title)s.%%(ext)s\"";
            }
        }

        size_needed = snprintf(NULL, 0, command_format, url, dir) + 1;
    }

    // tryna malloc
    char* command = malloc(size_needed);
    if (!command)
    {
        perror("command buffer malloc failed");
        return 1;   
    }

    if (custom_name)
    {
        snprintf(command, size_needed, command_format, url, dir, custom_name);
    }
    else
    {
        snprintf(command, size_needed, command_format, url, dir);
    }

    // all about running the command below
    printf("running command: %s\n", command);

    int status = system(command);
    free(command);

    if (status == -1)
    {
        perror("error executing yt-dlp command.");
        return 1;
    }
    else
    {
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                printf("command executed successfully.\n");
            }
            else
            {
                printf("yt-dlp exited with code %d.\n", exit_status);
                return 1;
            }
        }
        else
        {
            printf("yt-dlp did not terminate normally.\n");
            return 1;
        }
    }

    return 0;
}