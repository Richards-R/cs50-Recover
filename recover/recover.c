#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Accept a single command-line argument
    if (argc != 2)
    {
        printf("Usage: ./recover FILE\n");
        return 1;
    }

    // Number of bytes in .jpeg chunk
    const int CHUNK_SIZE = 512;

    // Open the memory card
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open media.\n");
        return 1;
    }

    // Keep track of number of jpegs created
    int counter = 0;

    // Initialise receiving file
    FILE *img = NULL;

    // Create a buffer for a block of data
    uint8_t buffer[CHUNK_SIZE];

    // Reserve memory for filenames
    char *outputdata = malloc(8 * sizeof(char));

    // While there's still data left to read from the memory card
    while (fread(&buffer, CHUNK_SIZE, 1, card) == 1)
    {
        // Create the outputfile
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // Close previous file before opening next
            if (counter > 0)
            {
                fclose(img);
            }
            // Number and write new files, add to counter
            sprintf(outputdata, "%03i.jpg", counter);
            img = fopen(outputdata, "w");
            counter++;
        }

        // Only write to file if already recognised as jpeg
        if (counter > 0)
        {
            fwrite(&buffer, CHUNK_SIZE, 1, img);
        }
    }

    // Release memory and close files
    free(outputdata);
    fclose(card);
    fclose(img);
    return 0;
}
