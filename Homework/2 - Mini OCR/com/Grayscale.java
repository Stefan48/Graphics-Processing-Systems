package com;

import java.awt.*;
import java.awt.image.BufferedImage;

public class Grayscale
{
    public static void ConvertToGrayscale(BufferedImage image)
    {
        int height = image.getHeight();
        int width = image.getWidth();
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                Color color = new Color(image.getRGB(j, i));
                int red = (int)(color.getRed() * 0.2126);
                int green = (int)(color.getGreen() * 0.7152);
                int blue = (int)(color.getBlue() * 0.0722);
                int gray = red + green + blue;
                image.setRGB(j, i, new Color(gray, gray, gray).getRGB());
            }
        }
    }
}
