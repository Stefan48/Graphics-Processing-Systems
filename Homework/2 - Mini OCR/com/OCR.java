package com;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.util.*;

public class OCR
{
    static BufferedImage ImageDeepCopy(BufferedImage image) {
        ColorModel colorModel = image.getColorModel();
        boolean isAlphaPremultiplied = colorModel.isAlphaPremultiplied();
        WritableRaster raster = image.copyData(null);
        return new BufferedImage(colorModel, raster, isAlphaPremultiplied, null);
    }

    public static BufferedImage ApplyMedianFilter(BufferedImage image, int radius)
    {
        BufferedImage newImage = OCR.ImageDeepCopy(image);
        int height = image.getHeight();
        int width = image.getWidth();
        int verticalLimit = height - radius;
        int horizontalLimit = width - radius;
        int d = 2 * radius + 1;
        int numPixels = d * d;
        int[] reds = new int[numPixels];
        int[] greens = new int[numPixels];
        int[] blues = new int[numPixels];

        for (int i = radius; i < verticalLimit; ++i)
        {
            for (int j = radius; j < horizontalLimit; ++j)
            {
                int pixelCount = 0;
                for (int k = -radius; k <= radius; ++k)
                {
                    for (int l = -radius; l <= radius; ++l)
                    {
                        Color color = new Color(image.getRGB(j+l, i+k));
                        reds[pixelCount] = color.getRed();
                        greens[pixelCount] = color.getGreen();
                        blues[pixelCount] = color.getBlue();
                        pixelCount++;
                    }
                }
                Arrays.sort(reds);
                Arrays.sort(greens);
                Arrays.sort(blues);
                newImage.setRGB(j, i, new Color(reds[numPixels/2], greens[numPixels/2], blues[numPixels/2]).getRGB());
            }
        }
        return newImage;
    }

    static class HSV
    {
        double H, S, V;

        HSV(double H, double S, double V)
        {
            this.H = H;
            this.S = S;
            this.V = V;
        }

        @Override
        public String toString()
        {
            return "(" + H + ", " + S + ", " + V + ")";
        }
    }

    public static HSV RgbToHsv(Color rgb)
    {
        double r = (double)(rgb.getRed()) / 255.0;
        double g = (double)(rgb.getGreen()) / 255.0;
        double b = (double)(rgb.getBlue()) / 255.0;
        double max = Math.max(r, Math.max(g, b));
        double min = Math.min(r, Math.min(g, b));
        double diff = max - min;
        double H, S, V;
        // compute H
        if (max == min)
        {
            H = 0;
        }
        else if (max == r)
        {
            H = (60 * ((g - b) / diff) + 360) % 360;
        }
        else if (max == g)
        {
            H = (60 * ((b - r) / diff) + 120) % 360;
        }
        else
        {
            H = (60 * ((r - g) / diff) + 240) % 360;
        }
        // compute S
        if (max == 0)
        {
            S = 0;
        }
        else
        {
            S = (diff / max) * 100;
        }
        // compute V
        V = max * 100;
        return new HSV(H, S, V);
    }

    private static boolean isGreen(double hue)
    {
        return (hue >= 60.0 && hue <= 170.0);
    }

    interface HsvSimilarityFilter
    {
        boolean PixelIsValid(HSV hsv);
    }

    static class HsvGreenFilter implements HsvSimilarityFilter
    {
        @Override
        public boolean PixelIsValid(HSV hsv)
        {
            return isGreen(hsv.H);
        }
    }

    static class HsvHighValueFilter implements HsvSimilarityFilter
    {
        @Override
        public boolean PixelIsValid(HSV hsv)
        {
            return (hsv.V > 30.0);
        }
    }

    private static Region BFS(BufferedImage image, HSV[][] imageHSV, int height, int width, int xStart, int yStart, boolean[][] visited, HsvSimilarityFilter filter)
    {
        class Position
        {
            int x;
            int y;

            Position(int x, int y)
            {
                this.x = x;
                this.y = y;
            }
        }

        int xTopLeft = Integer.MAX_VALUE, yTopLeft = Integer.MAX_VALUE;
        int xBottomRight = Integer.MIN_VALUE, yBottomRight = Integer.MIN_VALUE;
        LinkedList<Position> queue = new LinkedList<>();
        visited[xStart][yStart] = true;
        queue.add(new Position(xStart, yStart));
        while (!queue.isEmpty())
        {
            Position pixel = queue.removeFirst();
            int x = pixel.x;
            int y = pixel.y;
            if (x < xTopLeft)
            {
                xTopLeft = x;
            }
            if (x > xBottomRight)
            {
                xBottomRight = x;
            }
            if (y < yTopLeft)
            {
                yTopLeft = y;
            }
            if (y > yBottomRight)
            {
                yBottomRight = y;
            }
            image.setRGB(y, x, new Color(255, 255, 255).getRGB());
            // explore north
            if (x-1 >= 0 && !visited[x-1][y] && filter.PixelIsValid(imageHSV[x-1][y]))
            {
                visited[x-1][y] = true;
                queue.add(new Position(x-1, y));
            }
            // explore south
            if (x+1 < height && !visited[x+1][y] && filter.PixelIsValid(imageHSV[x+1][y]))
            {
                visited[x+1][y] = true;
                queue.add(new Position(x+1, y));
            }
            // explore west
            if (y-1 >= 0  && !visited[x][y-1] && filter.PixelIsValid(imageHSV[x][y-1]))
            {
                visited[x][y-1] = true;
                queue.add(new Position(x, y-1));
            }
            // explore east
            if (y+1 < width  && !visited[x][y+1] && filter.PixelIsValid(imageHSV[x][y+1]))
            {
                visited[x][y+1] = true;
                queue.add(new Position(x, y+1));
            }
        }
        return new Region(xTopLeft, yTopLeft, xBottomRight, yBottomRight);
    }

