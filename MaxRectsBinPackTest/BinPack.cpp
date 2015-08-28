#include <vector>

#include "../MaxRectsBinPack.h"
#include "../GuillotineBinPack.h"
#include "../ShelfBinPack.h"
#include "../ShelfNextFitBinPack.h"
#include "../SkylineBinPack.h"
#include "../RectangleBinPack/pack.h"
#include <cstdio>
#include <chrono>

#include "bitmap_image.hpp"

#define __in__
#define __out__
#define __inout__

#define BENCH_MARKS_ENABLED 1

#if BENCH_MARKS_ENABLED
#define BEN_DECL_TOTAL_TIME long long talTime = 0
#define BEN_START milliseconds before = duration_cast< milliseconds >(system_clock::now().time_since_epoch())
#define BEN_END milliseconds after = duration_cast< milliseconds >(system_clock::now().time_since_epoch())
#define BEN_ADD_TOTAL talTime += (after - before).count()
#define BEN_DURATION ((int)(after - before).count())
#define BEN_TOTAL ((int)talTime)
#else
#define BEN_DECL_TOTAL_TIME
#define BEN_START
#define BEN_END
#define BEN_ADD_TOTAL
#define BEN_DURATION -1
#define BEN_TOTAL -1
#endif

typedef bool(*PackRect)(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize);

std::vector<rbp::RectSize> randomRect(rbp::RectSize canvasSize)
{
	using namespace rbp;
	using namespace std;

	static bool randomized = false;
	if (!randomized)
	{
		srand(time(nullptr)/*2837646*/);
		randomized = true;
	}

	int thresshole = (canvasSize.width + canvasSize.height) / 6;
	vector<RectSize> freeRects;
	vector<RectSize> splittedRects;
	freeRects.push_back(canvasSize);

	// Split the whole rect
	while (!freeRects.empty())
	{
		auto &r = freeRects.back();
		freeRects.pop_back();

		if (r.width < 6 || r.height < 6 || r.width + r.height < thresshole)
		{
			r.allowFlip = false;
			if (r.width && r.height) // push if rect is not zero
				splittedRects.push_back(r);
			continue;
		}

		RectSize newRect;
		newRect.width = min(10 + rand() % thresshole, r.width);
		newRect.height = min(10 + rand() % thresshole, r.height);
		newRect.allowFlip = false;
		splittedRects.push_back(newRect);

		auto splitVertical = (r.width - newRect.width) < (r.height - newRect.height);

		RectSize r1, r2;
		if (splitVertical)
		{
			r1 = RectSize{ r.width, r.height - newRect.height };
			r2 = RectSize{ r.width - newRect.width, newRect.height };
		}
		else
		{
			r1 = RectSize{ r.width - newRect.width, r.height };
			r2 = RectSize{ newRect.width, r.height - newRect.height };
		}

		freeRects.push_back(r1);
		freeRects.push_back(r2);
	}

	return splittedRects;
}

bool MaxRectPack(std::vector<rbp::RectSize> &rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize, 
	rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic)
{
	rbp::MaxRectsBinPack bin;
	bin.Init(rectSize.width, rectSize.height);
	bin.Insert(rects, dst, heuristic);
	return rects.empty();
}

bool GuillotinePack(std::vector<rbp::RectSize> &rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize,
	rbp::GuillotineBinPack::FreeRectChoiceHeuristic heuristic, rbp::GuillotineBinPack::GuillotineSplitHeuristic splitHeuristic)
{
	rbp::GuillotineBinPack bin;
	bin.Init(rectSize.width, rectSize.height);
	bin.Insert(rects, true, heuristic, splitHeuristic);

	dst.insert(dst.begin(), bin.GetUsedRectangles().begin(), bin.GetUsedRectangles().end());

	return rects.empty();
}

bool MaxRectBestAreaFit(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return MaxRectPack(rects, dst, rectSize, rbp::MaxRectsBinPack::RectBestAreaFit);
}

bool MaxRectBestLongSideFit(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return MaxRectPack(rects, dst, rectSize, rbp::MaxRectsBinPack::RectBestLongSideFit);
}

bool MaxRectBestShortSideFit(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return MaxRectPack(rects, dst, rectSize, rbp::MaxRectsBinPack::RectBestShortSideFit);
}

bool MaxRectBottomLeftRule(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return MaxRectPack(rects, dst, rectSize, rbp::MaxRectsBinPack::RectBottomLeftRule);
}

