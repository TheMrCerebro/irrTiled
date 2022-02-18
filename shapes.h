/*
 *
 * Copyright (c) 2020-2022 TheMrCerebro
 *
 * irrTiled - Zlib license.
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented;
 * you must not claim that you wrote the original software.
 * If you use this software in a product, an acknowledgment
 * in the product documentation would be appreciated but
 * is not required.
 *
 * 2. Altered source versions must be plainly marked as such,
 * and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any
 * source distribution.
 *
*/

#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

void Shapes()
{
    IrrlichtDevice* device = createDevice(EDT_OPENGL);
    device->setWindowCaption(L"irrTiled v1.1 - by TheMrCerebro");

    IVideoDriver* driver = device->getVideoDriver();

    //======================================================
    irrTiled* tld = new irrTiled("media/shapes.tmx",device);
    //======================================================

    while(device->run())
    {
        driver->beginScene(true, true, SColor(0, 200, 200, 200));

        // Extracts data from all groups of objects.
        for (u32 i = 0; i < tld->ObjectGroup.size(); ++i)
        {
            OBJECTGROUP og = tld->ObjectGroup[i];

            // Extract data from all objects.
            for (u32 j = 0; j < og.Object.size(); ++j)
            {
                OBJECT obj = og.Object[j];

                // Extracts the position and the size of the object.
                s32 x = obj.Pos.X;
                s32 y = obj.Pos.Y;
                s32 w = obj.Size.Width;
                s32 h = obj.Size.Height;

                switch (obj.Shape)
                {
                case SHAPE::S_RECT:// Draw rectangles.
                    driver->draw2DRectangleOutline(recti(obj.Pos, obj.Size), SColor(255, 255, 0, 0));
                    break;
                case SHAPE::S_ELLIPSE:// Draw circles.You should draw ellipses with different sizes but for now you can't do that.
                    driver->draw2DPolygon(vector2di(x+w/2, y+h/2), w/2, SColor(255, 0, 255, 0), 16);
                    break;
                case SHAPE::S_POINT:// Draw points.On this occasion to show them, I have used a crossand a circle.
                {
                    driver->draw2DLine(vector2di(x-4, y), vector2di(x+4, y), SColor(255, 255, 0, 255));
                    driver->draw2DLine(vector2di(x, y-4), vector2di(x, y+4), SColor(255, 255, 0, 255));
                    driver->draw2DPolygon(vector2di(x, y),4, SColor(255, 255, 0, 255));
                }
                break;
                case SHAPE::S_POLYGON:// Draw polygons using the information extracted from the attribute.
                {
                    for (u32 p = 0; p < obj.Points.size(); ++p)
                    {
                        vector2di in, out;
                        in = obj.Points[p];
                        if(p == obj.Points.size()-1)
                            out = obj.Points[0];
                        else
                            out = obj.Points[p + 1];

                        // Draw polygons using lines.
                        driver->draw2DLine(obj.Pos + in, obj.Pos + out, SColor(255, 0, 0, 255));
                    }
                }
                break;
                default:
                    break;
                }
            }
        }

        driver->endScene();
    }

    free(tld);

    device->drop();
}

#endif // SHAPES_H_INCLUDED
