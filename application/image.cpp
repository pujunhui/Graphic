#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(uint32_t width, uint32_t height, RGBA* data) {
    mWidth = width;
    mHeight = height;
    if (data) {
        mData = new RGBA[mWidth * mHeight];
        memcpy(mData, data, sizeof(RGBA) * mWidth * mHeight);
    }
}

Image::~Image(){
    if(mData != nullptr) {
        delete[] mData;
    }
}

Image* Image::createImage(const std::string& path) {
    int picType = 0;
    int width{ 0 }, height{ 0 };

    //stb_image读取的图片，原点在左上角，y轴是向下生长的
    //而我们绘制的坐标系，以左下角为原点，故需要翻转y轴
    stbi_set_flip_vertically_on_load(true);

    unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);
    //由于图片是RGBA的格式，而我们采用BRGA的格式显示，所以需要交换R&B
    for (int i = 0; i < width * height * 4; i += 4)
    {
        byte tmp = bits[i];
        bits[i] = bits[i + 2];
        bits[i + 2] = tmp;
    }

    Image* image = new Image(width, height, (RGBA*)bits);

    stbi_image_free(bits);

    return image;
}

void Image::destroyImage(Image* image) {
    if (image) {
        delete image;
    }
}