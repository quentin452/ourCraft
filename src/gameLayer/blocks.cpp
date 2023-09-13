#include "blocks.h"
#include "rendering/renderer.h"
#include "worldGenerator.h"
#include "blocksLoader.h"

Block* Chunk::safeGet(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || z >= CHUNK_SIZE || y >= CHUNK_HEIGHT)
	{
		return nullptr;
	}
	else
	{
		return &unsafeGet(x, y, z);
	}
}

bool Chunk::bake(Chunk* left, Chunk* right, Chunk* front, Chunk* back)
{
	if (
		dirty
		||(!neighbourToLeft && left != nullptr)
		||(!neighbourToRight && right != nullptr)
		||(!neighbourToFront && front != nullptr)
		||(!neighbourToBack && back != nullptr)
		)
	{
		dirty = 0;
		neighbourToLeft = (left != nullptr);
		neighbourToRight = (right != nullptr);
		neighbourToFront = (front != nullptr);
		neighbourToBack = (back != nullptr);

		opaqueGeometry.clear();

		for (int x = 0; x < CHUNK_SIZE; x++)
			for (int z = 0; z < CHUNK_SIZE; z++)
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					auto& b = unsafeGet(x, y, z);
					if (!b.air())
					{
						auto bfront = safeGet(x, y, z + 1);
						auto bback = safeGet(x, y, z - 1);
						auto btop = safeGet(x, y + 1, z);
						auto bbottom = safeGet(x, y - 1, z);
						auto bleft = safeGet(x - 1, y, z);
						auto bright = safeGet(x + 1, y, z);

						if (bfront == nullptr && front != nullptr)
						{
							bfront = front->safeGet(x, y, 0);
						}

						if (bback == nullptr && back != nullptr)
						{
							bback = back->safeGet(x, y, CHUNK_SIZE - 1);
						}

						if (bleft == nullptr && left != nullptr)
						{
							bleft = left->safeGet(CHUNK_SIZE-1, y, z);
						}

						if (bright == nullptr && right != nullptr)
						{
							bright = right->safeGet(0, y, z);
						}

						Block* sides[6] = {bfront, bback, btop, bbottom, bleft, bright};

						//todo occlude if all of the blocks
						if (b.isGrassMesh())
						{
							for (int i = 6; i <= 9; i++)
							{
								//opaqueGeometry.push_back(mergeShorts(i, b.type));
								opaqueGeometry.push_back(mergeShorts(i, getGpuIdIndexForBlock(b.type, 0)));
								
								//opaqueGeometry.push_back(b.getSkyLight());
								opaqueGeometry.push_back(x + this->data.x * CHUNK_SIZE);
								opaqueGeometry.push_back(y);
								opaqueGeometry.push_back(z + this->data.z * CHUNK_SIZE);
								opaqueGeometry.push_back(15);

							}

						}
						else
						{

							//if (b.isAnimated())
							//{
							//
							//	int a = 0;
							//
							//}

							for (int i = 0; i < 6; i++)
							{
								
								if ((sides[i] != nullptr && !(sides[i])->isOpaque())
									|| ((i == 3 && y == 0) || (i == 2 && y == CHUNK_HEIGHT - 1)
									)
									)
								{
									//opaqueGeometry.push_back(mergeShorts(i + (int)b.isAnimated() * 10, b.type));

									if (b.isAnimated())
									{
										opaqueGeometry.push_back(mergeShorts(i + 10,
											getGpuIdIndexForBlock(b.type, i)));
									}
									else
									{
										opaqueGeometry.push_back(mergeShorts(i,
											getGpuIdIndexForBlock(b.type, i)));
									}
									
									//if (sides[i] != nullptr && i == 2)
									//{
									//	opaqueGeometry.push_back(15);
									//}
									//else if (sides[i] != nullptr && i == 3)
									//{
									//	opaqueGeometry.push_back(0);
									//}
									//else
									//{
									//	opaqueGeometry.push_back(sides[i]->getSkyLight());
									//}
									//opaqueGeometry.push_back(15);
									opaqueGeometry.push_back(x + this->data.x * CHUNK_SIZE);
									opaqueGeometry.push_back(y);
									opaqueGeometry.push_back(z + this->data.z * CHUNK_SIZE);
									opaqueGeometry.push_back(15);

								}
							}
						}

						

					}
				}

		return true;
	}
	else
	{
		return false;
	}
}

void Chunk::create(int x, int z)
{

	this->data.x = x;
	this->data.z = z;

	data.clearLightLevels();

	generateChunk(1234, *this);

}

void ChunkData::clearLightLevels()
{

	for (int x = 0; x < CHUNK_SIZE; x++)
		for (int z = 0; z < CHUNK_SIZE; z++)
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				unsafeGet(x, y, z).lightLevel = 0;
				unsafeGet(x, y, z).notUsed = 0;
			}

}

bool isBlockMesh(uint16_t type)
{
	return !isBlockMesh(type);
}

bool isCrossMesh(uint16_t type)
{
	return type == grass || type == rose;
}
