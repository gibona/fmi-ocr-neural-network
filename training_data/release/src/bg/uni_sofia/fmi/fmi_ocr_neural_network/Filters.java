package bg.uni_sofia.fmi.fmi_ocr_neural_network;

import java.awt.image.BufferedImage;
import java.io.File;

import javax.imageio.ImageIO;

public class Filters
{
	public static void blur(String inputFile, String outputFile)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI);
			data = Filters.blur_filter(data);
			File outputImageFile = new File(outputFile);
			System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
			BufferedImage res = Utils.getGrayscale(data);
			ImageIO.write(res, "png", outputImageFile);
		} catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public static void grayscale(String inputFile, String outputFile, int mode)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI, mode);
			File outputImageFile = new File(outputFile);
			System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
			BufferedImage res = Utils.getGrayscale(data);
			ImageIO.write(res, "png", outputImageFile);
		} catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public static void sharpen(String inputFile, String outputFile)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI);
			data = Filters.sharpen_filter(data);
			File outputImageFile = new File(outputFile);
			System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
			BufferedImage res = Utils.getGrayscale(data);
			ImageIO.write(res, "png", outputImageFile);
		} catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	private static int[][] blur_filter(int[][] data)
	{
		final int matrix[][]= {
				{1,2,1},
				{2,4,2},
				{1,2,1},
		};
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
		return apply_matrix(data, matrix);
	}
	
	public static int[][] sharpen_filter(int[][] data)
	{
		final int matrix[][]= {
				{-1, -1, -1},
				{-1,  9, -1},
				{-1, -1, -1},
			};
//				{0, -1, 0},
//				{-1,  5, -1},
//				{0, -1, 0},
//				{-1, -1, -1, -1, -1},
//				{-1,  2,  2,  2, -1},
//				{-1,  2,  8,  2, -1},
//				{-1,  2,  2,  2, -1},
//				{-1, -1, -1, -1, -1},
		return apply_matrix(data, matrix);
	}
	
	private static int[][] apply_matrix(int[][] data, int[][] matrix)
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
}