bool MaxRectContactPointRule(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return MaxRectPack(rects, dst, rectSize, rbp::MaxRectsBinPack::RectContactPointRule);
}

bool GuillotineRectBestAreaFitSplitLongerAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestAreaFit, rbp::GuillotineBinPack::SplitLongerAxis);
}

bool GuillotineRectBestAreaFitSplitLongerLeftoverAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestAreaFit, rbp::GuillotineBinPack::SplitLongerLeftoverAxis);
}

bool GuillotineRectBestAreaFitSplitMaximizeArea(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestAreaFit, rbp::GuillotineBinPack::SplitMaximizeArea);
}

bool GuillotineRectBestLongSideFitSplitLongerAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestLongSideFit, rbp::GuillotineBinPack::SplitLongerAxis);
}

bool GuillotineRectBestLongSideFitSplitLongerLeftoverAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestLongSideFit, rbp::GuillotineBinPack::SplitLongerLeftoverAxis);
}

bool GuillotineRectBestLongSideFitSplitMaximizeArea(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestLongSideFit, rbp::GuillotineBinPack::SplitMaximizeArea);
}

bool GuillotineRectBestShortSideFitSplitLongerAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestShortSideFit, rbp::GuillotineBinPack::SplitLongerAxis);
}

bool GuillotineRectBestShortSideFitSplitLongerLeftoverAxis(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestShortSideFit, rbp::GuillotineBinPack::SplitLongerLeftoverAxis);
}

bool GuillotineRectBestShortSideFitSplitMaximizeArea(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	return GuillotinePack(rects, dst, rectSize, rbp::GuillotineBinPack::RectBestShortSideFit, rbp::GuillotineBinPack::SplitMaximizeArea);
}

bool packFunc(std::vector<rbp::RectSize> rects, std::vector<rbp::Rect> &dst, rbp::RectSize rectSize)
{
	unsigned totalRects = rects.size();
	rect_xywhf **ptr_rects = new rect_xywhf*[totalRects];
	for (unsigned i = 0; i < totalRects; ++i)
	{
		ptr_rects[i] = new rect_xywhf(0, 0, rects[i].width, rects[i].height);
	}

	std::vector<bin> bins;
	auto success = pack(ptr_rects, (int)rects.size(), rectSize.width, rectSize.height, bins);
	if (!success || bins.size() != 1)
		return false;

	auto& rectsInBin = bins[0].rects;
	for (auto& rib : rectsInBin)
	{
		dst.push_back(rbp::Rect(rib->x, rib->y, rib->w, rib->h, rib->flipped));
	}

	for (unsigned i = 0; i < totalRects; ++i)
	{
		delete ptr_rects[i];
	}
	delete[] ptr_rects;
	return true;
}

int packCalls = 0;
std::vector<rbp::Rect> BinPackingFixWidth(int minH, std::vector<rbp::RectSize> &rects, PackRect packFunc,
	rbp::RectSize &rectSize, bool enlagreIfNotFit = true)
{
	// Why + 1 : tend toward a lagrer bin, better fit (smaller occupy however)
	auto size = rbp::RectSize(rectSize.width, (minH + rectSize.height + 1) / 2);
	std::vector<rbp::Rect> binRects;
	bool fit = packFunc(rects, binRects, size);
	++packCalls;
	if (fit)
	{
		if (minH + 1 == rectSize.height || minH + 2 == rectSize.height) // Terminate recursive condition met
		{
			rectSize.height = size.height;
			return binRects;
		}
		else
		{
			rectSize.height = size.height;
			if (size.height == minH + 1)
			{
				return binRects;
			}
			return BinPackingFixWidth(minH, rects, packFunc, rectSize, enlagreIfNotFit);
		}
	}
	else
	{
		// If this happens, that means initial rectSize is not enough => increase rectSize
		if (minH + 1 == rectSize.height || minH + 2 == rectSize.height)
		{
			rectSize.height += rectSize.height / 40; // more 2.5%
			return enlagreIfNotFit ? BinPackingFixWidth(minH + 1, rects, packFunc, rectSize, enlagreIfNotFit) : binRects;
		}
		else
		{
			// new minH = size.height
			return BinPackingFixWidth(size.height, rects, packFunc, rectSize, enlagreIfNotFit);
		}
	}
}

