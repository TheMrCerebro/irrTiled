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

#include <iostream>

using namespace std;

#include "animation.h"
#include "basic2d.h"
#include "basic3d.h"
#include "levels.h"
#include "objects.h"
#include "property.h"
#include "shapes.h"
#include "tilesets.h"

int main(int argc, char** argv)
{
    cout << "-----------------------------------\n";
    cout << "[ irrTiled v1.1 - by TheMrCerebro ]\n";
    cout << "-----------------------------------\n";
    cout << "\n";
	cout << "Please select the example you want:\n";
    cout << "(1) - Basic 2D\n";
    cout << "(2) - Basic 3D\n";
    cout << "(3) - Animation\n";
    cout << "(4) - Levels\n";
    cout << "(5) - Objects\n";
    cout << "(6) - Shapes\n";
    cout << "(7) - Tilesets\n";
    cout << "(8) - Property\n";
    cout << "\n";

    int i;
    cin >> i;

	switch(i)
	{
    case 1: Basic2D();
        break;
    case 2: Basic3D();
        break;
    case 3: Animation();
        break;
    case 4: Levels();
        break;
    case 5: Objects();
        break;
    case 6: Shapes();
        break;
    case 7: Tilesets();
        break;
    case 8:Property();
        break;
	}

    return 0;
}
