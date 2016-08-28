//---------------------------------------------------------------------------

#ifndef freiauH
#define freiauH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class Tfreia : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
        __fastcall Tfreia(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfreia *freia;
//---------------------------------------------------------------------------
#endif
