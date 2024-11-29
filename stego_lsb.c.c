#include <stdio.h>   // For file I/O operations
#include <stdlib.h> // For dynamic memory allocation

#define WIDTH 512  // Define the width of the image
#define HEIGHT 512 // Define the height of the image

// Function Prototypes
int readPGMText(const char *filename, unsigned char *pixels, int width, int height);
int writePGMText(const char *filename, unsigned char *pixels, int width, int height);
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height);
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height);
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height);

// Function to read a PGM image in text format
// It stores the pixel values of the image specified by 'filename' into the 'pixels' array
int readPGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "r");  // Open the file in read mode
    if (file == NULL) {
        return 1; // Return an error if the file cannot be opened
    }

    char buffer[16];
    // Read the header to check if it's a valid PGM format (P2 indicates a text-based PGM)
    if (fgets(buffer, sizeof(buffer), file) == NULL || buffer[0] != 'P' || buffer[1] != '2') {
        fclose(file);
        return 1; // Return an error if the format is not valid
    }

    // Skip comments (lines that start with #)
    do {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            fclose(file);
            return 1; // Error if there are issues reading the file
        }
    } while (buffer[0] == '#');

    int w, h, maxVal;
    // Read the image dimensions and the maximum pixel value (should be 255)
    sscanf(buffer, "%d %d", &w, &h); 
    if (fscanf(file, "%d", &maxVal) != 1 || w != width || h != height || maxVal != 255) {
        fclose(file);
        return 1; // Return an error if the dimensions don't match or the max value isn't 255
    }

    // Read the pixel values into the 'pixels' array
    for (int i = 0; i < width * height; i++) {
        if (fscanf(file, "%hhu", &pixels[i]) != 1) {
            fclose(file);
            return 1; // Return an error if there is an issue reading pixel data
        }
    }

    fclose(file);
    return 0; // Successfully read the image
}

// Function to write a PGM image in text format
// Writes the pixel values of the 'pixels' array to a file specified by 'filename'
int writePGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "w");  // Open the file in write mode
    if (file == NULL) {
        return 1; // Return an error if the file cannot be opened
    }

    // Write the header for a PGM text file
    fprintf(file, "P2\n# Created by stego_lsb.c\n%d %d\n255\n", width, height);

    // Write the pixel values to the file
    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%d\n", pixels[i]); // Write each pixel value on a new line
    }

    fclose(file);
    return 0; // Successfully saved the image
}

// Function to write a PGM image in binary format
// Writes the pixel values of the 'pixels' array to a file in binary format
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "wb"); // Open the file in binary write mode
    if (file == NULL) {
        return 1; // Return an error if the file cannot be opened
    }

    // Write the header for a PGM binary file
    fprintf(file, "P5\n# Created by stego_lsb.c\n%d %d\n255\n", width, height);

    // Write the pixel data to the file in binary format
    fwrite(pixels, sizeof(unsigned char), width * height, file);

    fclose(file);
    return 0; // Successfully saved the image in binary format
}

// Function to embed a secret image using the 4-bit LSB (Least Significant Bit) steganography algorithm
// It hides the secret image data into the cover image's least significant bits
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        // Update each pixel of the cover image to embed the secret in its LSB
        coverPixels[i] = (coverPixels[i] / 16) * 16 + (secretPixels[i] / 16);
    }
}

// Function to extract the secret image from the stego image using the 4-bit LSB extraction method
// It retrieves the hidden pixels from the least significant bits of the cover image
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        // Extract the LSB of each pixel in the cover image and store it in the output array
        outputPixels[i] = (coverPixels[i] % 16) * 16;
    }
}

// Main function
int main() {
    // File paths for cover image, secret image, stego image, and extracted secret image
    char cover_image[] = "D:/Users/User/OneDrive/Desktop/Complete web development bootcamp/Lab4_Q2/baboon.pgm";
    char secret_image[] = "D:/Users/User/OneDrive/Desktop/Complete web development bootcamp/Lab4_Q2/farm.pgm";
    char stego_image[] = "D:/Users/User/OneDrive/Desktop/Complete web development bootcamp/Lab4_Q2/stego_image_bin.pgm"; 
    char extracted_secret[] = "D:/Users/User/OneDrive/Desktop/Complete web development bootcamp/Lab4_Q2/extracted_secret.pgm"; 

    unsigned char *coverPixels, *secretPixels, *outputPixels;
    int coverWidth = WIDTH, coverHeight = HEIGHT, secretWidth = WIDTH, secretHeight = HEIGHT;

    // Allocate memory to store the cover image's pixels
    coverPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (coverPixels == NULL) {
        printf("Error: Memory allocation for coverPixels failed.\n");
        return 1;
    }

    // Read the cover image and store its pixel data in 'coverPixels'
    if (readPGMText(cover_image, coverPixels, coverWidth, coverHeight) != 0) {
        printf("Error: Unable to read cover image.\n");
        free(coverPixels);
        return 1;
    }

    // Allocate memory to store the secret image's pixels
    secretPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (secretPixels == NULL) {
        printf("Error: Memory allocation for secretPixels failed.\n");
        free(coverPixels);
        return 1;
    }

    // Read the secret image and store its pixel data in 'secretPixels'
    if (readPGMText(secret_image, secretPixels, secretWidth, secretHeight) != 0) {
        printf("Error: Unable to read secret image.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    // Check if the dimensions of the cover and secret images match
    if (coverWidth != secretWidth || coverHeight != secretHeight) {
        printf("Error: Image dimensions do not match.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    // Embed the secret image into the cover image using LSB steganography
    embedLSB(coverPixels, secretPixels, WIDTH, HEIGHT);

    // Save the modified cover image (now the stego image) in binary format
    if (writePGMBinary(stego_image, coverPixels, WIDTH, HEIGHT) != 0) {
        printf("Error: Unable to save stego image.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    // Allocate memory to store the extracted secret image's pixels
    outputPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (outputPixels == NULL) {
        printf("Error: Memory allocation for outputPixels failed.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    // Extract the secret image from the stego image
    extractLSB(coverPixels, outputPixels, WIDTH, HEIGHT);

    // Save the extracted secret image in text format
    if (writePGMText(extracted_secret, outputPixels, WIDTH, HEIGHT) != 0) {
        printf("Error: Unable to save extracted secret image.\n");
    }

    // Free dynamically allocated memory
    free(coverPixels);
    free(secretPixels);
    free(outputPixels);

    return 0;
}
