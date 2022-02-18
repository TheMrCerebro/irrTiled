/*********************************************************************
Copyright (c) 2020-2022 TheMrCerebro

irrTiled - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include "irrTiled.h"

void Animation()
{
    IrrlichtDevice* device = createDevice(EDT_OPENGL);
    device->setWindowCaption(L"irrTiled v1.1 - by TheMrCerebro");

    IVideoDriver* driver = device->getVideoDriver();

    //=========================================================
    irrTiled* tld = new irrTiled("media/animation.tmx",device);
    //=========================================================

	ITimer* timer = device->getTimer();
	u32 currentFrameIndex = 0;
	f32 currentFrameTime = 0;
	f32 deltaTime, oldTime = 0;

    while(device->run())
    {
        driver->beginScene(true, true, SColor(0,200,200,200));

		// For the animations I use a "delta time".
		deltaTime = timer->getTime() - oldTime;

		// Extracts data from all tile sets.
		for (u32 i = 0; i < tld->Tileset.size(); ++i)
		{
			TILESET ts = tld->Tileset[i];

			// Extracts data from all layers.
			for (u32 j = 0; j < tld->Layer.size(); ++j)
			{
				LAYER lyr = tld->Layer[j];

				// The array "lyr.data" contains all the ids of the tiles, in this way it can be used to identify
				// in an image the position of the section to be drawn.
				for (u32 d = 0; d < lyr.Data.size(); ++d)
				{
					s32 id = lyr.Data[d] - ts.FirstGID;
					if (id >= 0)
					{
						// Tile position.
						s32 x = (d % lyr.Size.Width) * tld->TileSize.Width;
						s32 y = (d / lyr.Size.Width) * tld->TileSize.Height;

						// Draw the tile.
						driver->draw2DImage(ts.Image.Texture, vector2di(x, y), ts.SubRects[id], 0, lyr.TintColor, true);

						// This zone corresponds to the tiles extracted when using an external tileset.
						// In this case, the animation data they contain will be used.
						for (u32 j = 0; j < ts.Tile.size(); ++j)
						{
							TILE tl = ts.Tile[j];

							// In this way it is possible to make some tiles animated.
							// Sure there are other ways, this is just an example.
							if (tl.ID == id && !tl.Animation.empty())
							{
								currentFrameTime += 1.0f / deltaTime;

								s32 tileid = tl.Animation[currentFrameIndex].TileID;
								s32 duration = tl.Animation[currentFrameIndex].Duration;

								if (currentFrameTime >= duration)
								{
									currentFrameTime = 0.f;

									if (currentFrameIndex == (tl.Animation.size() - 1))
										currentFrameIndex = 0;
									else
										++currentFrameIndex;
								}

								// Draw the animation.
								driver->draw2DImage(ts.Image.Texture, vector2di(x, y), ts.SubRects[tileid], 0, lyr.TintColor, true);
							}
						}

					}
				}
			}
		}

		oldTime = timer->getTime();

        driver->endScene();
    }

    free(tld);

    device->drop();
}

#endif // ANIMATION_H_INCLUDED
