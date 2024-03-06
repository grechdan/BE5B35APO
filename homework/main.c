#include <stdio.h>
#include <stdlib.h>

typedef struct pixels{
    unsigned char r,g,b;
}pixels;

#define RGB_COMPONENT_COLOR 255


int Valuescond(int value){
    if(value > 0){
        if (value < 256){
            return value;
        }else{
            return 255;
        }
    }return 0;
}


int main(int argc, char* argv[]){


    FILE *fp = fopen(argv[1], "rb");

    int W, H, Maxcol;

    if (fscanf(fp, "P6 %d %d %d\n", &W, &H, &Maxcol) != 3) return 1;
    
    int SIZE = W*H;

    
    pixels *orig_img = (pixels*)malloc(SIZE * sizeof(pixels));
    if(!fread(orig_img, 3*W, H, fp)) return 1;

    
    fclose(fp);

    pixels* new_img = (pixels*)malloc(SIZE * sizeof(pixels));

    
    int count[] = {0, 0, 0, 0, 0, 0};

    
    int i = 0, y;
    int value_red, value_green, value_blue;
    while(i < W){
        new_img[i] = orig_img[i];
        y = (0.2126*new_img[i].r + 0.7152*new_img[i].g + 0.0722*new_img[i].b + 0.5)/51;
        count[y]++;
        i++;
    }
    
    register short j = 2;
    while(i < SIZE-W){
        
        new_img[i] = orig_img[i];
        y = (0.2126*new_img[i].r + 0.7152*new_img[i].g + 0.0722*new_img[i].b + 0.5)/51;
        count[y]++;
        i++;
        
        while(i < (W*j)-1){
            value_red = 5*orig_img[i].r - orig_img[i-1].r - orig_img[i+1].r - orig_img[i-W].r - orig_img[i+W].r;
            value_green = 5*orig_img[i].g - orig_img[i-1].g - orig_img[i+1].g - orig_img[i-W].g - orig_img[i+W].g;
            value_blue = 5*orig_img[i].b - orig_img[i-1].b - orig_img[i+1].b - orig_img[i-W].b - orig_img[i+W].b;
            new_img[i].r = Valuescond(value_red);
            new_img[i].g = Valuescond(value_green);
            new_img[i].b = Valuescond(value_blue);
            y = (0.2126*new_img[i].r + 0.7152*new_img[i].g + 0.0722*new_img[i].b + 0.5)/51;
            count[y]++;
            i++;
        }
        
        new_img[i] = orig_img[i];
        y = (0.2126*new_img[i].r + 0.7152*new_img[i].g + 0.0722*new_img[i].b + 0.5)/51;
        count[y]++;
        i++;
        j++;
    }

    while(i < SIZE){
        new_img[i] = orig_img[i];
        y = (0.2126*new_img[i].r + 0.7152*new_img[i].g + 0.0722*new_img[i].b + 0.5)/51;
        count[y]++;
        i++;
    }

    free(orig_img);
    
    fp = fopen("output.ppm", "wb");
    fprintf(fp, "P6\n%d\n%d\n255\n", W, H);
    fwrite(new_img, 3 * W, H, fp);
    fclose(fp);

    free(new_img);

    fp = fopen("output.txt", "wb");
    fprintf(fp, "%d %d %d %d %d", count[0], count[1], count[2], count[3], count[4] + count[5]);
    fclose(fp);
}