import java.awt.image.BufferedImage;
import java.awt.image.DataBufferUShort;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Pattern;


import javax.imageio.ImageIO;

public class PNGtoMatrix
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
		cwd = cwd + "source_data";
		File actual = new File(cwd);
		for (File f : actual.listFiles())
		{
			if (f.isDirectory())
			{
				System.out.println("Current foder: " + f.getAbsolutePath());
				BufferedOutputStream bufferedOutput;
				try
				{
					bufferedOutput = new BufferedOutputStream(new FileOutputStream(f.getAbsolutePath() + ".txt"));
				
					File[] files= (File[])f.listFiles();
					bufferedOutput.write(("" + files.length+ "\n").getBytes());
					String s = "";
					for (File fonts : files)
					{
						System.out.println("processing " + f.getName() + " - " + fonts.getName());
						try
						{
							BufferedImage BI = ImageIO.read(fonts);
							byte[][] data = convertTo2DWithoutUsingGetRGB(BI);
							for(int i=0; i<data.length; i++)
							{
								for(int j=0; j<data[i].length; j++)
								{
									s += data[i][j];
								}
								s += "\n";
							}
							
						} catch (IOException e)
						{
							e.printStackTrace();
						}
					}
					bufferedOutput.write(s.getBytes());
					bufferedOutput.close();
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
		}
	}
	
	private static byte[][] convertTo2DWithoutUsingGetRGB(BufferedImage image)
	{
		//byte[] pixels = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
		byte[] pixels = (byte[])image.getRaster().getDataElements(0, 0, image.getWidth(), image.getHeight(), null);
		final int width = image.getWidth();
		final int height = image.getHeight();
		byte[][] result = new byte[height][width];
		for (int pixel = 0, row = 0, col = 0; pixel < pixels.length; pixel ++)
		{
			result[row][col] = (byte) (pixels[pixel] == 0 ? 1 : 0);
			
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
