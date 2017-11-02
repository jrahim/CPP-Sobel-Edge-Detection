#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <iostream>
#include <cmath>

using namespace std;
#define PI 3.14159265


/** Read the JPEG image at `filename` as an array of bytes.
  Data is returned through the out pointers, while the return
  value indicates success or failure.
  NOTE: 1) if image is RGB, then the bytes are concatenated in R-G-B order
        2) `image` should be freed by the user
 */
static inline int
read_JPEG_file(char *filename,
               int *width, int *height, int *channels, unsigned char *(image[]))
{
  FILE *infile;
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  (void) jpeg_read_header(&cinfo, TRUE);
  (void) jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width, *height = cinfo.output_height;
  *channels = cinfo.num_components;
  // printf("width=%d height=%d c=%d\n", *width, *height, *channels);
  *image = malloc(*width * *height * *channels * sizeof(*image));
  JSAMPROW rowptr[1];
  int row_stride = *width * *channels;

  while (cinfo.output_scanline < cinfo.output_height) {
    rowptr[0] = *image + row_stride * cinfo.output_scanline;
    jpeg_read_scanlines(&cinfo, rowptr, 1);
  }
  jpeg_finish_decompress(&cinfo);

  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  return 1;
}


/** Writes the image in the specified file.
  NOTE: works with Grayscale or RGB modes only (based on number of channels)
 */
static inline void
write_JPEG_file(char *filename, int width, int height, int channels,
                unsigned char image[], int quality)
{
  FILE *outfile;
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }

  struct jpeg_error_mgr jerr;
  struct jpeg_compress_struct cinfo;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo,outfile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = channels;
  cinfo.in_color_space = channels == 1 ? JCS_GRAYSCALE : JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);
  JSAMPROW rowptr[1];
  int row_stride = width * channels;
  while (cinfo.next_scanline < cinfo.image_height) {
    rowptr[0] = & image[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, rowptr, 1);
  }
  jpeg_finish_compress(&cinfo);

  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}


int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: ./ex1 image_in.jpg image_out.jpg\n");
    return 1;
  }

  unsigned char *image;
  int width, height, channels;
  read_JPEG_file(argv[1], &width, &height, &channels, &image);


  if (channels == 3) {
    unsigned char *image2;
    image2 = malloc(width * height * channels * sizeof(image));
    channels = 1;
    for (int i=0; i<height; i++) {
      for (int j=0; j<width; j++) {
        image2[i*width+j] = (image[i*width*3+j*3] + image[i*width*3+j*3+1] + image[i*width*3+j*3+2])/3;
      }
    }
    free(image);
    image = image2;
    write_JPEG_file("grayscale.jpg", width, height, channels, image, 95);
  }


  int img2d[height][width];
  int img2dhor[height][width];
  int img2dver[height][width];
  int img2dhororg[height][width];
  int img2dverorg[height][width];
  int img2dmag[height][width];


  for (int i=0; i<height; i++){
    for (int j=0; j<width; j++) {
      img2d[i][j] = image[i*width+j];
    }
  }


  int max=-200, min=2000;

  for (int i=1; i<height-1; i++){
    for (int j=1; j<width-1; j++) {
      img2dhor[i][j]=img2d[i-1][j-1]+2*img2d[i-1][j]+img2d[i-1][j+1]-img2d[i+1][j-1]-2*img2d[i+1][j]-img2d[i+1][j+1];
      img2dhororg[i][j] = img2dhor[i][j];
      if (img2dhor[i][j]>max) max = img2dhor[i][j];
      if (img2dhor[i][j]<min) min = img2dhor[i][j];
    }
  }

  for (int i=1; i<height-1; i++){
    for (int j=1; j<width-1; j++) {
      img2dhor[i][j] += abs(min);
    }
  }

  float ratio = (max+abs(min))/255.0;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++){
      img2dhor[i][j] /= ratio;
    }
  }


  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      image[i*width+j]=img2dhor[i][j];
    }
  }
  write_JPEG_file("hor.jpg", width, height, channels, image, 95);

  max=-200; min=2000;

  for (int i=1; i<height-1; i++){
    for (int j=1; j<width-1; j++) {
      img2dver[i][j]=img2d[i-1][j-1]+2*img2d[i][j-1]+img2d[i+1][j-1]-img2d[i-1][j+1]-2*img2d[i][j+1]-img2d[i+1][j+1];
      img2dverorg[i][j] = img2dver[i][j];
      if (img2dver[i][j]>max) max = img2dver[i][j];
      if (img2dver[i][j]<min) min = img2dver[i][j];
    }
  }

  for (int i=1; i<height-1; i++){
    for (int j=1; j<width-1; j++) {
      img2dver[i][j] += abs(min);
    }
  }

  ratio = (max+abs(min))/255.0;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++){
      img2dver[i][j] /= ratio;
    }
  }


  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      image[i*width+j]=img2dver[i][j];
    }
  }

  write_JPEG_file("ver.jpg", width, height, channels, image, 95);

  max=-200; min=2000;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      img2dmag[i][j] = sqrt(pow(img2dhororg[i][j], 2)+pow(img2dverorg[i][j], 2));
      if (img2dmag[i][j]>max) max = img2dmag[i][j];
      if (img2dmag[i][j]<min) min = img2dmag[i][j];
    }
  }

  int diff = max - min;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++){
      float abc = (img2dmag[i][j]-min)/(diff*1.0);
      img2dmag[i][j] = abc* 255;
    }
  }


  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      image[i*width+j]=img2dmag[i][j];
    }
  }

  write_JPEG_file(argv[2], width, height, channels, image, 95);

  max=-200; min=2000;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      if (img2dverorg[i][j] != 0) img2dmag[i][j]=atan2(img2dhororg[i][j], img2dverorg[i][j]) *180.0 /PI;
      else img2dmag[i][j]=90;
      if (img2dmag[i][j]>max) max = img2dmag[i][j];
      if (img2dmag[i][j]<min) min = img2dmag[i][j];
    }
  }

  diff = max - min;

  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++){
      float abc = (img2dmag[i][j]-min)/(diff*1.0);
      img2dmag[i][j] = abc* 255;
    }
  }


  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++) {
      image[i*width+j]=img2dmag[i][j];
    }
  }

  int threshold = 0;
  int histogram[256];

  for (int i=0; i<255; i++) {
    histogram[i]=0;
  }

  for (int i=0; i<width*height; i++) {
    int intensity = image[i];
    histogram[intensity]+=1;
  }


  int cumulate=0, numerator=0, calculating = 1;
  double uh, ul;
  while (calculating){
    for (int j=0; j<=threshold; j++) {
      cumulate += histogram[j];
      numerator += j*histogram[j];
    }
    if (cumulate==0) ul=0;
    else ul = numerator/cumulate;
    numerator=0; cumulate=0;
    for (int j=threshold+1; j<255; j++) {
      cumulate += histogram[j];
      numerator += j*histogram[j];
    }
    if (cumulate==0) uh=0;
    else uh = numerator/cumulate;
    numerator=0; cumulate=0;
    int checkT = (int)((ul+uh)/2.0);
    if (checkT<=threshold) {
      calculating=0;
      threshold = checkT;
    } else {
      threshold += 1;
    }
  }

  for (int i=0; i<width*height; i++) {
    if (image[i]>threshold) image[i]=255;
    else image[i]=0;
  }

  write_JPEG_file("sobel-orientation.jpg", width, height, channels, image, 95);
  free(image);

  return 0;
}
