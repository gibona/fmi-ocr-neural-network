package bg.uni_sofia.fmi.fmi_ocr_neural_network;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.LinkedList;
import java.util.Queue;

import javax.imageio.ImageIO;

public class WordCropper
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
			int[][] used = new int[height][width];
			int i,j,x,y;
			Queue<Utils.Point> q = new LinkedList<Utils.Point>();
			int maxX, maxY,minX, minY, ind=0;
			for(j=0;j<height;j++)
				for(i=0;i<width;i++)
				if(used[j][i] == 0)
				{
					x = i;
					y = j;
					maxX = minX = x;
					maxY = minY = y;
					Utils.Point p = new Utils.Point(x, y);
					q.add(p);
					while(!q.isEmpty())
					{
						p = q.poll();
						if(p.x < minX){ minX = p.x; }
						if(p.y < minY){ minY = p.y; }
						if(p.x > maxX){ maxX = p.x; }
						if(p.y > maxY){ maxY = p.y; }
						used[p.y][p.x] = 1;
						if(p.y < height-1 && data[p.y+1][p.x] == 0 && used[p.y+1][p.x] == 0)
						{
							q.add(new Utils.Point(p.x, p.y+1));
							used[p.y-1][p.x] = 1;
						}
						if( p.y > 0 && data[p.y-1][p.x] == 0 && used[p.y-1][p.x] == 0)
						{
							q.add(new Utils.Point(p.x, p.y-1));
							used[p.y-1][p.x] = 1;
						}
						if(p.x < width-1 && data[p.y][p.x+1] == 0 && used[p.y][p.x+1] == 0)
						{
							q.add(new Utils.Point(p.x+1, p.y));
							used[p.y][p.x+1] = 1;
						}
						if(p.x > 0 && data[p.y][p.x-1] == 0 && used[p.y][p.x-1] == 0)
						{
							q.add(new Utils.Point(p.x-1, p.y));
							used[p.y][p.x-1] = 1;
						}
					}
					
					int[][] new_data = Utils.copy_rectangle(minY, maxY+1, minX, maxX+1, data);
					if(new_data == null) continue;
					File outputImageFile = new File(outputPrefix +minY+"x"+minX+".png");
					System.out.println("Saving output to: " + outputImageFile.getAbsolutePath());
					BufferedImage res = Utils.getGrayscale(new_data);
					Image resized = res.getScaledInstance(29, 29, Image.SCALE_SMOOTH);
					
					BufferedImage resizedBI= new BufferedImage(29,29, BufferedImage.TYPE_BYTE_GRAY);
					resizedBI.getGraphics().drawImage(resized, 0, 0, null);
					ImageIO.write(resizedBI, "png", outputImageFile);
					ind++;
					
				}
			
			
		} catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}

	
