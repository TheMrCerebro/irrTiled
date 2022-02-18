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

#ifndef IRRTILED_H_INCLUDED
#define IRRTILED_H_INCLUDED

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;

enum SHAPE
{
	S_RECT,
	S_ELLIPSE,
	S_POINT,
	S_POLYGON
};

struct PROPERTY
{
	bool BoolValue;
	s32 IntValue;
	s32 ObjectValue;
	f32 FloatValue;
	core::stringc Name;
	core::stringc Type;
	core::stringc FileValue;
	core::stringc StringValue;
	video::SColor ColorValue;
};

struct IMAGE
{
	core::dimension2du Size;
	core::stringc Source;
	video::ITexture* Texture;
};

struct FRAME
{
	s32 TileID;
	s32 Duration;
};

struct OBJECT
{
	s32 ID;
	s32 GID;
	core::position2di Pos;
	s32 Rotation;
	core::dimension2du Size;
	core::array<PROPERTY> Properties;
	SHAPE Shape;
	core::array<core::vector2di> Points;
};

struct OBJECTGROUP
{
	bool Visible;
	s32 ID;
	core::stringc Name;
	video::SColor TintColor;
	core::array<OBJECT> Object;
	core::array<PROPERTY> Properties;
};

struct TILE
{
	s32 ID;
	core::array<FRAME> Animation;
	core::array<OBJECTGROUP> ObjectGroup;
};

struct TILESET
{
	s32 FirstGID;
	core::dimension2du Size;
	s32 TileCount;
	u32 Columns;
	core::stringc Source;
	core::stringc Name;
	IMAGE Image;
	core::array<recti> SubRects;
	core::array<TILE> Tile;
};

struct LAYER
{
	bool Visible;
	s32 ID;
	core::dimension2du Size;
	core::stringc Name;
	core::stringc Encoding;
	video::SColor TintColor;
	core::array<s32> Data;
	core::array<PROPERTY> Properties;
};

struct GROUP
{
	s32 ID;
	core::stringc Name;
	core::array<PROPERTY> Properties;
};

struct IMAGELAYER
{
	s32 ID;
	core::stringc Name;
	bool Visible;
	video::SColor TintColor;
};

//
class irrTiled
{
public:

