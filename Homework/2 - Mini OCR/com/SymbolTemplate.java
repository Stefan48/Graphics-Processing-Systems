package com;

public class SymbolTemplate
{
    char symbol;
    int xTopLeft;
    int yTopLeft;

    public SymbolTemplate(char symbol, int yTopLeft, int xTopLeft)
    {
        this.symbol = symbol;
        this.xTopLeft = xTopLeft;
        this.yTopLeft = yTopLeft;
    }
}
