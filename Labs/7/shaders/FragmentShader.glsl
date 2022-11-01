#version 410

// Input
layout(location = 0) in vec2 texture_coord;

// Uniform properties
uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;
uniform int outputMode = 2; // 0: original, 1: grayscale, 2: blur

// Output
layout(location = 0) out vec4 out_color;

// Local variables
vec2 textureCoord = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y); // Flip texture


vec4 grayscale()
{
    vec4 color = texture(textureImage, textureCoord);
    float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
    return vec4(gray, gray, gray,  0);
}


vec4 blur(int blurRadius)
{
    vec2 texelSize = 1.0f / screenSize;
    vec4 sum = vec4(0);
    for(int i = -blurRadius; i <= blurRadius; i++)
    {
        for(int j = -blurRadius; j <= blurRadius; j++)
        {
            sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
        }
    }
        
    float samples = pow((2 * blurRadius + 1), 2);
    return sum / samples;
}

vec4 median(int medianRadius)
{
    if (medianRadius > 5)
    {
        medianRadius = 5;
    }
    vec2 texelSize = 1.0f / screenSize;
    int samples = int(pow((2 * medianRadius + 1), 2));
    vec4 s[121];
    int cnt = 0;
    // compute samples
    for(int i = -medianRadius; i <= medianRadius; i++)
    {
        for(int j = -medianRadius; j <= medianRadius; j++)
        {
            s[cnt++] = texture(textureImage, textureCoord + vec2(i, j) * texelSize);
        }
    }
    // sort samples (treat each color channel independently, including the alpha component)
    float aux;
    for (int i = 0; i < samples; i++)
    {
        for (int j = i + 1; j < samples; j++)
        {
            if (s[i].x > s[j].x)
            {
                aux = s[i].x;
                s[i].x = s[j].x;
                s[j].x = aux;
            }
            if (s[i].y > s[j].y)
            {
                aux = s[i].y;
                s[i].y = s[j].y;
                s[j].y = aux;
            }
            if (s[i].z > s[j].z)
            {
                aux = s[i].z;
                s[i].z = s[j].z;
                s[j].z = aux;
            }
            if (s[i].a > s[j].a)
            {
                aux = s[i].a;
                s[i].a = s[j].a;
                s[j].a = aux;
            }
        }
    }
    return s[samples / 2];
}


void main()
{
    switch (outputMode)
    {
        case 1:
        {
            out_color = grayscale();
            break;
        }

        case 2:
        {
            out_color = blur(3);
            break;
        }
        case 3:
        {
            out_color = median(1);
            break;
        }

        default:
            out_color = texture(textureImage, textureCoord);
            break;
    }
}
