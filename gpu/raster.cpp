#include "raster.h"
#include "../math/math.h"

Raster::Raster()
{
}

Raster::~Raster()
{
}

void Raster::rasterize(
	std::vector<VsOutput>& results,
	const uint32_t& drawMode,
	const std::vector<VsOutput>& inputs
) {
	if (drawMode == DRAW_LINES) {
		for (uint32_t i = 0; i < inputs.size(); i+=2) {
			rasterizeLine(results, inputs[i], inputs[i + 1]);
		}
	}else if (drawMode == DRAW_TRIANGLES) {
		for (uint32_t i = 0; i < inputs.size(); i += 3) {
			rasterizeTriangle(results, inputs[i], inputs[i + 1], inputs[i + 2]);
		}
	}
}

void Raster::rasterizeLine(
	std::vector<VsOutput>& results,
	const VsOutput& v0,
	const VsOutput& v1
) {
	VsOutput start = v0;
	VsOutput end = v1;

	//1 保证x方向是从小到大的
	if (start.mPosition.x > end.mPosition.x) {
		auto tmp = start;
		start = end;
		end = tmp;
	}

	results.push_back(start);

	//2 保证y方向也是从小到大，如果需要翻转，必须记录
	bool flipY = false;
	if (start.mPosition.y > end.mPosition.y) {
		start.mPosition.y *= -1.0f;
		end.mPosition.y *= -1.0f;
		flipY = true;
	}

	//3 保证斜率在0~1之间，如果需要调整，必须记录
	int deltaX = static_cast<int>(end.mPosition.x - start.mPosition.x);
	int deltaY = static_cast<int>(end.mPosition.y - start.mPosition.y);

	bool swapXY = false;
	if (deltaX < deltaY) {
		std::swap(start.mPosition.x, start.mPosition.y);
		std::swap(end.mPosition.x, end.mPosition.y);
		std::swap(deltaX, deltaY);
		swapXY = true;
	}

	//4 brensenham
	int currentX = static_cast<int>(start.mPosition.x);
	int currentY = static_cast<int>(start.mPosition.y);

	int resultX = 0;
	int resultY = 0;

	VsOutput currentVsOutput;
	int p = 2 * deltaY - deltaX;

	for (int i = 0; i < deltaX; i++) {
		if (p >= 0) {
			currentY += 1;
			p -= 2 * deltaX;
		}

		currentX += 1;
		p += 2 * deltaY;

		//处理新xy，flip and swap
		resultX = currentX;
		resultY = currentY;
		if (swapXY) {
			std::swap(resultX, resultY);
		}

		if (flipY) {
			resultY *= -1;
		}

		//产生新顶点
		currentVsOutput.mPosition.x = resultX;
		currentVsOutput.mPosition.y = resultY;

		//这里不能传入start和end，因为currentVsOutput点是需要绘制的点，它一定在原始直线v0,v1上，而start,end是从v0,v1变换而来
		interpolantLine(v0, v1, currentVsOutput);

		results.push_back(currentVsOutput);
	}
}

void Raster::interpolantLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target) {
	float weight = 1.0f;
	//if (v1.mPosition.x != v0.mPosition.x) {
	//    //用x做比例
	//    weight = (float)(target.mPosition.x - v0.mPosition.x) / (float)(v1.mPosition.x - v0.mPosition.x);
	//}
	//else if (v1.mPosition.y != v0.mPosition.y) {
	//    //用y做比例
	//    weight = (float)(target.mPosition.y - v0.mPosition.y) / (float)(v1.mPosition.y - v0.mPosition.y);
	//}

	int deltaX = static_cast<int>(v1.mPosition.x - v0.mPosition.x);
	int deltaY = static_cast<int>(v1.mPosition.y - v0.mPosition.y);
	if (abs(deltaX) >= abs(deltaY)) {
		//用x做比例
		weight = (float)(target.mPosition.x - v0.mPosition.x) / deltaX;
	}
	else {
		//用y做比例
		weight = (float)(target.mPosition.y - v0.mPosition.y) / deltaY;
	}

	//std::cout << "weight=" << weight << "--" << static_cast<float>(result.mR) << "--" << static_cast<float>(result.mG) << "--" << static_cast<float>(result.mB) << std::endl;

	//插值1/w，用于透视恢复
	target.mOneOverW = math::lerp(v0.mOneOverW, v1.mOneOverW, weight);

	//插值深度值
	target.mPosition.z = math::lerp(v0.mPosition.z, v1.mPosition.z, weight);

	target.mColor = math::lerp(v0.mColor, v1.mColor, weight);
	target.mUV = math::lerp(v0.mUV, v1.mUV, weight);
}

