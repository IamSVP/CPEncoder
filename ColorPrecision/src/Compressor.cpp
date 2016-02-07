/*
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
*/

// Refer to "Real-Time DXT Compression" by J.M.P. van Waveren for a more thorough discussion of the
// algorithms used in this code.

#include "CPEncoder/CPCompressor.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "CPEncoder/stb_image.h"
typedef std::basic_ofstream<unsigned char, std::char_traits<unsigned char> > uofstream;
#define INSET_SHIFT 4 // Inset the bounding box with (range >> shift).
#define C565_5_MASK 0xF8 // 0xFF minus last three bits
#define C565_6_MASK 0xFC // 0xFF minus last two bits

namespace CPD{


	void ExtractBlock(const uint8* block, int BlockSz, uint8* decompressedBlock){


	}

	void DecompressImageCP(const uint8* blocks, int w, int h, int BlockSz){

		uint8* OutPixels = (uint8*)malloc(w* h * 4);

		for (int j = 0; j < h; j+=4){
			for (int i = 0; i < w; i+=4){

				int blockNum = j * (w / 4) + i;
				int blockOffset = blockNum * BlockSz;
				uint8 * block;
				


			}
		}



	}


}

namespace CPC{

	// Function prototypes
	
	void ExtractBlock(const uint32* inPtr, uint32 width, uint8* colorBlock);
	void GetMinMaxRGBA(const uint8* colorBlock, uint8* minColor, uint8* maxColor);
	void ComputeColorIndices(const uint8* block, const uint8* minColor, const uint8* maxColor, uint8*& outBuf);
	uint8* CompressImageCP(uint8* ImageData, int w, int h, int blockSz){
		uint8 block[64];
		uint8 minColor[4];
		uint8 maxColor[4];
		
		const uint32 startBlock = 0;
		int  vWidth, vHeight, vBlocksz, vNumBlocks;
		vWidth = w;
		vHeight = h;
		vBlocksz = blockSz;
		vNumBlocks = ((vWidth + 3) / 4)  * ((vHeight + 3) / 4);
		unsigned char *outptr = (uint8 *)malloc(vBlocksz*vNumBlocks);
		uint8 *outBuf = outptr;
		std::cout << vWidth << vHeight << vBlocksz << vNumBlocks << std::endl;
		const uint32 *inPixels = reinterpret_cast<const uint32*>(ImageData);
		
			
		for (uint32 j = 0; j < vHeight; j += 4) {
			
			for (uint32 i = 0; i < vWidth; i += 4) {

				const uint32 kOffset = j*vWidth + i;
				ExtractBlock(inPixels + kOffset, vWidth, block);
				GetMinMaxRGBA(block,minColor, maxColor);
				EmitWord(outBuf, ColorTo565(minColor));
				EmitWord(outBuf, ColorTo565(maxColor));
				ComputeColorIndices(block, minColor, maxColor, outBuf);

			}
			
	
		}
		char file[256];
		sprintf(file, "out\\out%05d.CP",1) ;
		std::ofstream outfile;
		outfile.open(file, std::ios::out | std::ios::binary);
		outfile.write(reinterpret_cast<char*>(outptr), vBlocksz*vNumBlocks);
		outfile.close();
		return outBuf;
	}

	void ExtractBlock(const uint32* inPtr, uint32 width, uint8* colorBlock){

		for (int j = 0; j < 4; j++){
			memcpy(&colorBlock[j * 4 * 4], inPtr, 4 * 4);
			inPtr += width;
		}
	}