	// Constructor
	irrTiled(const c8* filename, IrrlichtDevice* device)
	{
		io::IXMLReader* xml = device->getFileSystem()->createXMLReader(filename);

		// Extrae el nombre del directorio donde esta el archivo tmx
		for (s32 i=0; i<core::stringc(filename).findFirst('/')+1; ++i)
			directory += core::stringc(filename)[i];

		dev = device;

		if (!xml)
			printf("File not exist!!!\n");

		while (xml->read())
		{
			if (xml->getNodeType() == io::EXN_ELEMENT)
			{
				// Read Maps information
				if (core::stringc(xml->getNodeName()) == "map")
				{
					//read in the key
					Version = xml->getAttributeValue(L"version");
					TiledVersion = xml->getAttributeValue(L"tiledversion");
					Orientation = xml->getAttributeValue(L"orientation");
					RenderOrder = xml->getAttributeValue(L"renderorder");
					Size.Width = xml->getAttributeValueAsInt(L"width");
					Size.Height = xml->getAttributeValueAsInt(L"height");
					TileSize.Width = xml->getAttributeValueAsInt(L"tilewidth");
					TileSize.Height = xml->getAttributeValueAsInt(L"tileheight");
					Infinite = xml->getAttributeValueAsInt(L"infinite");
					NextLayerID = xml->getAttributeValueAsInt(L"nextlayerid");
					NextObjectID = xml->getAttributeValueAsInt(L"nextobjectid");
					BackgroundColor = HEX2RGB(xml->getAttributeValue(L"backgroundcolor"));

					#if _DEBUG
					printf("MAP[*]\n");
					printf("- Version: %s\n", Version.c_str());
					printf("- Tiled Version: %s\n", TiledVersion.c_str());
					printf("- Orientation: %s\n", Orientation.c_str());
					printf("- Render Order: %s\n", RenderOrder.c_str());
					printf("- Width: %i\n", Size.Width);
					printf("- Height: %i\n", Size.Height);
					printf("- Tile Width: %i\n", TileSize.Width);
					printf("- Tile Height: %i\n", TileSize.Height);
					printf("- Infinite: %i\n", Infinite);
					printf("- Next Layer ID: %i\n", NextLayerID);
					printf("- Next Object ID: %i\n", NextObjectID);
					printf("- Background Color: R/%i, G/%i, B/%i\n\n",
						BackgroundColor.getRed(),
						BackgroundColor.getGreen(),
						BackgroundColor.getBlue());
					#endif
				}

				// Read all Properties of map
				if (core::stringc(xml->getNodeName()) == "properties")
					Properties = readProperties(xml);

				// Read Tileset information
				if (core::stringc(xml->getNodeName()) == "tileset")
				{
					TILESET ts;

					ts.FirstGID = xml->getAttributeValueAsInt(L"firstgid");

					// If the tileset uses a ".TSX" file, a external tileset
					ts.Source = xml->getAttributeValue(L"source");
					if (!ts.Source.empty())
					{
						io::IXMLReader* tsx = device->getFileSystem()->createXMLReader(directory+ts.Source);
						while (tsx->read())
						{
							if (tsx->getNodeType() == io::EXN_ELEMENT)
							{
								// Read Tileset information from .tsx file
								if (core::stringc(tsx->getNodeName()) == "tileset")
								{
									ts.Name = tsx->getAttributeValue(L"name");
									ts.Size.Width = tsx->getAttributeValueAsInt(L"tilewidth");
									ts.Size.Height = tsx->getAttributeValueAsInt(L"tileheight");
									ts.TileCount = tsx->getAttributeValueAsInt(L"tilecount");
									ts.Columns = tsx->getAttributeValueAsInt(L"columns");

									while (tsx->read())
									{
										if (tsx->getNodeType() == io::EXN_ELEMENT)
										{
											// Read Image information
											if (core::stringc(tsx->getNodeName()) == "image")
												ts.Image = readImages(tsx);

											// Read Tile information
											if (core::stringc(tsx->getNodeName()) == "tile")
											{
												TILE tld;
												tld.ID = tsx->getAttributeValueAsInt(L"id");

												#if _DEBUG
												printf("TILE [*]\n");
												printf("- ID: %i\n\n", tld.ID);
												#endif

												while (tsx->read())
												{
													if (tsx->getNodeType() == io::EXN_ELEMENT)
													{
														// Read Object Groups information
														if (core::stringc(tsx->getNodeName()) == "objectgroup")
															tld.ObjectGroup.push_back(readObjectGroups(tsx));

														// Read Animations information
														if (core::stringc(tsx->getNodeName()) == "animation")
														{
															#if _DEBUG
															printf("ANIMATION [*]\n\n");
															#endif

															while (tsx->read())
															{
																if (tsx->getNodeType() == io::EXN_ELEMENT)
																{
																	// Read Frames information
																	if (core::stringc(tsx->getNodeName()) == "frame")
																	{
																		FRAME frm;

																		frm.TileID = tsx->getAttributeValueAsInt(L"tileid");
																		frm.Duration = tsx->getAttributeValueAsInt(L"duration");

																		#if _DEBUG
																		printf("FRAME [*]\n");
																		printf("- TileID: %i\n", frm.TileID);
																		printf("- Duration: %i\n\n", frm.Duration);
																		#endif

																		tld.Animation.push_back(frm);
																	}
																}
																else
																	break;
															}
														}
													}
													else
														break;
												}

												ts.Tile.push_back(tld);
											}
										}
										else
											break;
									}
								}
							}
						}
						tsx->drop();
					}
					else
					{
						ts.Name = xml->getAttributeValue(L"name");
						ts.Size.Width = xml->getAttributeValueAsInt(L"tilewidth");
						ts.Size.Height = xml->getAttributeValueAsInt(L"tileheight");
						ts.TileCount = xml->getAttributeValueAsInt(L"tilecount");
						ts.Columns = xml->getAttributeValueAsInt(L"columns");

						while (xml->read())
						{
							// Read Image information
							if (xml->getNodeType() == io::EXN_ELEMENT)
							{
								if (core::stringc(xml->getNodeName()) == "image")
									ts.Image = readImages(xml);
							}
							else
								break;
						}
					}

					#if _DEBUG
					printf("TILESET[*]\n");
					printf("- First GID: %i\n", ts.FirstGID);
					printf("- Source: %s\n", ts.Source.c_str());
					printf("- Name: %s\n", ts.Name.c_str());
					printf("- Tile Width: %i\n", ts.Size.Width);
					printf("- Tile Height: %i\n", ts.Size.Height);
					printf("- Tile Count: %i\n", ts.TileCount);
					printf("- Columns: %i\n\n", ts.Columns);
					#endif

					// tiles/subrects are counted from 0, left to right, top to bottom
					for (u32 y=0; y<ts.TileCount/ts.Columns; ++y)
					{
						for (u32 x=0; x<ts.Columns; ++x)
							ts.SubRects.push_back(recti(vector2di(x * ts.Size.Width, y * ts.Size.Height), ts.Size));
					}

					Tileset.push_back(ts);
				}

				// Read Layer information
				if (core::stringc(xml->getNodeName()) == "layer")
					Layer.push_back(readLayers(xml));

				// Read Object Groups information
				if (core::stringc(xml->getNodeName()) == "objectgroup")
					ObjectGroup.push_back(readObjectGroups(xml));

				// Read Image Layer information
				if (core::stringc(xml->getNodeName()) == "imagelayer")
					ImageLayer.push_back(readImageLayers(xml));

				// Read Group information
				if (core::stringc(xml->getNodeName()) == "group")
					Group.push_back(readGroups(xml));
			}
		}

		xml->drop();
	}

