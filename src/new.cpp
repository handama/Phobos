#include <YRPP.h>
#include <Helpers/Macro.h>
DEFINE_HOOK(4F4583, GScreenClass_DrawOnTop_TheDarkSideOfTheMoon, 6) {


    static int idxFrame = 0;

    RectangleStruct boundRect;

    Point2D Position;

    SHPReference* pSHP;

    if (strcmp(ScenarioClass::Instance->FileName, "RSOV01.MAP") == 0)
        pSHP = GameCreate<SHPReference>("cghitori_a.SHP");
    else pSHP = nullptr;

    if (!pSHP)

        return 0;

    //游戏界面关联的Surface此时是Composite

    Position = { 0,DSurface::Composite->GetHeight() - pSHP->Height - 20 };

    DSurface::Composite->DrawSHP(FileSystem::ANIM_PAL, pSHP, idxFrame, &Position,
        DSurface::Composite->GetRect(&boundRect), BlitterFlags::TransLucent25, 0, 0, 0, 1000, 0, nullptr, 0, 0, 0);



    ++idxFrame %= pSHP->Frames;



    GameDelete(pSHP);
    return 0;
}