std::vector<rbp::Rect> BinPackingFixHeight(int minW, std::vector<rbp::RectSize> &rects, PackRect packFunc,
	rbp::RectSize &rectSize, bool enlagreIfNotFit = true)
{
	auto size = rbp::RectSize((minW + rectSize.width + 1) / 2, rectSize.height);
	std::vector<rbp::Rect> binRects;
	bool fit = packFunc(rects, binRects, size);
	++packCalls;
	if (fit)
	{
		if (minW + 1 == rectSize.width || minW + 2 == rectSize.width)
		{
			rectSize.width = size.width;
			return binRects;
		}
		else
		{
			rectSize.width = size.width;
			if (size.width == minW + 1)
			{
				return binRects;
			}
			return BinPackingFixHeight(minW, rects, packFunc, rectSize, enlagreIfNotFit);
		}
	}
	else
	{
		if (minW + 1 == rectSize.width || minW + 2 == rectSize.width)
		{
			rectSize.width += rectSize.width / 40;
			return enlagreIfNotFit ? BinPackingFixHeight(minW + 1, rects, packFunc, rectSize, enlagreIfNotFit) : binRects;
		}
		else
		{
			return BinPackingFixHeight(size.width, rects, packFunc, rectSize, enlagreIfNotFit);
		}
	}
}

/*
 @param rects all rects that needed to pack
 @param packFunc bin packing function algorithm
 @param rectSize max bin size => out: min bin size that contains all rects
 @param occupy output occupy

 @param rectSize (old algorithm) start size that will be grown in each step
*/
#define NEW_ALGORITHM 1
std::vector<rbp::Rect> BinPacking(std::vector<rbp::RectSize> rects, PackRect packFunc,
	__inout__ rbp::RectSize &rectSize, __out__ float &occupy)
{
#if NEW_ALGORITHM
	float area = 0;
	for (auto& r : rects)
	{
		area += r.width * r.height;
	}

	packCalls = 0;
	int minH = (int)(area) / rectSize.width;
	rbp::RectSize sizeFixW = rectSize;
	auto binFixWidth = BinPackingFixWidth(minH, rects, packFunc, sizeFixW);
	rbp::RectSize sizeDecrW = { sizeFixW.width - 1, sizeFixW.height };
	auto binDecrWidth = BinPackingFixHeight(sizeDecrW.width - 4, rects, packFunc, sizeDecrW, false);
	if (sizeDecrW.width < sizeFixW.width)
	{
		sizeFixW.width = sizeDecrW.width;
		binFixWidth = binDecrWidth;
	}
	float occupy1 = area / (sizeFixW.width * sizeFixW.height);

	int minW = (int)(area) / rectSize.height;
	rbp::RectSize sizeFixH = rectSize;
	auto binFixHeight = BinPackingFixHeight(minW, rects, packFunc, sizeFixH);
	rbp::RectSize sizeDecrH = { sizeFixH.width, sizeFixH.height - 1 };
	auto binDecrHeight = BinPackingFixWidth(sizeDecrH.height - 4, rects, packFunc, sizeDecrH, false);
	if (sizeDecrH.width < sizeFixH.width)
	{
		sizeFixH.width = sizeDecrH.width;
		binFixHeight = binDecrHeight;
	}
	float occupy2 = area / (sizeFixH.width * sizeFixH.height);

	if (occupy1 > occupy2)
	{
		occupy = occupy1;
		rectSize.width = sizeFixW.width;
		rectSize.height = sizeFixW.height;
		return binFixWidth;
	}
	occupy = occupy2;
	rectSize.width = sizeFixH.width;
	rectSize.height = sizeFixH.height;
	return binFixHeight;
#else
	//===============================
	// Old algor

	int dw = rectSize.width / 40;
	int dh = rectSize.height / 40;
	float area = 0;
	for (auto& r : rects)
	{
		area += r.width * r.height;
	}
	std::vector<rbp::Rect> result;
	float bestOccupy = 0;
	rbp::RectSize bestSize;
	int calls = 0;
	int bestFoundBy = 0;
	auto rsw = rectSize;
	for (int i = 0; i < 20; ++i)
	{
		std::vector<rbp::Rect> dst;
		++calls;
		if (packFunc(rects, dst, rbp::RectSize(rsw.width + i * dw, rsw.height)))
		{
			rsw.width = rectSize.width + i * dw;
			result = dst;
			bestSize = rsw;
			bestOccupy = area / (rsw.width * rsw.height);
			break;
		}
	}

	int lowerWidth = rsw.width - dw;
	for (int width = rsw.width - 1; width > lowerWidth; --width)
	{
		std::vector<rbp::Rect> dst;
		++calls;
		if (packFunc(rects, dst, rbp::RectSize(width, rsw.height)))
		{
			rsw.width = width;
			result = dst;
			bestSize = rsw;
			bestOccupy = area / (rsw.width * rsw.height);
			bestFoundBy = 1;
		}
		else
		{
			break;
		}
	}

	auto rsh = rectSize;
	for (int i = 0; i < 20; ++i)
	{
		std::vector<rbp::Rect> dst;
		++calls;
		if (packFunc(rects, dst, rbp::RectSize(rectSize.width + i * dh, rectSize.height)))
		{
			rsh.height = rectSize.height + i * dh;
			occupy = area / (rsh.width * rsh.height);
			if (occupy > bestOccupy)
			{
				result = dst;
				bestSize = rsh;
				bestOccupy = occupy;
				bestFoundBy = 2;
			}
			break;
		}
	}

	int lowerHeight = rsh.height - dh;
	for (int height = rsh.height - 1; height > lowerHeight; --height)
	{
		std::vector<rbp::Rect> dst;
		++calls;
		if (packFunc(rects, dst, rbp::RectSize(rsh.width, height)))
		{
			rsh.height = height;
			occupy = area / (rsh.width * rsh.height);
			if (occupy > bestOccupy)
			{
				result = dst;
				bestSize = rsh;
				bestOccupy = occupy;
				bestFoundBy = 3;
			}
		}
		else
		{
			break;
		}
	}
	printf("= %d Calls, best found by\t\t%d\n", calls, bestFoundBy);
	occupy = bestOccupy;
	rectSize.width = bestSize.width;
	rectSize.height = bestSize.height;
	return result;
#endif
}