	// Clear all :-(
	~irrTiled()
	{
		Properties.clear();
		Tileset.clear();
		Layer.clear();
		ObjectGroup.clear();
		Group.clear();
		ImageLayer.clear();
	}

	// Variables
	core::stringc Version;
	core::stringc TiledVersion;
	core::stringc Orientation;
	core::stringc RenderOrder;

	core::dimension2du Size;
	core::dimension2du TileSize;

	s32 Infinite=0;
	s32 NextLayerID=0;
	s32 NextObjectID=0;

	video::SColor BackgroundColor;

	core::array<PROPERTY>    Properties;
	core::array<TILESET>     Tileset;
	core::array<LAYER>       Layer;
	core::array<OBJECTGROUP> ObjectGroup;
	core::array<GROUP>       Group;
	core::array<IMAGELAYER>  ImageLayer;

private:

	// Convert from hex to rgb
	video::SColor HEX2RGB(core::stringc value)
	{
		video::SColor color(255, 255, 255, 255);
		value.remove("#");

		if (!value.empty())
		{
			u8* pin = (u8*)value.c_str();
			u32 out = 0;

			while (*pin != 0)
			{
				out <<= 4;
				out += (*pin < 'A') ? *pin & 0xF : (*pin & 0x7) + 9;
				pin++;
			}

			color = out;
		}
		return color;
	}

	// Read Image Layer information
	IMAGELAYER readImageLayers(io::IXMLReader* xml)
	{
		IMAGELAYER iml;

		iml.ID = xml->getAttributeValueAsInt(L"id");
		iml.Name = xml->getAttributeValue(L"name");
		iml.Visible = xml->getAttributeValueAsInt(L"Visible") ? true : false;
		iml.TintColor = HEX2RGB(xml->getAttributeValue(L"TintColor"));

		#if _DEBUG
		printf("IMAGE LAYER[*]\n");
		printf("- ID: %i\n", iml.ID);
		printf("- Name: %s\n", iml.Name.c_str());
		printf("- Visible: %s\n", iml.Visible ? "True" : "False");
		printf("- Tint Color: R/%i, G/%i, B/%i\n\n",
			iml.TintColor.getRed(),
			iml.TintColor.getGreen(),
			iml.TintColor.getBlue());
		#endif

		return iml;
	}

