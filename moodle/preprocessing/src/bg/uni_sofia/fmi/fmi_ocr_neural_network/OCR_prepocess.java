package bg.uni_sofia.fmi.fmi_ocr_neural_network;

public class OCR_prepocess
{
	
	public static void main(String[] args)
	{
//		String[] args = {"-datamatrix", "courier-new.png", "a.txt"};
		if(args.length == 0)
		{
			System.out.println("Not enough parameters");
			return;
		}
		
		if(args[0].equals("-wordcropper"))
		{
			System.out.println("Wordcropper selected");
			if(args.length == 1)
			{
				System.out.println("Not enough parameters");
				return;
			}
			if(args[1].equals("-histogram"))
			{
				System.out.println("Mode: histogram");
				String inputFile = "image.png";
				String outputPrefix = "res_";
				if(args.length >=3)
				{
					inputFile = args[2];
				}
				if(args.length >=4)
				{
					outputPrefix = args[3];
				}
				WordCropperHistogram.exec(inputFile, outputPrefix);
			}
			else if (args[1].equals("-bfs"))
			{
				System.out.println("Mode: bfs");
				String inputFile = "image.png";
				String outputPrefix = "res_";
				if(args.length >=3)
				{
					inputFile = args[2];
				}
				if(args.length >=4)
				{
					outputPrefix = args[3];
				}
				WordCropper.exec(inputFile, outputPrefix);
			}
			else
			{
				System.out.println("No mode selected");
			}
		}
		else if(args[0].equals("-filter"))
		{
			System.out.println("Filter selected");
			if(args.length == 1)
			{
				System.out.println("Not enough parameters");
				return;
			}
			if(args[1].equals("-blur"))
			{
				System.out.println("Mode: blur");
				String inputFile = "image.png";
				String outputFile = "outputimage.png";
				if(args.length >=3)
				{
					inputFile = args[2];
				}
				if(args.length >=4)
				{
					outputFile = args[3];
				}
				Filters.blur(inputFile, outputFile);
			}
			else if(args[1].equals("-sharpen"))
			{
				System.out.println("Mode: sharpen");
				String inputFile = "image.png";
				String outputFile = "outputimage.png";
				if(args.length >=3)
				{
					inputFile = args[2];
				}
				if(args.length >=4)
				{
					outputFile = args[3];
				}
				Filters.sharpen(inputFile, outputFile);
			}
			else if(args[1].equals("-grayscale"))
			{
				System.out.println("Mode: grayscale");
				String inputFile = "image.png";
				String outputFile = "outputimage.png";
				int mode = 0;
				if(args.length >=3)
				{
					inputFile = args[2];
				}
				if(args.length >=4)
				{
					outputFile = args[3];
				}
				if(args.length >=5)
				{
					if(args[4].equals("-type-average"))	{ mode = Utils.GrayscaleMode.AVERAGE; }
					if(args[4].equals("-type-hdtv"))	{ mode = Utils.GrayscaleMode.HDTV; }
					if(args[4].equals("-type-pal"))		{ mode = Utils.GrayscaleMode.PAL; }
				}
				Filters.grayscale(inputFile, outputFile, mode);
			}
			else
			{
				System.out.println("No mode selected");
			}
		}
		else if(args[0].equals("-binarize"))
		{
			System.out.println("Otsu binarization selected");
			String inputFile = "";
			String outputPrefix = "";
			int granularity_start = -1, granularity_end = -1; 
			for(int i=1; i < args.length; i++)
			{
				if(args[i].charAt(0) != '-')
				{
					if(inputFile.equals("")){ inputFile = args[i]; continue; }
					if(outputPrefix.equals("")){ outputPrefix = args[i]; continue; }
				}
				else
				{
					if(args[i].equals("-start") && i+1<args.length)
					{
						granularity_start = new Integer(args[i+1]);
						i++;
						continue; 
					}
					if(args[i].equals("-end") && i+1<args.length)
					{
						granularity_end = new Integer(args[i+1]);
						i++;
						continue;
					}
				}
				System.out.println("Unknown parameter: " + args[i]);
			}
			if(inputFile.equals("")){ inputFile = "image.png"; }
			if(outputPrefix.equals("")){ outputPrefix = "out_"; }
			if(granularity_start < 1){ granularity_start = 1; }
			if(granularity_end <= granularity_start){ granularity_end = granularity_start + 1; }
			OtsuTreshouter.exec(inputFile, outputPrefix, granularity_start, granularity_end);
		}
		else if(args[0].equals("-datamatrix"))
		{
			System.out.println("Converting to datamatrix selected");
			String inputFile = "image.png";
			String outputFile = "matrix.txt";
			if(args.length >=2)
			{
				inputFile = args[1];
			}
			if(args.length >=3)
			{
				outputFile = args[2];
			}
			Utils.datamatrix(inputFile, outputFile);
		}
		else
		{
			System.out.println("Nothing selected");
		}
			
	}
	
}