	void GetMinMaxRGBA(const uint8* block, uint8* minColor, uint8* maxColor){

		minColor[0] = minColor[1] = minColor[2] = 255;
		maxColor[0] = maxColor[1] = maxColor[2] = 0;

		for (uint32 i = 0; i < 16; i++){
			if (block[i * 4 + 0] < minColor[0])
				minColor[0] = block[i * 4 + 0];
			if (block[i * 4 + 1] < minColor[1])
				minColor[1] = block[i * 4 + 1];
			if (block[i * 4 + 2] < minColor[2])
				minColor[2] = block[i * 4 + 2];
			if (block[i * 4 + 0] > maxColor[0])
				maxColor[0] = block[i * 4 + 0];
			if (block[i * 4 + 1] > maxColor[1])
				maxColor[1] = block[i * 4 + 1];
			if (block[i * 4 + 2] > maxColor[2])
				maxColor[2] = block[i * 4 + 2];
		}


	}
	void ComputeColorIndices(const uint8* block, const uint8* minColor, const uint8* maxColor, uint8*& outBuf){

		uint16 colors[4][4];
		uint32 resultR, resultG, resultB;
		resultR = resultG = resultB = 0;

		colors[0][0] = (minColor[0] & C565_5_MASK) | (minColor[0] >> 5);
		colors[0][1] = (minColor[1] & C565_6_MASK) | (minColor[1] >> 6);
		colors[0][2] = (minColor[2] & C565_5_MASK) | (minColor[2] >> 5);

		colors[3][0] = (maxColor[0] & C565_5_MASK) | (maxColor[0] >> 5);
		colors[3][1] = (maxColor[1] & C565_6_MASK) | (maxColor[1] >> 6);
		colors[3][2] = (maxColor[2] & C565_5_MASK) | (maxColor[2] >> 5);

		colors[1][0] = (2 * colors[0][0] + colors[3][0]) / 3;
		colors[1][1] = (2 * colors[0][1] + colors[3][1]) / 3;
		colors[1][2] = (2 * colors[0][2] + colors[3][2]) / 3;

		colors[2][0] = (colors[0][0] + 2 * colors[3][0]) / 3;
		colors[2][1] = (colors[0][1] + 2 * colors[3][1]) / 3;
		colors[2][2] = (colors[0][2] + 2 * colors[3][2]) / 3;

		for (int i = 15; i >= 0; i--){			// danggers of infinite loops using unsigned intergers

			int32 cr = block[i * 4 + 0];
			int32 cg = block[i * 4 + 1];
			int32 cb = block[i * 4 + 2];

			int32 d0 = abs(colors[0][0] - cr);
			int32 d1 = abs(colors[1][0] - cr);
			int32 d2 = abs(colors[2][0] - cr);
			int32 d3 = abs(colors[3][0] - cr);

			int32 b0 = d0 > d3;
			int32 b1 = d1 > d2;
			int32 b2 = d0 > d2;
			int32 b3 = d1 > d3;
			int32 b4 = d2 > d3;
			int32 b5 = d0 > d1;
			int32 b6 = d2 > d1;


			int32 x0 = b1 & b2;
			int32 x1 = b0 & b3;
			int32 x2 = b0 & b4;
			int32 x3 = b5 & b6;
			resultR |= ((x2 | x3) | ((x0 | x1) << 1)) << (i << 1);
			
			d0 = abs(colors[0][1] - cg);
			d1 = abs(colors[1][1] - cg);
			d2 = abs(colors[2][1] - cg);
			d3 = abs(colors[3][1] - cg);

			b0 = d0 > d3;
			b1 = d1 > d2;
			b2 = d0 > d2;
			b3 = d1 > d3;
			b4 = d2 > d3;
			b5 = d0 > d1;
			b6 = d2 > d1;

			x0 = b1 & b2;
			x1 = b0 & b3;
			x2 = b0 & b4;
			x3 = b5 & b6;

			resultG |= ((x2 | x3) | ((x0 | x1) << 1)) << (i << 1);

			d0 = abs(colors[0][2] - cb);
			d1 = abs(colors[1][2] - cb);
			d2 = abs(colors[2][2] - cb);
			d3 = abs(colors[3][2] - cb);

			b0 = d0 > d3;
			b1 = d1 > d2;
			b2 = d0 > d2;
			b3 = d1 > d3;
			b4 = d2 > d3;
			b5 = d0 > d1;
			b6 = d2 > d1;

			x0 = b1 & b2;
			x1 = b0 & b3;
			x2 = b0 & b4;
			x3 = b5 & b6;

			resultB |= ((x2 | x3) | ((x0 | x1) << 1)) << (i << 1);

		}

		EmitDoubleWord(outBuf, resultR);
		EmitDoubleWord(outBuf, resultG);
		EmitDoubleWord(outBuf, resultB);
	}

	void EmitByte(uint8 *& dest, uint8 b){
		dest[0] = b;
		dest += 1;

	}
	void EmitWord(uint8 *& dest, uint16 w){

		dest[0] = (w >> 0) & 255;
		dest[1] = (w >> 8) & 255;

		dest += 2;
	}
	void EmitDoubleWord(uint8 *& dest, uint32 i){

		dest[0] = (i >> 0) & 255;
		dest[1] = (i >> 8) & 255;
		dest[2] = (i >> 16) & 255;
		dest[3] = (i >> 24) & 255;

		dest += 4;
	}

	// Convert a color in 24-bit RGB888 format to 16-bit RGB565 format.
	uint16 ColorTo565(const uint8* color)
	{
		return ((color[0] >> 3) << 11) | ((color[1] >> 2) << 5) | (color[2] >> 3);
	}

}

int main(int argc, char *argv[]){

	char filename[256];

	int img_num = 1;
	sprintf(filename, "Kodak\\png\\Kodak%05d.png", img_num);

	int w, h, n;
	uint8 * ImageData = stbi_load(filename,&w,&h,&n,4);
	
	uint8 * output= CPC::CompressImageCP(ImageData,w,h,16);
	
	return 0;

}