	// Read Group information
	GROUP readGroups(io::IXMLReader* xml)
	{
		GROUP grp;

		grp.ID = xml->getAttributeValueAsInt(L"id");
		grp.Name = xml->getAttributeValue(L"name");

		#if _DEBUG
		printf("GROUP[*]\n");
		printf("- ID: %i\n", grp.ID);
		printf("- Name: %s\n", grp.Name.c_str());
		#endif

		if (!xml->isEmptyElement())
		{
			while (xml->read())
			{
				if (xml->getNodeType() == io::EXN_ELEMENT)
				{
					// Read all Properties of map
					if (core::stringc(xml->getNodeName()) == "properties")
						grp.Properties = readProperties(xml);

					// Read Layer information
					if (core::stringc(xml->getNodeName()) == "layer")
						Layer.push_back(readLayers(xml));

					// Read Object Groups information
					if (core::stringc(xml->getNodeName()) == "objectgroup")
						ObjectGroup.push_back(readObjectGroups(xml));

					// Read Image Layer information
					if (core::stringc(xml->getNodeName()) == "imagelayer")
						ImageLayer.push_back(readImageLayers(xml));

					// Read Groups information
					if (core::stringc(xml->getNodeName()) == "group")
						Group.push_back(readGroups(xml));
				}
			}
		}

		return grp;
	}

	// Read Layer information
	LAYER readLayers(io::IXMLReader* xml)
	{
		LAYER lyr;

		lyr.ID = xml->getAttributeValueAsInt(L"id");
		lyr.Name = xml->getAttributeValue(L"name");
		lyr.Size.Width = xml->getAttributeValueAsInt(L"width");
		lyr.Size.Height = xml->getAttributeValueAsInt(L"height");
		lyr.Visible = xml->getAttributeValueAsInt(L"Visible") ? true : false;
		lyr.TintColor = HEX2RGB(xml->getAttributeValue(L"TintColor"));

		#if _DEBUG
		printf("LAYER[*]\n");
		printf("- ID: %i\n", lyr.ID);
		printf("- Name: %s\n", lyr.Name.c_str());
		printf("- Width: %i\n", lyr.Size.Width);
		printf("- Height: %i\n", lyr.Size.Height);
		printf("- Visible: %s\n", lyr.Visible ? "True" : "False");
		printf("- Tint Color: R/%i, G/%i, B/%i\n\n",
			lyr.TintColor.getRed(),
			lyr.TintColor.getGreen(),
			lyr.TintColor.getBlue());
		#endif

		while (xml->read())
		{
			if (xml->getNodeType() == io::EXN_ELEMENT)
			{
				// Read Data
				if (core::stringc(xml->getNodeName()) == "data")
				{
					lyr.Encoding = xml->getAttributeValue(L"encoding");

					// Read XML data (deprecated)
					if(lyr.Encoding.empty())
					{
						while (xml->read())
						{
							if (xml->getNodeType() == io::EXN_ELEMENT)
							{
								if (core::stringc(xml->getNodeName()) == "tile")
									lyr.Data.push_back(xml->getAttributeValueAsInt(L"gid"));
							}
							else
								break;
						}
					}

					#if _DEBUG
					printf("DATA[*]\n");
					printf("- Encoding: %s\n", lyr.Encoding.c_str());
					#endif
				}

				// Read all Properties (if exist)
				if (core::stringc(xml->getNodeName()) == "properties")
					lyr.Properties = readProperties(xml);
			}
			else if (xml->getNodeType() == io::EXN_TEXT)
			{
				core::stringc data = xml->getNodeName();

				#if _DEBUG
				printf("- Data: %s\n", data.c_str());
				#endif

				// Read CSV data
				if (lyr.Encoding == "csv")
				{
					const c8* ptr = data.c_str();
					while (true)
					{
						c8* end;
						u32 res = strtoul(ptr, &end, 10);
						if (end == ptr) break;
						ptr = end;
						lyr.Data.push_back(res);
						if (*ptr == ',') ++ptr;
					}
				}

				// Read Base64 data
				if (lyr.Encoding == "base64")
				{
					core::stringc out, in = data;
					in.remove("\n");
					in.remove("\r");
					in.remove(" ");

					c8 T[256];
					for (u32 i = 0; i < 64; i++)
						T[ L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i] ] = i;

					s32 val = 0, valb = -8;
					for (u32 c = 0; c < in.size(); ++c)
					{
					    u32 s = in[c];

						if (T[ s ] == -1) break;

						val = (val << 6) + T[ s ];
						valb += 6;
						if (valb >= 0)
						{
							out += c8((val >> valb) & 0xFF);
							valb -= 8;
						}
					}

					u32* p = (u32*)out.c_str();
					for (u32 i = 0; i < (out.size() / sizeof(u32)); i++)
						lyr.Data.push_back(p[i]);
				}
			}
			else
				break;
		}

