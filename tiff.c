#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <math.h>

extern void calc_pixel_value(int nx, int ny, int coordArray[nx][ny], int maxiter)
{
	int pointiter = 0;
	int i = 0;
	int m_image_data[nx*ny];
	int elecount = 0;
	int a = 0;
	//double grey = (0.3*r) + (0.59*g) + (0.11*b);
	char outputname[10] = "output.tif";
	for (i = 1; i<=ny; i++)
	{
		for (a = 1; a<=nx; a++)
		{
			pointiter = coordArray[a][i];
			if (pointiter == maxiter)
				m_image_data[elecount] = 0;
			else 
				m_image_data[elecount] = 1;
			elecount++;	
	//		printf("%d\n", pointiter);	
		}
	}
	for(i =0;i<=(nx*ny); i++){
		printf("%d\n", m_image_data[i]);}
	write_tiff_bilevel(outputname, 500,500, m_image_data);
	
}

void write_tiff_bilevel(char * outputFilename, int m_width, int m_height, int m_image_data[m_width*m_height])
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
	TIFFSetField(outputname, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(outputname, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(outputname, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(outputname, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(outputname, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	
	

	//write info to file
	TIFFWriteEncodedStrip(outputname, 0, m_image_data, m_width*m_height);

	TIFFClose(outputname);

}
