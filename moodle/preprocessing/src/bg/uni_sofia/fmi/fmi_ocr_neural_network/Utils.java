package bg.uni_sofia.fmi.fmi_ocr_neural_network;
import java.awt.Transparency;
import java.awt.color.ColorSpace;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.awt.image.SampleModel;
import java.awt.image.WritableRaster;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import javax.imageio.ImageIO;

public class Utils
{
	public static int[][] convertTo2DGrayscaleArray(BufferedImage image)
	{
		return convertTo2DGrayscaleArray(image, GrayscaleMode.HDTV);
	}
	
	public static int[][] convertTo2DGrayscaleArray(BufferedImage image, int mode)
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
			switch(mode)
			{
				case GrayscaleMode.PAL :
					result[row][col] = (int)Math.round(( 0.299 * red + 0.587 * green +  0.114 * blue));
					break;
				case GrayscaleMode.AVERAGE :
					result[row][col] = (red + green + blue)/3;
					break;
				case GrayscaleMode.HDTV :
				default:
					result[row][col] = (int)Math.round(( 0.2126 * red + 0.7152 * green +  0.0722 * blue));
			}
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
	
	public static int[][] copy_rectangle(int y1, int y2, int x1, int x2, int[][] data)
	{
		
		int i,j,f;
		
		for(i=y1; i<y2; i++)
		{
			f = 0;
			for(j=x1; j<x2; j++)f = f + (data[i][j] == 0 ? 1 : 0);
			if(f == 0){ y1++; }
			else break;
		}
		for(i=y2-1; i>=y1; i--)
		{
			f = 0;
			for(j=x1; j<x2; j++) f = f + (data[i][j] == 0 ? 1 : 0);
			if(f == 0){ y2--; }
			else break;
		}
		
		
		if(y2-y1 < 10) return null;
		if(x2-x1 < 10) return null;
		int[][] res = new int[y2-y1][x2-x1];
		for(i=y1; i<y2; i++)
		{
			for(j=x1; j<x2; j++)
			{
				res[i-y1][j-x1]=data[i][j];
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

	public static class Point
	{
		public int x,y;
		Point(int x, int y)
		{
			this.x = x;
			this.y = y;
		}
		
		public String toString()
		{
			return x + " " + y;
		}
	}
	
	public static class GrayscaleMode
	{
		public static final int AVERAGE	= 1;
		public static final int PAL		= 2;
		public static final int HDTV	= 3;
	}

	public static void datamatrix(String inputFile, String outputFile)
	{
		File actual = new File(inputFile);
		System.out.println("Selected file: " + actual.getAbsolutePath());
		try
		{
			File output = new File(outputFile);
			BufferedOutputStream bufferedOutput = new BufferedOutputStream(new FileOutputStream(outputFile, true));
			System.out.println("Saving output to: " + output.getAbsolutePath());
			String s = "";
			BufferedImage BI = ImageIO.read(actual);
			int[][] data = Utils.convertTo2DGrayscaleArray(BI);
			int i,j,k;
			for(i=0; i<data.length; i++)
			{
				s = "";
				for(j=0; j<data[i].length; j++)
				{
					k = 255-data[i][j];
					if(k < 0){ k = 0; }
					if(k > 255){ k = 255; }
					s +=  String.format("% 4d ", k);
				}
				s += "\n";
				bufferedOutput.write(s.getBytes());
			}
			bufferedOutput.close();		
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	
}

