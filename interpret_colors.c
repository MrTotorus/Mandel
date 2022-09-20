
#include "libBMP.h"
#include "helperFunctions.h"

#include <math.h>

char dat_path[PATH_MAX];
time_t mtime = 100000000;

int checkifnewer(const char *path, const struct stat *sb, int typeflag) {
    if (typeflag == FTW_F && sb->st_mtime > mtime) {
        mtime = sb->st_mtime;
        strncpy(dat_path, path, PATH_MAX);
    }
    return 0;
}

int main(void) {
    double image_width = 0;
    double image_height = 0;
    long maximal_recursions = 0;
    long minimal_recursions = 10000;
    char file_name[80];
    char bmp_path[100];
    char buffer[30];
    char *end;
   
    printf("Please enter datafile you want to transform into an image (without extension) or 'latest' / 'l'\n");
    scanf("%s", file_name);

    if (strcmp(file_name, "latest") && strcmp(file_name, "l")) {
        sprintf(dat_path, "data/%s.dat", file_name);
    } else {
        ftw("data", checkifnewer, 1);
        end = strstr(dat_path, ".");
        *end = '\0';
        strcpy(file_name, strstr(dat_path, "/")+1); 
        *end = '.';
    }

    printf("Please enter the width of the image\n");
    scanf("%lf", &image_width);
    image_width = floor(image_width);
    image_height = floor(image_width * 5/7);

	uint32_t *image_data = (uint32_t*) malloc(sizeof(uint32_t) * image_width * image_height);
    long tiefe;

    FILE *file;
    file = fopen(dat_path, "r");
    for (long i = 0; i < (int) (image_width * image_height); i++) {
        fgets(buffer,30,file);
        tiefe = strtoul(buffer,NULL,0);

        if (tiefe < minimal_recursions) {
            minimal_recursions = tiefe;
        } else if (tiefe > maximal_recursions) {
            maximal_recursions = tiefe;
        }

		image_data[i] = tiefe;

    }
    fclose(file);

    printf("\nminimal recursions: %ld, maximal recursions: %ld\n", minimal_recursions, maximal_recursions);
	
    for (long i = 0; i < (int) (image_width * image_height); i++) {
        tiefe = image_data[i];
    
		uint32_t r, g, b, h_value, s_value, v_value;
		h_value = map_value(tiefe%100, 0, 100, 0.0, 359.0); 
		//h_value = tiefe%359; 
		//h_value = map_value(pow(log(tiefe),3), 0, pow(log(N_MAX),3), 0.0, 359.0);
		//h_value = map_value(tiefe, 0.0, N_MAX/tiefe, 0.0, 359.0);
		
        s_value = 100;//map_value(tiefe%100, 0, 100, 0.0, 100.0); 
		v_value = 100;//map_value(tiefe%100, 0, 100, 0.0, 100.0);

		HSV_to_RGB(&r, &g, &b, h_value, s_value, v_value);
    
        image_data[i] = combine_color(r, g, b);
    }
    
    printf("\nPainting graph\n");
    
    sprintf(bmp_path, "images/%s.bmp", file_name);
	bmp_create(bmp_path, image_data, image_width, image_height);

	printf("\nSuccess!\n");
}