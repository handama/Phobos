#include "DigitalDisplayTypeClass.h"

#include <Utilities/TemplateDef.h>

Enumerable<DigitalDisplayTypeClass>::container_t Enumerable<DigitalDisplayTypeClass>::Array;

const char* Enumerable<DigitalDisplayTypeClass>::GetMainSection()
{
	return "DigitalDisplayTypes";
}

void DigitalDisplayTypeClass::LoadFromINI(CCINIClass * pINI)
{
	const char* section = this->Name;

	INI_EX exINI(pINI);

	this->Text_ColorHigh.Read(exINI, section, "Text.ColorHigh");
	this->Text_ColorMid.Read(exINI, section, "Text.ColorMid");
	this->Text_ColorLow.Read(exINI, section, "Text.ColorLow");
	this->Text_Background.Read(exINI, section, "Text.Background");
	this->Offset.Read(exINI, section, "Offset");
	this->Offset_WithoutShield.Read(exINI, section, "Offset.WithoutShield");
	this->UseSHP.Read(exINI, section, "UseSHP");
	this->SHP_SHPFile.Read(pINI, section, "SHP.SHPFile");
	this->SHP_PALFile.Read(pINI, section, "SHP.PALFile");
	this->SHP_Interval.Read(exINI, section, "SHP.Interval");
	this->SHP_Interval_Building.Read(exINI, section, "SHP.Interval.Building");
	this->Align.Read(pINI, section, "Align");
	this->Percentage.Read(exINI, section, "Percentage");
	this->HideStrength.Read(exINI, section, "HideStrength");

	if (strcmp(Align.data(), "Left") == 0)
		Alignment = AlignType::Left;
	else if (strcmp(Align.data(), "Right") == 0)
		Alignment = AlignType::Right;
	else if (strcmp(Align.data(), "Center") == 0)
		Alignment = AlignType::Center;
	else
		Alignment = AlignType::Default;

	if (UseSHP.Get())
	{
		if (strcmp(SHP_SHPFile.data(), "") != 0)
			SHPFile = FileSystem::LoadSHPFile(SHP_SHPFile.data());
		
		if (strcmp(SHP_PALFile.data(), "") != 0)
			PALFile = FileSystem::LoadPALFile(SHP_PALFile.data(), DSurface::Composite);
		else
			PALFile = FileSystem::PALETTE_PAL;
	}
}

template <typename T>
void DigitalDisplayTypeClass::Serialize(T& Stm)
{
	Stm
		.Process(this->Text_ColorHigh)
		.Process(this->Text_ColorMid)
		.Process(this->Text_ColorLow)
		.Process(this->Text_Background)
		.Process(this->Offset)
		.Process(this->Offset_WithoutShield)
		.Process(this->UseSHP)
		.Process(this->SHP_SHPFile)
		.Process(this->SHP_PALFile)
		.Process(this->SHP_Interval)
		.Process(this->SHP_Interval_Building)
		.Process(this->Align)
		.Process(this->Alignment)
		.Process(this->Percentage)
		.Process(this->HideStrength)
		;
}

void DigitalDisplayTypeClass::LoadFromStream(PhobosStreamReader& Stm)
{
	this->Serialize(Stm);

	if (UseSHP.Get())
	{
		if (strcmp(SHP_SHPFile.data(), "") != 0)
			SHPFile = FileSystem::LoadSHPFile(SHP_SHPFile.data());

		if (strcmp(SHP_PALFile.data(), "") != 0)
			PALFile = FileSystem::LoadPALFile(SHP_PALFile.data(), DSurface::Composite);
		else
			PALFile = FileSystem::PALETTE_PAL;
	}
}

void DigitalDisplayTypeClass::SaveToStream(PhobosStreamWriter& Stm)
{
	this->Serialize(Stm);
}
