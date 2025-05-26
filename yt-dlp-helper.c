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
    return (strstr(url, "list=") != NULL);
}


int main(int argc, char* argv[])
{
    if (argc != 4 && argc != 5)
    {
        printf("format: %s [video|audio] \"URL\" \"/path/to/save\" [optional_name]\n", argv[0]);
        return 1;
    }

    // checking if yt-dlp is installed
    if (system("command -v yt-dlp > /dev/null 2>&1") != 0)
    {
        printf("error: yt-dlp not found. Please install yt-dlp first.\n");
        return 1;
    }
    // checking if ffmpeg is installed
    if (system("command -v ffmpeg > /dev/null 2>&1") != 0)
    {
        printf("error: ffmpeg not found. Please install ffmpeg first.\n");
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
        printf("invalid mode. Use [video|audio]\n");
        return 1;
    }
    if (!is_valid_url(url))
    {
        printf("invalid URL: \"%s\". URL must start with http:// or https://\n", url);
        return 1;
    }
    if (custom_name && is_playlist_url(url))
    {
        printf("error: custom_name cannot be used when downloading a playlist.\n");
        return 1;
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
        printf("error: %s exists but is not a directory.\n", dir);
        return 1;
    }


    // all about command below
    char* command_format = NULL;
    size_t size_needed = 0;

    if (custom_name)
    {
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
        if (strcmp(mode, "audio") == 0)
        {
            command_format = "yt-dlp -x --audio-format opus \"%s\" -o \"%s/%%(playlist)s/%%(playlist_index)03d - %%(title)s.%%(ext)s\"";
        }
        else // video
        {
            command_format = "yt-dlp \"%s\" -o \"%s/%%(playlist)s/%%(playlist_index)03d - %%(title)s.%%(ext)s\"";
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
        printf("error executing yt-dlp command.\n");
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