    public static ArrayList<Region> ApplySegmentation(BufferedImage image, HsvSimilarityFilter filter)
    {
        int height = image.getHeight();
        int width = image.getWidth();
        // RGB to HSV conversion
        HSV[][] imageHsv = new HSV[height][width];
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                imageHsv[i][j] = RgbToHsv(new Color(image.getRGB(j, i)));
            }
        }
        // perform BFS to determine regions
        ArrayList<Region> regions = new ArrayList<>();
        boolean[][] visited = new boolean[height][width];
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                if (filter.PixelIsValid(imageHsv[i][j]))
                {
                    if (!visited[i][j])
                    {
                        regions.add(BFS(image, imageHsv, height, width, i, j, visited, filter));
                    }
                }
                else
                {
                    image.setRGB(j, i, new Color(0, 0, 0).getRGB());
                }
            }
        }
        // sort regions by their center
        regions.sort(new RegionComparator());
        return regions;
    }

    public static BufferedImage DrawFrames(BufferedImage image, ArrayList<Region> regions)
    {
        int imageHeight = image.getHeight();
        int imageWidth = image.getWidth();
        BufferedImage newImage = OCR.ImageDeepCopy(image);
        for (Region region : regions)
        {
            int x1 = Math.max(0, region.xTopLeft - 1);
            int y1 = Math.max(0, region.yTopLeft - 1);
            int x2 = Math.min(imageHeight, region.xBottomRight + 1);
            int y2 = Math.min(imageWidth, region.yBottomRight + 1);
            for (int j = x1; j <= x2; ++j)
            {
                newImage.setRGB(y1, j, new Color(255, 0, 0).getRGB());
                newImage.setRGB(y2, j, new Color(255, 0, 0).getRGB());
            }
            for (int j = y1 + 1; j < y2; ++j)
            {
                newImage.setRGB(j, x1, new Color(255, 0, 0).getRGB());
                newImage.setRGB(j, x2, new Color(255, 0, 0).getRGB());
            }
        }
        return newImage;
    }

    private static int spaceDistance = 30;

    public static String GetText(BufferedImage image, ArrayList<Region> regions, BufferedImage templatesImage, ArrayList<TemplateRegion> templateRegions)
    {
        // match each frame with a symbol template
        StringBuilder outputText = new StringBuilder();
        for (int r = 0; r < regions.size(); ++r)
        {
            Region region = regions.get(r);
            int x1 = region.xTopLeft;
            int y1 = region.yTopLeft;
            int minDifferentPixels = Integer.MAX_VALUE;
            TemplateRegion matchingTemplate = null;
            for (TemplateRegion template : templateRegions)
            {
                int height = Math.max(region.xBottomRight - x1, template.xBottomRight - template.xTopLeft) + 1;
                int width = Math.max(region.yBottomRight - y1, template.yBottomRight - template.yTopLeft) + 1;
                int x2 = x1 + height - 1;
                int y2 = y1 + width - 1;
                int offsetX = template.xTopLeft - x1;
                int offsetY = template.yTopLeft - y1;
                int differentPixels = 0;
                for (int i = x1; i <= x2; ++i)
                {
                    if (i > region.xBottomRight || i + offsetX > template.xBottomRight)
                    {
                        differentPixels += (x2 - i + 1) * (y2 - y1 + 1);
                        break;
                    }
                    for (int j = y1; j <= y2; ++j)
                    {
                        if (j > region.yBottomRight || j + offsetY > template.yBottomRight)
                        {
                            differentPixels += (y2 - j + 1);
                            break;
                        }
                        if (image.getRGB(j, i) != templatesImage.getRGB(j + offsetY, i + offsetX))
                        {
                            differentPixels++;
                        }
                    }
                }
                if (differentPixels < minDifferentPixels)
                {
                    minDifferentPixels = differentPixels;
                    matchingTemplate = template;
                }
            }
            // the distance between two consecutive frames determines whether there is a space/newline or not
            if (r > 0)
            {
                if (Math.abs(region.xCenter - regions.get(r - 1).xCenter) < RegionComparator.tolerance)
                {
                    if (region.yTopLeft - regions.get(r - 1).yBottomRight >= spaceDistance)
                    {
                        outputText.append(" ");
                    }
                }
                else
                {
                    outputText.append("\n");
                }
            }
            if (matchingTemplate != null)
            {
                outputText.append(matchingTemplate.symbol);
            }
        }
        return outputText.toString();
    }
}
