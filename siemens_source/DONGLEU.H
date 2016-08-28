//---------------------------------------------------------------------------

#ifndef dongleuH
#define dongleuH
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
class Tdonglefunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TRadioGroup *dongleaction;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall Tdonglefunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tdonglefunctions *donglefunctions;
//---------------------------------------------------------------------------
#endif