std::vector<rbp::Rect> PackMultiAlgo(std::vector<rbp::RectSize> &rects, __inout__ rbp::RectSize &rectSize)
{
	using namespace rbp;

	PackRect packFuncs[] = {
		MaxRectBestAreaFit, 
		MaxRectBestLongSideFit,
		MaxRectBestShortSideFit,
		//MaxRectBottomLeftRule, // Not effective
		MaxRectContactPointRule,
		//GuillotineRectBestAreaFitSplitLongerAxis,
		//GuillotineRectBestAreaFitSplitLongerLeftoverAxis,
		//GuillotineRectBestAreaFitSplitMaximizeArea,
		//GuillotineRectBestLongSideFitSplitLongerAxis, 
		//GuillotineRectBestLongSideFitSplitLongerLeftoverAxis, 
		//GuillotineRectBestLongSideFitSplitMaximizeArea, 
		//GuillotineRectBestShortSideFitSplitLongerAxis,
		//GuillotineRectBestShortSideFitSplitLongerLeftoverAxis,
		//GuillotineRectBestShortSideFitSplitMaximizeArea,
		//packFunc
	};

	char* algNames[] = {
		"MaxRectBestAreaFit",
		"MaxRectBestLongSideFit",
		"MaxRectBestShortSideFit",
		//"MaxRectBottomLeftRule",
		"MaxRectContactPointRule",
		//"GuillotineRectBestAreaFitSplitLongerAxis",
		//"GuillotineRectBestAreaFitSplitLongerLeftoverAxis",
		//"GuillotineRectBestAreaFitSplitMaximizeArea",
		//"GuillotineRectBestLongSideFitSplitLongerAxis",
		//"GuillotineRectBestLongSideFitSplitLongerLeftoverAxis",
		//"GuillotineRectBestLongSideFitSplitMaximizeArea",
		//"GuillotineRectBestShortSideFitSplitLongerAxis",
		//"GuillotineRectBestShortSideFitSplitLongerLeftoverAxis",
		//"GuillotineRectBestShortSideFitSplitMaximizeArea",
		//"packFunc"
	};

	using namespace std::chrono;
	float maxOccupy = 0;
	std::vector<rbp::Rect> binRects;
	rbp::RectSize minBinSize;
	BEN_DECL_TOTAL_TIME;
	for (int i = 0; i < sizeof(packFuncs) / sizeof(PackRect); ++i)
	{
		rbp::RectSize optSize = rectSize;
		float occupy = 0;

		BEN_START;
		auto bin = BinPacking(rects, packFuncs[i], optSize, occupy);
		BEN_END;
		BEN_ADD_TOTAL;
		//printf("\t%d millis, %d, %f, %s\n", BEN_DURATION, packCalls, occupy, algNames[i]);

		if (occupy > maxOccupy)
		{
			minBinSize = optSize;
			maxOccupy = occupy;
			binRects = bin;
		}
	}

	rectSize.width = minBinSize.width;
	rectSize.height = minBinSize.height;
	printf("Best %f, (%d; %d), %d millis\n", maxOccupy, rectSize.width, rectSize.height, BEN_TOTAL);
	//for (auto& rr : binRects)
	//{
	//	printf("{x: %d, y: %d, w: %d, h: %d, flipped: %d},\n", rr.x, rr.y, rr.width, rr.height, rr.flipped ? 1 : 0);
	//}
	return binRects;
}

