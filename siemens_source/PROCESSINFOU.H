//---------------------------------------------------------------------------

#ifndef processinfouH
#define processinfouH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "CGAUGES.h"
//---------------------------------------------------------------------------
class Tprocessinfo : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TMemo *memo;
        TCGauge *progressbar;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
        __fastcall Tprocessinfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tprocessinfo *processinfo;
//---------------------------------------------------------------------------
#endif
