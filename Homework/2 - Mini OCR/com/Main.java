package com;

import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.stream.ImageInputStream;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Scanner;

public class Main {
    public static void main(String[] args)
    {
        String currentDir = System.getProperty("user.dir");
	    File inputFile = new File(currentDir + "/input.png");
	    File templatesFile = new File(currentDir + "/templates.png");
	    File templatesPositionsFile = new File(currentDir + "/templates.txt");
	    File noiselessFile = new File(currentDir + "/noiseless.png");
        File segmentationFile = new File(currentDir + "/segmentation.png");
        File framesFile = new File(currentDir + "/frames.png");

	    try
        {
            // load input image
            ImageInputStream imageInputStream = ImageIO.createImageInputStream(inputFile);
            Iterator<ImageReader> it = ImageIO.getImageReaders(imageInputStream);
            ImageReader imageReader = it.next();
            String imageFormat = imageReader.getFormatName();
            BufferedImage inputImage = ImageIO.read(imageInputStream);
            // apply median filter to reduce noise
            BufferedImage noiselessImage = OCR.ApplyMedianFilter(inputImage, 1);
            ImageIO.write(noiselessImage, imageFormat, noiselessFile);
            // apply segmentation
            ArrayList<Region> regions = OCR.ApplySegmentation(noiselessImage, new OCR.HsvGreenFilter());
            ImageIO.write(noiselessImage, imageFormat, segmentationFile);
            // draw frames
            BufferedImage framesImage = OCR.DrawFrames(noiselessImage, regions);
            ImageIO.write(framesImage, imageFormat, framesFile);
            // read symbols template file
            ArrayList<SymbolTemplate> symbolTemplates = new ArrayList<>();
            Scanner scanner = new Scanner(templatesPositionsFile);
            while (scanner.hasNext())
            {
                symbolTemplates.add(new SymbolTemplate(scanner.next().charAt(0), scanner.nextInt(), scanner.nextInt()));
            }
            // load symbol templates image
            imageInputStream = ImageIO.createImageInputStream(templatesFile);
            it = ImageIO.getImageReaders(imageInputStream);
            imageReader = it.next();
            BufferedImage templatesImage = ImageIO.read(imageInputStream);
            String templatesImageFormat = imageReader.getFormatName();
            // apply segmentation to the templates image
            ArrayList<Region> templatesImageRegions = OCR.ApplySegmentation(templatesImage, new OCR.HsvHighValueFilter());
            ArrayList<TemplateRegion> templateRegions = new ArrayList<>();
            // associate each template region with a symbol template
            for (Region region : templatesImageRegions)
            {
                for (SymbolTemplate symbolTemplate : symbolTemplates)
                {
                    int d = Math.abs(symbolTemplate.xTopLeft - region.xTopLeft) + Math.abs(symbolTemplate.yTopLeft - region.yTopLeft);
                    if (d < 10)
                    {
                        templateRegions.add(new TemplateRegion(region, symbolTemplate.symbol));
                        break;
                    }
                }
            }
            // get text from image
            String text = OCR.GetText(noiselessImage, regions, templatesImage, templateRegions);
            System.out.println(text);
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
