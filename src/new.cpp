#include <YRPP.h>
#include <Helpers/Macro.h>
DEFINE_HOOK(4F4583, GScreenClass_DrawOnTop_TheDarkSideOfTheMoon, 6) {


    static int idxFrame = 0;

    RectangleStruct boundRect;

    Point2D Position;

    SHPReference* pSHP;

    char* filename = ScenarioClass::Instance->FileName;
    int length = 0;
    for (int i = 0; i < sizeof(filename); i++)
    {
        if (filename[i] == '.')
        {
            length = i + 1 + 4;
        }
    }
    char* name = new char[length];
    for (int i = 0; i < sizeof(filename); i++)
    {
        if (filename[i] != '.')
            name[i] = filename[i];
        else
        {
            name[i] = '.';
            name[i+1] = 'S';
            name[i+2] = 'H';
            name[i+3] = 'P';
            break;
        }
    }
    pSHP = GameCreate<SHPReference>(name);
    delete[]name; name = NULL;

    if (!pSHP)

        return 0;

    //游戏界面关联的Surface此时是Composite

    Position = { 0,DSurface::Composite->GetHeight() - pSHP->Height};

    DSurface::Composite->DrawSHP(FileSystem::ANIM_PAL, pSHP, idxFrame, &Position,
        DSurface::Composite->GetRect(&boundRect), BlitterFlags::TransLucent25, 0, 0, 0, 1000, 0, nullptr, 0, 0, 0);



    ++idxFrame %= pSHP->Frames;



    GameDelete(pSHP);
    return 0;
}