		return lyr;
	}

	// Read image information
	IMAGE readImages(io::IXMLReader* xml)
	{
		IMAGE img;

		img.Source = xml->getAttributeValue(L"source");
		img.Texture = dev->getVideoDriver()->getTexture(directory+img.Source);
		img.Size.Width = xml->getAttributeValueAsInt(L"width");
		img.Size.Height = xml->getAttributeValueAsInt(L"height");

		// Make color transparent
		video::SColor col = HEX2RGB(xml->getAttributeValue(L"trans"));
		dev->getVideoDriver()->makeColorKeyTexture(img.Texture, col);

		#if _DEBUG
		printf("IMAGE[*]\n");
		printf("- source: %s\n", img.Source.c_str());
		printf("- Width: %i\n", img.Size.Width);
		printf("- Height: %i\n", img.Size.Height);
		printf("- Tansparent: R/%i, G/%i, B/%i\n\n",col.getRed(),col.getGreen(),col.getBlue());
		#endif

		return img;
	}

	// Read all properties
	core::array<PROPERTY> readProperties(io::IXMLReader* xml)
	{
		core::array<PROPERTY> pro;
		while (xml->read())
		{
			if (xml->getNodeType() == io::EXN_ELEMENT)
			{
				if (core::stringc(xml->getNodeName()) == "property")
				{
					PROPERTY pry;

					pry.Name = xml->getAttributeValue(L"name");
					pry.Type = xml->getAttributeValue(L"type");

					#if _DEBUG
					printf("PROPERTY[*]\n");
					printf("Name: %s\n", pry.Name.c_str());
					printf("Type: %s\n", pry.Type.c_str());
					#endif

					if (pry.Type == "bool")
					{
						pry.BoolValue = xml->getAttributeValueAsInt(L"value") ? false : true;
						#if _DEBUG
						printf("Value: %s\n", pry.BoolValue ? "False" : "True");
						#endif
					}
					else if (pry.Type == "float")
					{
						pry.FloatValue = xml->getAttributeValueAsFloat(L"value");
						#if _DEBUG
						printf("Value: %.3f\n", pry.FloatValue);
						#endif
					}
					else if (pry.Type == "int")
					{
						pry.IntValue = xml->getAttributeValueAsInt(L"value");
						#if _DEBUG
						printf("Value: %i\n", pry.IntValue);
						#endif
					}
					else if (pry.Type == "object")
					{
						pry.ObjectValue = xml->getAttributeValueAsInt(L"value");
						#if _DEBUG
						printf("Value: %i\n", pry.ObjectValue);
						#endif
					}
					else if (pry.Type == "file")
					{
						pry.FileValue = xml->getAttributeValue(L"value");
						#if _DEBUG
						printf("Value: %s\n", pry.FileValue.c_str());
						#endif
					}
					else if (pry.Type == "color")
					{
						pry.ColorValue = HEX2RGB(xml->getAttributeValue(L"value"));
						#if _DEBUG
						printf("Value: R/%i, G/%i, B/%i\n",pry.ColorValue.getRed(),pry.ColorValue.getGreen(),pry.ColorValue.getBlue());
						#endif
					}
					else
					{
						pry.StringValue = xml->getAttributeValue(L"value");
						#if _DEBUG
						printf("Value: %s\n", pry.StringValue.c_str());
						#endif
					}

					printf("\n");
					pro.push_back(pry);
				}
			}
			else
				break;
		}
		return pro;
	}

	// Read Object Groups information
	OBJECTGROUP readObjectGroups(io::IXMLReader* xml)
	{
		OBJECTGROUP og;

		og.ID = xml->getAttributeValueAsInt(L"id");
		og.Name = xml->getAttributeValue(L"name");
		og.Visible = xml->getAttributeValueAsInt(L"Visible") ? true : false;
		og.TintColor = HEX2RGB(xml->getAttributeValue(L"TintColor"));

		#if _DEBUG
		printf("OBJECT GROUP[*]\n");
		printf("- ID: %i\n", og.ID);
		printf("- Name: %s\n", og.Name.c_str());
		printf("- Visible: %s\n", og.Visible ? "True" : "False");
		printf("- Tint Color: R/%i, G/%i, B/%i\n\n",
			og.TintColor.getRed(),
			og.TintColor.getGreen(),
			og.TintColor.getBlue());
		#endif

		// Read Objects information
		if (!xml->isEmptyElement())
		{
			while (xml->read())
			{
				if (xml->getNodeType() == io::EXN_ELEMENT)
				{
					// Read all Properties
					if (core::stringc(xml->getNodeName()) == "properties")
						og.Properties = readProperties(xml);

					// Read all objects
					if (core::stringc(xml->getNodeName()) == "object")
					{
						OBJECT obj;

						obj.ID = xml->getAttributeValueAsInt(L"id");
						obj.GID = xml->getAttributeValueAsInt(L"gid");
						obj.Pos.X = xml->getAttributeValueAsInt(L"x");
						obj.Pos.Y = xml->getAttributeValueAsInt(L"y");
						obj.Size.Width = xml->getAttributeValueAsInt(L"width");
						obj.Size.Height = xml->getAttributeValueAsInt(L"height");
						obj.Rotation = xml->getAttributeValueAsInt(L"rotation");
						obj.Shape = SHAPE::S_RECT;

						#if _DEBUG
						printf("OBJECT[*]\n");
						printf("- ID: %i\n", obj.ID);
						printf("- X: %i\n", obj.Pos.X);
						printf("- Y: %i\n", obj.Pos.Y);
						printf("- Rotation: %i\n", obj.Rotation);
						printf("- Width: %i\n", obj.Size.Width);
						printf("- Height: %i\n\n", obj.Size.Height);
						#endif

						// Read all Properties (if exist)
						if (!xml->isEmptyElement())
						{
							while (xml->read())
							{
								if (xml->getNodeType() == io::EXN_ELEMENT)
								{
									if (core::stringc(xml->getNodeName()) == "properties")
										obj.Properties = readProperties(xml);

									if (core::stringc(xml->getNodeName()) == "ellipse")
										obj.Shape = SHAPE::S_ELLIPSE;

									if (core::stringc(xml->getNodeName()) == "point")
										obj.Shape = SHAPE::S_POINT;

									if (core::stringc(xml->getNodeName()) == "polygon")
									{
										obj.Shape = SHAPE::S_POLYGON;

										core::array<core::stringc> ac;
										core::stringc(xml->getAttributeValue(L"points")).split(ac, " ", 4);

										for (u32 x=0; x<ac.size(); ++x)
										{
											core::vector2di pos;
											sscanf_s(ac[x].c_str(), "%i,%i", &pos.X, &pos.Y);
											obj.Points.push_back(pos);
											#if _DEBUG
											printf("Point: %i,%i\n", pos.X, pos.Y);
											#endif
										}
									}
								}
								else
									break;
							}
						}

						og.Object.push_back(obj);
					}
				}
				else
					break;
			}
		}

		return og;
	}

private:

	core::stringc directory;
	irr::IrrlichtDevice* dev;

};

#endif // IRRTILED_H_INCLUDED
