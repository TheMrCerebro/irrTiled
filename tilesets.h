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

#ifndef TILESETS_H_INCLUDED
#define TILESETS_H_INCLUDED

#include "irrTiled.h"

void Tilesets()
{
    IrrlichtDevice* device = createDevice(EDT_OPENGL);
    device->setWindowCaption(L"irrTiled v1.1 - by TheMrCerebro");

    IVideoDriver* driver = device->getVideoDriver();

    //========================================================
    irrTiled* tld = new irrTiled("media/tilesets.tmx",device);
    //========================================================

    while(device->run())
    {
        driver->beginScene(true, true, SColor(0,200,200,200));

		// Extracts data from all layers.
		for (u32 j = 0; j < tld->Layer.size(); ++j)
		{
			LAYER lyr = tld->Layer[j];

			// The array "lyr.data" contains all the ids of the tiles, in this way it can be used to identify
			// in an image the position of the section to be drawn.
			for (u32 d = 0; d < lyr.Data.size(); ++d)
			{
				// Extracts data from all tile sets.
				for (u32 i = 0; i < tld->Tileset.size(); ++i)
				{
					TILESET ts = tld->Tileset[i];

					s32 id = lyr.Data[d] - ts.FirstGID;
					if (id >= 0 && id < ts.TileCount)
					{
						// Tile position.
						s32 x = (d % lyr.Size.Width) * tld->TileSize.Width;
						s32 y = (d / lyr.Size.Width) * tld->TileSize.Height;

						// Draw the tile.
						driver->draw2DImage(ts.Image.Texture, vector2di(x, y), ts.SubRects[id], 0, lyr.TintColor, true);
					}
				}
			}
		}

        driver->endScene();
    }

    free(tld);

    device->drop();
}

#endif // TILESETS_H_INCLUDED
