package bg.uni_sofia.fmi.fmi_ocr_neural_network;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class OtsuTreshouter
{
	public static void exec(String inputFile, String outputPrefix, int granularity_start, int granularity_end)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI);
			final int height = data.length;
			final int width = data[0].length;
			final int min_passes = granularity_start;
			final int max_passes = granularity_end;
			final int plus = 255/(max_passes - min_passes);
			int x,y,i;
			int[][] all= new int[height][width];
			int[][] otsu;
			for(i = min_passes; i < max_passes; i++)
			{
				System.out.println("Otsu binarization splitting the image on " + i + "x" + i +" sectors");
				otsu = OtsuBinarization(data, i);
				for(x = 0; x < width; x++)
					for(y = 0; y< height; y++)
					{
						if(otsu[y][x]> 0)	all[y][x] += plus;
					}
				File outputImageFile = new File(outputPrefix + i + ".png");
				System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
				BufferedImage res = Utils.getGrayscale(otsu);
				ImageIO.write(res, "png", outputImageFile);
			}
			if(max_passes - min_passes > 1)
			{
				System.out.println("Otsu binarization over all binarizations");
				otsu = OtsuBinarization(all, 1);
				File outputImageFile = new File(outputPrefix + "all.png");
				System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
				BufferedImage res = Utils.getGrayscale(otsu);
				ImageIO.write(res, "png", outputImageFile);
			}
			
			
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	private static int[][] OtsuBinarization(int[][] data, int parts)
	{
		final int max_x = data.length;
		final int max_y = data[0].length;
		if(parts <1){ parts = 1; }
		int step_x = max_x/parts;
		int step_y = max_y/parts;
		if(step_x < 1){ step_x = 1; }
		if(step_y < 1){ step_y = 1; }
		final int L = 256;
		double hist[] = new double[L];
		int[][] res = new int[max_x][max_y];

		int x,y, max_step_x, max_step_y, local_x, local_y, N, i, max_k, k;
		double ut, wk, uk, sigma_k, max_sigma_k_;
		
		for(local_x=0; local_x < max_x; local_x+=step_x)
		for(local_y=0; local_y < max_y; local_y+=step_y)
		{
			//initialize grayscale histogram
			for(x=0; x < L; ++x){ hist[x] = 0; }
			
			//set image regions
			max_step_x = local_x + step_x;
			if(max_step_x > max_x) max_step_x = max_x;
			
			max_step_y = local_y + step_y;
			if(max_step_y > max_y) max_step_y = max_y;
			
			//creating grayscale histogram
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
			//calculating intra-class variance for every k
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
				//getting max intra-class variance
				if (sigma_k > max_sigma_k_)
				{
					max_k = k;
					max_sigma_k_ = sigma_k;
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
	
}
