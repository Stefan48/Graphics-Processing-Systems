package com;

import java.util.Comparator;

public class Region
{
    int xTopLeft, yTopLeft;
    int xBottomRight, yBottomRight;
    int xCenter, yCenter;

    public Region(int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight)
    {
        this.xTopLeft = xTopLeft;
        this.yTopLeft = yTopLeft;
        this.xBottomRight = xBottomRight;
        this.yBottomRight = yBottomRight;
        this.xCenter = (xTopLeft + xBottomRight) / 2;
        this.yCenter = (yTopLeft + yBottomRight) / 2;
    }

    @Override
    public String toString()
    {
        return "[(" + xTopLeft + ", " + yTopLeft + "), (" + xBottomRight + ", " + yBottomRight + ")]";
    }
}

class TemplateRegion extends Region
{
    char symbol;

    public TemplateRegion(Region region, char symbol)
    {
        super(region.xTopLeft, region.yTopLeft, region.xBottomRight, region.yBottomRight);
        this.symbol = symbol;
    }

    @Override
    public String toString()
    {
        return "[" + symbol + " (" + xTopLeft + ", " + yTopLeft + "), (" + xBottomRight + ", " + yBottomRight + ")]";
    }
}

class RegionComparator implements Comparator<Region>
{
    static int tolerance = 50;

    public int compare(Region r1, Region r2)
    {
        if (Math.abs(r1.xCenter - r2.xCenter) < tolerance)
        {
            return Integer.compare(r1.yCenter, r2.yCenter);
        }
        if (r1.xCenter < r2.xCenter)
        {
            return -1;
        }
        return 1;
    }
}
