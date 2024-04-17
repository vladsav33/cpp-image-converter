#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

#include <iostream>

using namespace std;

namespace img_lib {

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

PACKED_STRUCT_BEGIN BitmapFileHeader {
    BitmapFileHeader(int width, int height) {
        size = GetBMPStride(width) * height + 54;
    }

    char type[2] = {'B', 'M'};
    uint32_t size = {};
    uint32_t reserved = 0;
    uint32_t header_size = 54;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    BitmapInfoHeader(int width, int height)
        : width_(width), height_(height) {
          size_image = GetBMPStride(width) * height;
        }

    uint32_t size_ = 40;
    int32_t width_;
    int32_t height_;
    uint16_t planes = 1;
    uint16_t bit_count = 24;
    uint32_t compression = 0;
    uint32_t size_image;
    int32_t res_x = 11811;
    int32_t res_y = 11811;
    int32_t color_used = 0;
    int32_t color_sign = 0x1000000;
}
PACKED_STRUCT_END

bool SaveBMP(const Path& file, const Image& image) {
    int32_t width = image.GetWidth();
    int32_t height = image.GetHeight();

    BitmapInfoHeader info_header(width, height);
    BitmapFileHeader file_header(width, height);
    
    ofstream out_file(file, ios::binary);
    out_file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out_file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    
    int stride = GetBMPStride(width);
    vector<char> buffer(stride);

    cout << "width=" << width << " height=" << height << " stride=" << stride << endl;

    for (int y = height - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < width; ++x) {
            buffer[x * 3] = static_cast<char>(line[x].b);
            buffer[x * 3 + 1] = static_cast<char>(line[x].g);
            buffer[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out_file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }
    return true;
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream in_file(file, ios::binary);
    int width;
    int height;
    
    in_file.ignore(18);
    in_file.read(reinterpret_cast<char*>(&width), sizeof(width));
    in_file.read(reinterpret_cast<char*>(&height), sizeof(height));

    in_file.ignore(28);
    int stride = GetBMPStride(width);
    Image result(stride / 3, height, Color::Black());
    vector<char> buffer(width * 3);

    for (int y = height - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        in_file.read(buffer.data(), stride);
        for (int x = 0; x < width; ++x) {
            line[x].b = static_cast<byte>(buffer[x * 3]);
            line[x].g = static_cast<byte>(buffer[x * 3 + 1]);
            line[x].r = static_cast<byte>(buffer[x * 3 + 2]);
        }
    }
    return result;
}

}  // namespace img_lib