void Raster::rasterizeTriangle(
	std::vector<VsOutput>& results,
	const VsOutput& v0,
	const VsOutput& v1,
	const VsOutput& v2
) {
	//获取三角形最小包围矩形
	int maxX = static_cast<int>(std::max(v0.mPosition.x, std::max(v1.mPosition.x, v2.mPosition.x)));
	int minX = static_cast<int>(std::min(v0.mPosition.x, std::min(v1.mPosition.x, v2.mPosition.x)));
	int maxY = static_cast<int>(std::max(v0.mPosition.y, std::max(v1.mPosition.y, v2.mPosition.y)));
	int minY = static_cast<int>(std::min(v0.mPosition.y, std::min(v1.mPosition.y, v2.mPosition.y)));

	math::vec2f pv0, pv1, pv2;
	VsOutput result;
	for (int i = minX; i <= maxX; ++i) {
		for (int j = minY; j <= maxY; ++j) {
			pv0 = math::vec2f(v0.mPosition.x - i, v0.mPosition.y - j);
			pv1 = math::vec2f(v1.mPosition.x - i, v1.mPosition.y - j);
			pv2 = math::vec2f(v2.mPosition.x - i, v2.mPosition.y - j);

			auto crossl = math::cross(pv0, pv1);
			auto cross2 = math::cross(pv1, pv2);
			auto cross3 = math::cross(pv2, pv0);

			bool negativeAll = crossl < 0 && cross2 < 0 && cross3 < 0;
			bool positiveAll = crossl > 0 && cross2 > 0 && cross3 > 0;

			if (negativeAll || positiveAll) {
				result.mPosition.x = i;
				result.mPosition.y = j;
				interpolantTriangle(v0, v1, v2, result);

				results.push_back(result);
			}
		}
	}
}

void Raster::interpolantTriangle(
	const VsOutput& v0,
	const VsOutput& v1,
	const VsOutput& v2,
	VsOutput& target
) {
	auto e1 = math::vec2f(v1.mPosition.x - v0.mPosition.x, v1.mPosition.y - v0.mPosition.y);
	auto e2 = math::vec2f(v2.mPosition.x - v0.mPosition.x, v2.mPosition.y - v0.mPosition.y);
	float sumArea = std::abs(math::cross(e1, e2));

	auto pv0 = math::vec2f(v0.mPosition.x - target.mPosition.x, v0.mPosition.y - target.mPosition.y);
	auto pv1 = math::vec2f(v1.mPosition.x - target.mPosition.x, v1.mPosition.y - target.mPosition.y);
	auto pv2 = math::vec2f(v2.mPosition.x - target.mPosition.x, v2.mPosition.y - target.mPosition.y);

	//计算v0的权重
	float vOArea = std::abs(math::cross(pv1, pv2));
	float v1Area = std::abs(math::cross(pv0, pv2));
	float v2Area = std::abs(math::cross(pv0, pv1));

	float weight0 = vOArea / sumArea;
	float weight1 = v1Area / sumArea;
	float weight2 = v2Area / sumArea;

	//插值1/w，用于透视恢复
	target.mOneOverW = math::lerp(v0.mOneOverW, v1.mOneOverW, v2.mOneOverW, weight0, weight1, weight2);

	//插值深度值
	target.mPosition.z = math::lerp(v0.mPosition.z, v1.mPosition.z, v2.mPosition.z, weight0, weight1, weight2);
	
	//对于颜色的插值
	target.mColor = math::lerp(v0.mColor, v1.mColor, v2.mColor, weight0, weight1, weight2);

	//对于法线的插值
	target.mNormal = math::lerp(v0.mNormal, v1.mNormal, v2.mNormal, weight0, weight1, weight2);

	//对于uv坐标的插值
	target.mUV = math::lerp(v0.mUV, v1.mUV, v2.mUV, weight0, weight1, weight2);
}
