//---------------------------------------------------------------------------

#ifndef miscuH
#define miscuH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OvalBtn.hpp"
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class Tmiscellanousfunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TRadioGroup *miscellanousaction;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall Tmiscellanousfunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tmiscellanousfunctions *miscellanousfunctions;
//---------------------------------------------------------------------------
#endif
