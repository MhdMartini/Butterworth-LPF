The code contained takes in a grayscale image and applied a butterworth low-pass filter to it.
The executable can either output two images of the normalized spectrum and the filtered image,
or can calculate the total energy of the filtered image and print it to console.

Source:
    imgIo.c
    Four2.c
    butterworth.c

Executable:
    butterworth

Build:
    Executable was built with:
        gcc butterworth.c imgIo.c Four2.c -lm -o butterworth

Run:
    Get the help menu by calling the executable with the help flag:
        ./butterworth --help

    Help menu:
        """
        Please run the butterworth LPF as follows:
            ./butterworth <image_path> <power>

            <image_path>    Full path to an image
            <power>         1 for only calculating the spectrum of the input image.
                            0 for outputting images for the spectrum and filtered image
        """
