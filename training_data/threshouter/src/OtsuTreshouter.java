import java.awt.Transparency;
import java.awt.color.ColorSpace;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBufferUShort;
import java.awt.image.Raster;
import java.awt.image.SampleModel;
import java.awt.image.WritableRaster;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;

public class OtsuTreshouter
{
	public static void main(String[] args)
	{
		System.out.println("Working Directory = " + System.getProperty("user.dir"));
		String cwd = System.getProperty("user.dir");
		File actual = new File(cwd + File.separator + "image.jpg");
	
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = convertTo2DWithoutUsingGetRGB(BI);
			final int height = data.length;
			final int width = data[0].length;
//			final int min_passes = 13;
//			final int max_passes = 20;
			final int min_passes = 1;
			final int max_passes = 30;
			final int plus = 255/(max_passes - min_passes);
			int x,y,i;
			int[][] all= new int[height][width];
			System.out.println("doing Grayscale");
			File grayscaleImageFile = new File(cwd + File.separator + "grayscale.png");
			BufferedImage grayscale = getGrayscale(data);
			ImageIO.write(grayscale, "png", grayscaleImageFile);
			for(i=0; i<100; i++)
			{
				System.out.println("doing blur/sharpen");
				data = blur(data);
//				data = sharpen(data);
			}
			System.out.println("saving blur/sharpen");
			File denosedImageFile = new File(cwd + File.separator + "denosed.png");
			BufferedImage denosedImage = getGrayscale(data);
			ImageIO.write(denosedImage, "png", denosedImageFile);
			System.out.println("doing Otsu");
			int[][] otsu;
			for(i = min_passes; i < max_passes; i++)
			{
				System.out.println("Otsu pass " + i);
				otsu = OtsuBinarization(data, i);
				for(x = 0; x < width; x++)
					for(y = 0; y< height; y++)
					{
						if(otsu[y][x]> 0)	all[y][x] += plus;
					}
				File outputImageFile = new File(cwd + File.separator + "res" + i + ".png");
				BufferedImage res = getGrayscale(otsu);
				ImageIO.write(res, "png", outputImageFile);
			}
			System.out.println("Otsu pass all");
			otsu = OtsuBinarization(all, 1);
			
			File outputImageFile = new File(cwd + File.separator + "res_all.png");
			BufferedImage res = getGrayscale(otsu);
			ImageIO.write(res, "png", outputImageFile);
			
			
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	public static int[][] blur(int[][] data)
	{
		final int matrix[][]= {
				{1,2,1},
				{2,4,2},
				{1,2,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{1,1,1,1,1,1,1},
//				{4,4,4,4,4},
//				{4,2,2,2,4},
//				{4,2,1,2,4},
//				{4,2,2,2,4},
//				{4,4,4,4,4},
	/*			{0,0,1,0,0},
				{0,1,2,1,0},
				{1,2,4,2,1},
				{0,1,2,1,0},
				{0,0,1,0,0},*/
			};
		return apply_matrix(data, matrix);
	}
	
	public static int[][] sharpen(int[][] data)
	{
		final int matrix[][]= {
//				{0, -1, 0},
//				{-1,  5, -1},
//				{0, -1, 0},
				{-1, -1, -1},
				{-1,  9, -1},
				{-1, -1, -1},
			};
//		final int matrix[][]= {
//				{-1, -1, -1, -1, -1},
//				{-1,  2,  2,  2, -1},
//				{-1,  2,  8,  2, -1},
//				{-1,  2,  2,  2, -1},
//				{-1, -1, -1, -1, -1},
//		};
		return apply_matrix(data, matrix);
	}
	
	public static int[][] apply_matrix(int[][] data, int[][] matrix)
	{
		final int height = data.length;
		final int width = data[0].length;

		final int matrix_offset_width = matrix[0].length/2;
		final int matrix_offset_height= matrix.length/2;
		final int matrix_width = matrix[0].length;
		final int matrix_height= matrix.length;
		
		int matrix_weight =0;
		int x,y,i,j,dx,dy;
		
		int[][] res = new int[height][width];
		for(y=0; y < height; ++y)
			for(x=0; x < width; ++x)
			{
				res[y][x] = 0;
				matrix_weight =0;
				for(j=0; j<matrix_height; j++)
					for(i=0; i<matrix_width; i++)
					{
						dy = y + j - matrix_offset_height;
						dx = x + i - matrix_offset_width;
						if(	dx < 0
						||	dx >= width
						||	dy < 0
						||	dy >= height) continue;
						matrix_weight += matrix[j][i];
						res[y][x] = res[y][x] + (matrix[j][i] * data[y + j - matrix_offset_height][x + i - matrix_offset_width]);
					}
				if(matrix_weight != 0) { res[y][x] = res[y][x] / matrix_weight; }
				if(res[y][x] < 0){ res[y][x] = 0; }
				if(res[y][x] > 255){ res[y][x] = 255; }
			}
		return res;
	}
	public static int[][] OtsuBinarization(int[][] data, int parts)
	{
		final int max_x = data.length;
		final int max_y = data[0].length;
//		final int step_x = 10;
//		final int step_y = 10;
		final int step_x = max_x/parts;
		final int step_y = max_y/parts;
		final int L = 256;
		double hist[] = new double[L];
		int[][] res = new int[max_x][max_y];

		int x,y, max_step_x, max_step_y, local_x, local_y, N, i, max_k, max_sigma_k_, k;
		double ut, wk, uk, sigma_k;
		
		for(local_x=0; local_x < max_x; local_x+=step_x)
		for(local_y=0; local_y < max_y; local_y+=step_y)
		{
			//calculate grayscale histogram
			for(x=0; x < L; ++x){ hist[x] = 0; }
			
			max_step_x = local_x + step_x;
			if(max_step_x > max_x) max_step_x = max_x;
			
			max_step_y = local_y + step_y;
			if(max_step_y > max_y) max_step_y = max_y;
			
			
			for(x=local_x; x < max_step_x; ++x)
			for(y=local_y; y < max_step_y; ++y)
			{
				hist[data[x][y]]+=1;
			}
			
			N = (max_step_x - local_x)*(max_step_y - local_y);
			
			//normalize histogram
			for (i=0; i<L; i++) hist[i]/=N;
			
			
			ut = 0;
			for (i=0; i<L; i++) ut+=i*hist[i];
			
			max_k=0;
			max_sigma_k_=0;
			for (k=0; k < L;++k)
			{
				wk = 0;
				for (i = 0; i <=k; ++i){ wk += hist[i];}
				uk = 0;
				for (i = 0; i <=k; ++i){ uk+= i*hist[i]; }
			
				sigma_k = 0;
				if (wk !=0 && wk!=1)
				{
					sigma_k  = ((ut*wk - uk)*(ut*wk - uk))/(wk*(1-wk));
				}
			
				if (sigma_k > max_sigma_k_)
				{
					max_k = k;
					max_sigma_k_ = (int) sigma_k;
				}
			}
			
			for(x=local_x; x < max_step_x; ++x)
			for(y=local_y; y < max_step_y; ++y)
			{
				if ((data[x][y]) < max_k)
				{
					res[x][y] = 0x00;
				}
				else
				{
					res[x][y] = 0xFF;
				}
			}
		}
		return res;
	}
	
	public static BufferedImage getGrayscale(int[][] bytes)
	{
		final int width = bytes[0].length;
		final int height = bytes.length;
		byte[] pixels = new byte[width * height];
		int i,j;
		for (j = 0; j < height; j++)
			for(i = 0; i < width; i++)
			{
				pixels[j * width + i] = (byte)bytes[j][i];
			}
		ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_GRAY);
		int[] nBits = { 8 };
		ColorModel cm = new ComponentColorModel(cs, nBits, false, true, Transparency.OPAQUE, DataBuffer.TYPE_BYTE);
		SampleModel sm = cm.createCompatibleSampleModel(width, height);
		DataBufferByte db = new DataBufferByte(pixels, width * height);
		WritableRaster raster = Raster.createWritableRaster(sm, db, null);
		BufferedImage result = new BufferedImage(cm, raster, false, null);
		
		return result;
	}

	private static int[][] convertTo2DWithoutUsingGetRGB(BufferedImage image)
	{
		//byte[] pixels = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
		final int width = image.getWidth();
		final int height = image.getHeight();
		int[] pixels = new int[width*height];
		image.getRGB(0, 0, width, height, pixels, 0, width);

		int[][] result = new int[height][width];
		int red, green, blue, pixel, row, col;
		for (pixel = 0, row = 0, col = 0; pixel < pixels.length; pixel ++)
		{
			red 	= (pixels[pixel] & 0xFF);
			green 	= (pixels[pixel] & 0xFF00)>>8;
			blue 	= (pixels[pixel] & 0xFF0000)>>16;
//			result[row][col] = green;
//			result[row][col] = (red + green + blue)/3;
//			result[row][col] = Math.round(( 0.299 * red + 0.587 * green +  0.114 * blue));
			result[row][col] = (int)Math.round(( 0.2126 * red + 0.7152 * green +  0.0722 * blue));
			col++;
			if (col == width)
			{
				col = 0;
				row++;
				if(row >= height) 
				{
					return result;
				}
			}		
		}
		return result;
	}

}
