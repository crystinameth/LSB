#include <stdio.h>
#include <stdlib.h>

void encoder()
{
    /* open image and text file */

    FILE *img_pointer = fopen("test.bmp", "rb");
    if (img_pointer == NULL)
    {
        printf("Error opening image\n");
        exit(0);
    }
    FILE *file_pointer = fopen("text.txt", "r");
    if (file_pointer == NULL)
    {
        printf("Error opening text file\n");
        exit(0);
    }

    /* size comparison */

    fseek(img_pointer, 0, SEEK_END);
    int img_size = ftell(img_pointer);
    
    fseek(file_pointer, 0, SEEK_END);
    int file_size = ftell(file_pointer);

    if (file_size > (img_size - 54) / 9)
    {
        printf("Text file is too large to be encoded in the image\n");
        exit(0);
    }

    /* copy data from img to new_img */
    FILE *new_img_pointer = fopen("new_img.bmp", "wb");
    fseek(img_pointer, 0, SEEK_SET);
    fseek(new_img_pointer, 0, SEEK_SET);
    int i = 0;
    while (i < 54)
    {
        fputc(fgetc(img_pointer), new_img_pointer);
        i++;
    }

    /* encoding */
    // image header till 54 bytes ->do not disturb
    // stego key marks starting of encryption
    
    fseek(file_pointer, 0, SEEK_SET);     // move file pointer to start of file
    while (!feof(file_pointer))
    {   char ch=0;
        fread(&ch,1,1,file_pointer);
        char demo = 0;
       
        for (int i = 0; i < 8; i++) // writing lsb first and so on
        {
            unsigned char change=0;
            demo = ch & 1; // extracting lsb
            fread(&change,1,1,img_pointer); // read byte from image
           // fseek(new_img_pointer, -1, SEEK_CUR); 
            change = change & 0xfe;
            change = change | demo;
            fwrite(&change, 1, 1, new_img_pointer); // write lsb of ch to image
            ch = ch >> 1;
        }

    }

    
    /* copy rest of the image */
    while (!feof(img_pointer))
    {   char ch=0;
        fread(&ch,1,1,img_pointer);
        fwrite(&ch,1,1, new_img_pointer);
    }
    fclose(img_pointer);
    fclose(new_img_pointer);
    fclose(file_pointer);
    printf("Encryption successful\n");
}

void decoder()
{
    /* decoding */
    FILE *new_img_pointer = fopen("new_img.bmp", "rb");
    fseek(new_img_pointer, 54, SEEK_SET);
    char demo = 0;
    while (demo != (char)35)
    {
         demo = 0;
        for (int i = 0; i < 8; i++) // writing lsb first and so on
        {
            unsigned char ch = fgetc(new_img_pointer); 
            char temp = ch & 1;
            temp = temp << i;
            demo = demo | temp; // extracting lsb

        }
        printf("%c", demo);
    
    }
    printf("Decryption successful\n");
    fclose(new_img_pointer);

}

int main()
{
    // create menu for encryption decryption and exit
    int choice;
    do
    {
        /* code */
        printf("1. Encryption\n2. Decryption\n3. Exit\nEnter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1: // encryption
            printf("Encryption\n");
            encoder();
            break;
        case 2: // decryption
            printf("Decryption\n");
            decoder();
            break;
        case 3: // exit
            printf("Exit\n");
            break;
        default:
            printf("Invalid choice\n");
        }
    } while (choice == 1 || choice == 2);

    return 0;
}