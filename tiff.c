#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <math.h>

void write_tiff_bilevel(char * outputFilename, int m_width, int m_height, unsigned char m_image_data[m_width*m_height]);

extern void calc_pixel_value(int nx, int ny, int coordArray[nx][ny], int maxiter)
{
	int pointiter = 0;
	unsigned char *m_image_data = malloc(sizeof(unsigned char)*nx*ny);
	int i = 0;
	int elecount = 0;
	int a = 0;
	char outputname[10] = "output.tif";
	for (i = 0; i<ny; i++)
	{
		for (a = 0; a<nx; a++)
		{
			pointiter = coordArray[a][i];
			if (pointiter == 1)			//COLOR HERE
				m_image_data[elecount] = 0;
			else 
				m_image_data[elecount] = 255;
			elecount++;	
		}
	}
//	for(i =0;i<=(nx*ny); i++){
//		printf("%d\n", m_image_data[i]);}
	write_tiff_bilevel(outputname, nx, ny, m_image_data);
	
}

void write_tiff_bilevel(char * outputFilename, int m_width, int m_height, unsigned char m_image_data[m_width*m_height])
{
	int i;
	TIFF *outputname;
	if((outputname = TIFFOpen(outputFilename, "w")) == NULL)
	{
		printf("Unable to write to TIFF file");
		exit(42);
	}
	TIFFSetField(outputname, TIFFTAG_IMAGEWIDTH, m_width);
	TIFFSetField(outputname, TIFFTAG_IMAGELENGTH, m_height);
	TIFFSetField(outputname, TIFFTAG_ROWSPERSTRIP, m_height);
	TIFFSetField(outputname, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(outputname, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(outputname, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(outputname, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	TIFFSetField(outputname, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	//TIFFSetField(outputname, TIFFTAG_XRESOLUTION, 500.0);
	//TIFFSetField(outputname, TIFFTAG_YRESOLUTION, 500.0);
	//TIFFSetField(outputname, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);	
	//TIFFSetField(outputname, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);

	//write info to file
	TIFFWriteEncodedStrip(outputname, 0, m_image_data, m_width*m_height);

	TIFFClose(outputname);

}