void saveImgs(unsigned int w, unsigned int h)
{
	bitmap_image image(w, h);

	static rgb_store COLORS[] = { {255, 0, 0},{255, 128, 0},{255, 255, 0},{255, 255, 128},{128, 128, 128},{0, 255, 0},{128, 255, 0},{0, 0, 255},{128, 0, 255},{255, 0, 255},{0, 128, 255},{0, 255, 255},{64, 196, 0},{0, 196, 64},{196, 0, 64} };
	static int numColors = sizeof(COLORS) / sizeof(rgb_store);
	static int count = 0;
	rgb_store col = COLORS[count % numColors];

	for (unsigned int x = 0; x < w; ++x)
	{
		for (unsigned int y = 0; y < h; ++y)
		{
			image.set_pixel(x, y, col.red, col.green, col.blue);
		}
	}
	char filename[50];
	sprintf(filename, "D:\\Projects\\RectangleBinPack\\bitmaps\\bm%d.bmp", ++count);
	image.save_image(filename);
}

void autoTest()
{
	using namespace rbp;
	auto rectSize = RectSize{ 600, 600, false };
	auto rects = randomRect(rectSize);

	//rects.pop_back();
	//rects.pop_back();
	//rects.pop_back();

	rects[0].allowFlip = false;

	printf("===============================================\n"
		"Here's a benchmark for _ %u _ Rects\n", rects.size());

	//printf("Data: ");
	//for (auto& r : rects)
	//{
	//	printf("%d %d ", r.width, r.height);
	//}
	//printf("\n");

	PackMultiAlgo(rects, RectSize(1024, 512));
	for (auto& r : rects)
	{
		r.allowFlip = true;
	}
	//printf("____ Allow Flip\n");
	auto bin = PackMultiAlgo(rects, RectSize(1024, 512));
	for (auto& r : bin)
	{
		printf("{x: %d, y: %d, w: %d, h: %d, flipped: %d},\n", r.x, r.y, r.width, r.height, r.flipped);
	}
}

int main(int argc, char **argv)
{
	using namespace rbp;

	if (argc < 5 || argc % 2 != 1)
	{
		printf("Usage: MaxRectsBinPackTest binWidth binHeight w_0 h_0 w_1 h_1 w_2 h_2 ... w_n h_n\n");
		printf("       where binWidth and binHeight define the size of the bin.\n");
		printf("       w_i is the width of the i'th rectangle to pack, and h_i the height.\n");
		printf("Example: MaxRectsBinPackTest 256 256 30 20 50 20 10 80 90 20\n");

		system("cls");

		for (int i = 0; i < 5; i++)
		{
			autoTest();
		}

		system("pause");
		return 0;
	}
	
	
	// Create a bin to pack to, use the bin size from command line.
	MaxRectsBinPack bin;
	int binWidth = atoi(argv[1]);
	int binHeight = atoi(argv[2]);
	printf("Initializing bin to size %dx%d.\n", binWidth, binHeight);
	bin.Init(binWidth, binHeight);
	
	// Pack each rectangle (w_i, h_i) the user inputted on the command line.
	for(int i = 3; i < argc; i += 2)
	{
		// Read next rectangle to pack.
		int rectWidth = atoi(argv[i]);
		int rectHeight = atoi(argv[i+1]);
		printf("Packing rectangle of size %dx%d: ", rectWidth, rectHeight);

		// Perform the packing.
		MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = MaxRectsBinPack::RectBestShortSideFit; // This can be changed individually even for each rectangle packed.
		Rect packedRect = bin.Insert(rectWidth, rectHeight, true, heuristic);

		// Test success or failure.
		if (packedRect.height > 0)
			printf("Packed to (x,y)=(%d,%d), (w,h)=(%d,%d). Free space left: %.2f%%\n", packedRect.x, packedRect.y, packedRect.width, packedRect.height, 100.f - bin.Occupancy()*100.f);
		else
			printf("Failed! Could not find a proper position to pack this rectangle into. Skipping this one.\n");
	}
	printf("Done. All rectangles packed.\n");
}
