import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferUShort;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Pattern;


import javax.imageio.ImageIO;

public class PNGtoMatrixReal
{
	public static void main(String[] args)
	{
		System.out.println("Working Directory = " + System.getProperty("user.dir"));
		String cwd = System.getProperty("user.dir");
		String cwds[] = cwd.split(Pattern.quote(File.separator));
		cwd = "";
		for (int i = 0; i < cwds.length - 1; i++)
		{
			cwd = cwd + cwds[i] + File.separator;
		}
		cwd = cwd + "real_test_data";
		File actual = new File(cwd);
		try
		{
			
			String s = "", letter;
			System.out.println("Scanning folders");

			for (File f : actual.listFiles())
			{
				if(f.isDirectory()) continue;
				BufferedOutputStream bufferedOutput;
				bufferedOutput = new BufferedOutputStream(new FileOutputStream(actual.getAbsolutePath() + "\\data.txt", true));
				System.out.println("processing " + f.getName() + " - " + f.getName());
				try
				{
					s = "";
					BufferedImage BI = ImageIO.read(f);
					if(BI == null) continue;
					int[][] data = convertTo2DWithoutUsingGetRGB(BI);
					for(int i=0; i<data.length; i++)
					{
						for(int j=0; j<data[i].length; j++)
						{
							s +=  String.format("% 4d ", 255 - data[i][j]);
						}
						s += "\n";
					}
					letter = f.getName();
					s += ""+ letter.charAt(0) + "\n";
					
					bufferedOutput.write(s.getBytes());
				} catch (IOException e)
				{
					e.printStackTrace();
				}
				bufferedOutput.close();
			}
		} catch (FileNotFoundException e1)
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private static int[][] convertTo2DWithoutUsingGetRGB(BufferedImage image)
	{
		//byte[] pixels = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
		final int width = image.getWidth();
		final int height = image.getHeight();
		int[] pixels = new int[width*height];
		image.getRGB(0, 0, width, height, pixels, 0, width);

		int[][] result = new int[height][width];
		for (int pixel = 0, row = 0, col = 0; pixel < pixels.length; pixel ++)
		{
			result[row][col] = (pixels[pixel] & 0xFF);
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
