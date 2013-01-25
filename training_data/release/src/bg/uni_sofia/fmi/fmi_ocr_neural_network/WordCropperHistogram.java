package bg.uni_sofia.fmi.fmi_ocr_neural_network;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.Vector;

import javax.imageio.ImageIO;

public class WordCropperHistogram
{
	public static void exec(String inputFile, String outputPrefix)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI);
			final int height = data.length;
			final int width = data[0].length;
			
			double histogram_height[] = new double[height];
			int i,j;
			for(j=0; j<height; j++)
			{
				for(i=0; i<width; i++)
				{
					if(data[j][i] == 0)
					{
						histogram_height[j] += 1;
					}
				}
			}
			Vector<Integer> height_points = new Vector<Integer>();
			boolean zero_mode;
			
			
			if(histogram_height[0] > 0) height_points.add(0);
			zero_mode = histogram_height[0] == 0; 
			for(i=1; i<height-1; i++)
			{
				if(histogram_height[i] == 0)
				{
					if(!zero_mode)
					{
						height_points.add(i);
					}
					zero_mode = true;
				}
				else
				{
					if(zero_mode)
					{
						height_points.add(i);
					}
					zero_mode = false;
				}
			}
			if(histogram_height[height-1] > 0) height_points.add(height-1);
			
			int ind = 0,h;
			int height_start, height_end;
			for(h=0; h+1<height_points.size(); h+=2)
			{
				Vector<Integer> width_points = new Vector<Integer>();
				
				double histogram_width[] = new double[width];
				
				height_start = height_points.elementAt(h);
				height_end	 = height_points.elementAt(h+1);
				for(j=height_start; j<height_end; j++)
				{
					for(i=0; i<width; i++)
					{
						if(data[j][i] == 0)
						{
							histogram_width[i] += 1;
						}
					}
				}
				
				if(histogram_width[0] > 0) width_points.add(0);
				zero_mode = histogram_width[0] == 0; 
				for(i=1; i<width-1; i++)
				{
					if(histogram_width[i] == 0)
					{
						if(!zero_mode)
						{
							width_points.add(i);
						}
						zero_mode = true;
					}
					else
					{
						if(zero_mode)
						{
							width_points.add(i);
						}
						zero_mode = false;
					}
				}
				if(histogram_width[width-1] > 0) width_points.add(width-1);
		
				for(i=0; i+1<width_points.size(); i+=2)
				{
					int[][] new_data = Utils.copy_rectangle(height_start, height_end, width_points.elementAt(i), width_points.elementAt(i+1), data);
					if(new_data == null) continue;
					File outputImageFile = new File(outputPrefix+ind+".png");
					System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
					BufferedImage res = Utils.getGrayscale(new_data);
					Image resized = res.getScaledInstance(29, 29, Image.SCALE_SMOOTH);
					
					BufferedImage resizedBI= new BufferedImage(29,29, BufferedImage.TYPE_BYTE_GRAY);
					resizedBI.getGraphics().drawImage(resized, 0, 0, null);
					ImageIO.write(resizedBI, "png", outputImageFile);
					ind++;
				}
			}
			
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
}
