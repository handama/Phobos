#include <YRPP.h>
#include <Helpers/Macro.h>
DEFINE_HOOK(4F4583, GScreenClass_DrawOnTop_TheDarkSideOfTheMoon, 6)
{


	static int idxFrame = 0;

	RectangleStruct boundRect;

	Point2D Position;

	SHPReference* pSHP;

	char* filename = ScenarioClass::Instance->FileName;
	char name[260];
	memset(name, '\0', sizeof(name));

	for (int i = 0; i < strlen(filename) + 1; i++)
	{
		if (filename[i] == '.')
		{
			name[i] = '.';
			name[i + 1] = 'S';
			name[i + 2] = 'H';
			name[i + 3] = 'P';
			name[i + 4] = '\0';
			break;
		}
		else
			name[i] = filename[i];
	}
	pSHP = GameCreate<SHPReference>(name);
	if (!pSHP)
		return 0;
	if (pSHP->Height == 0)
		return 0;

	//游戏界面关联的Surface此时是Composite

	Position = { 0,DSurface::Composite->GetHeight() - pSHP->Height };

	DSurface::Composite->DrawSHP(FileSystem::ANIM_PAL, pSHP, idxFrame, &Position,
		DSurface::Composite->GetRect(&boundRect), BlitterFlags::bf_400, 0, 0, 0, 1000, 0, nullptr, 0, 0, 0);



	++idxFrame %= pSHP->Frames;



	GameDelete(pSHP);
	return 0;
}



